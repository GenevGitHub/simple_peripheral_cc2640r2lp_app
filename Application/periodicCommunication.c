/******************************************************************************

 @file  periodicCommunication.c

 @brief This file contains the Code of motor control interfacing


 *****************************************************************************/

/*********************************************************************
 * INCLUDES
 */
/* This Header file contains all BLE API and icall structure definition */
#include "STM32MCP/STM32MCP.h"
#include "motorControl.h"
#include "periodicCommunication.h"
#include "simple_peripheral.h"
#include "Controller.h"
#include <icall.h>
#include <string.h>
#include <stdlib.h>
/*********************************************************************
 * CONSTANTS
 */
/*********************************************************************
 * GLOBAL VARIABLES
 */
/*********************************************************************
 * LOCAL VARIABLES
 */
static motorcontrol_timerManager_t *motorcontrol_hftimerManager;
static motorcontrol_timerManager_t *motorcontrol_lftimerManager;
static simplePeripheral_bleCBs_t *motorcontrol_BLE_GATT;
static uint8_t state = PERIODIC_COMMUNICATION_DEACTIVATE;
/**********************************************************************
 *  Local functions
 */
static void periodicCommunication_setGatt(uint16_t serviceUUID, uint8_t charteristics, uint8_t payloadLength, uint8_t* payload);
/*********************************************************************
 * @fn      periodicCommunication_start
 *
 * @brief   Initialization timer for the Motor Control periodic sampling.
 *
 * @param   none
 *
 * @return  none
 */
void periodicCommunication_start()
{
    if(state == PERIODIC_COMMUNICATION_DEACTIVATE)
    {
        motorcontrol_hftimerManager->timerStart();
        motorcontrol_lftimerManager->timerStart();
        state = PERIODIC_COMMUNICATION_ACTIVATE;
    }
}

/*********************************************************************
 * @fn      periodicCommunication_stop
 *
 * @brief   Initialization timer for the Motor Control periodic sampling.
 *
 * @param   none
 *
 * @return  none
 */
void periodicCommunication_stop()
{
    if(state == PERIODIC_COMMUNICATION_ACTIVATE)
    {
        motorcontrol_hftimerManager->timerStop();
        motorcontrol_lftimerManager->timerStop();
        state = PERIODIC_COMMUNICATION_DEACTIVATE;
    }
}
/*********************************************************************
 * @fn      periodicCommunication_toggle
 *
 * @brief   Initialization timer for the Motor Control periodic sampling.
 *
 * @param   none
 *
 * @return  none
 */
void periodicCommunication_toggle()
{
    if(state == PERIODIC_COMMUNICATION_DEACTIVATE)
    {
        motorcontrol_hftimerManager->timerStart();
        motorcontrol_lftimerManager->timerStart();
        state = PERIODIC_COMMUNICATION_ACTIVATE;
    }
    else if(state == PERIODIC_COMMUNICATION_ACTIVATE)
    {
        motorcontrol_hftimerManager->timerStop();
        motorcontrol_lftimerManager->timerStop();
        state = PERIODIC_COMMUNICATION_DEACTIVATE;
    }
}
/*********************************************************************
 * @fn      periodicCommunication_registerTimer
 *
 * @brief   Initialization timer for the Motor Control periodic sampling.
 *
 * @param   none
 *
 * @return  none
 */
void periodicCommunication_register_hfTimer(motorcontrol_timerManager_t *obj)
{
    motorcontrol_hftimerManager = obj;
}
/*********************************************************************
 * @fn      periodicCommunication_registerTimer
 *
 * @brief   Initialization timer for the Motor Control periodic sampling.
 *
 * @param   none
 *
 * @return  none
 */
void periodicCommunication_register_lfTimer(motorcontrol_timerManager_t *obj)
{
    motorcontrol_lftimerManager = obj;
}
/*********************************************************************
 * @fn      periodicCommunication_registerTimer
 *
 * @brief   Initialization timer for the Motor Control periodic sampling.
 *
 * @param   none
 *
 * @return  none
 */
void periodicCommunication_registerBLE_Gatt(simplePeripheral_bleCBs_t *obj)
{
    motorcontrol_BLE_GATT = obj;
}
/*********************************************************************
 * @fn      periodicCommunication_hf_communication
 *
 * @brief   This function will execute every 200ms
 *
 * @param
 */
void periodicCommunication_hf_communication()
{
    STM32MCP_getRegisterFrame(STM32MCP_MOTOR_1_ID,STM32MCP_BUS_VOLTAGE_REG_ID);
    //STM32MCP_getRegisterFrame(STM32MCP_MOTOR_1_ID,STM32MCP_BUS_CURRENT_REG_ID);       // Need to create a getRegisterFrame for battery current
    STM32MCP_getRegisterFrame(STM32MCP_MOTOR_1_ID,STM32MCP_SPEED_MEASURED_REG_ID);
    STM32MCP_getRegisterFrame(STM32MCP_MOTOR_1_ID,STM32MCP_HEATSINK_TEMPERATURE_REG_ID);
    STM32MCP_getRegisterFrame(STM32MCP_MOTOR_1_ID,STM32MCP_SPEED_MEASURED_REG_ID);
}
/*********************************************************************
 * @fn      periodicCommunication_hf_communication
 *
 * @brief   This function will execute every 200ms
 *
 * @param
 */
void periodicCommunication_lf_communication()
{
    //uint16_t rpm = 1500;
    //periodicCommunication_setGatt(CONTROLLER_SERV_UUID, CONTROLLER_MOTOR_RPM, CONTROLLER_MOTOR_RPM_LEN, (uint8_t *) &rpm);
}
/*********************************************************************
 * @fn      motorcontrol_setGatt
 * @brief   Set the gatt
 *
 * @param   serviceUUID - the service UUID
 *          charteristics - the charteristics (Not UUID, just the index)
 *          payloadLength - the length of the payload
 *          payload - just the payload
 *
 * @return  TRUE or FALSE
 */
static void periodicCommunication_setGatt(uint16_t serviceUUID, uint8_t charteristics, uint8_t payloadLength, uint8_t* payload)
{
    uint8_t *theMessage = ICall_malloc(sizeof(uint8_t)*(payloadLength + 4));
    if (theMessage)
    {
        theMessage[0] = (serviceUUID >> 0) & 0xFF;
        theMessage[1] = (serviceUUID >> 8) & 0xFF;
        theMessage[2] = charteristics;
        theMessage[3] = payloadLength;
        memcpy(theMessage + 4, payload, payloadLength);
        motorcontrol_BLE_GATT->enqueueMsg(SBP_MC_GATT_EVT, 0, theMessage);
    }
}
