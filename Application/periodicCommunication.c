/******************************************************************************

 @file  periodicCommunication.c

 @brief This file contains the Code of motor control / Mobile App interfacing


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
#include "dataAnalysis.h"
#include "simple_peripheral.h"
#include <icall.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
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
 *********************************************************************/
static void periodicCommunication_setGatt(uint16_t serviceUUID, uint8_t charteristics, uint8_t payloadLength, uint8_t* payload);
/*********************************************************************
 * @fn      periodicCommunication_start
 *
 * @brief   Initialization timer for the Motor Control periodic sampling.
 *
 * @param   none
 *
 * @return  none
 *********************************************************************/
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
 *********************************************************************/
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
 *********************************************************************/
void periodicCommunication_toggle()
{
    //switch(state)
    //{
    //case PERIODIC_COMMUNICATION_DEACTIVATE:
    if(state == PERIODIC_COMMUNICATION_DEACTIVATE)
    {
        motorcontrol_hftimerManager->timerStart();
        motorcontrol_lftimerManager->timerStart();
        state = PERIODIC_COMMUNICATION_ACTIVATE;
        //break;
    }
    //case PERIODIC_COMMUNICATION_ACTIVATE:
    else if(state == PERIODIC_COMMUNICATION_ACTIVATE)
    {
        motorcontrol_hftimerManager->timerStop();
        motorcontrol_lftimerManager->timerStop();
        state = PERIODIC_COMMUNICATION_DEACTIVATE;
        //break;
    }
    //default:
    //break;
}
/*********************************************************************
 * @fn      periodicCommunication_registerTimer
 *
 * @brief   Initialization timer for the Motor Control periodic sampling.
 *
 * @param   none
 *
 * @return  none
 *********************************************************************/
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
 *********************************************************************/
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
 *********************************************************************/
void periodicCommunication_registerBLE_Gatt(simplePeripheral_bleCBs_t *obj)
{
    motorcontrol_BLE_GATT = obj;
}

/*********************************************************************
 * @fn      periodicCommunication_hf_communication
 *
 * @brief   This function will execute every PERIODIC_COMMUNICATION_HF_SAMPLING_TIME -> controlled by UDHAL_TIM4.c (timer 4).
 *          High frequency communication is for tasks that are require to execute at regular
 *          high frequency intervals.  This includes:
 *          Retrieving data such as battery voltage, battery current, Heat Sink Temperature, motor rpm etc. from controller.
 *          Speed is also sent to dashboard and app (if connected) at this time interval.
 *
 * @param   none
 *
 * @return  none
 *********************************************************************/
static uint16_t rpm[DATA_ANALYSIS_POINTS] = {0};                        //revolutions per minute data collected every time interval
static uint16_t speed_cmph[DATA_ANALYSIS_POINTS] = {0};                 //rpm is converted to cm per second (cm/s)
static uint16_t batteryCurrent_mA[DATA_ANALYSIS_POINTS] = {0};          //battery current data collected at every hf communication interval
static uint16_t batteryVoltage_mV[DATA_ANALYSIS_POINTS] = {0};          //battery voltage data collected at every hf communication interval
static int8_t heatSinkTemperature_C[DATA_ANALYSIS_POINTS] = {0};        // temperature can be negative
static int8_t MotorTemperature_C[DATA_ANALYSIS_POINTS] = {0};           // temperature can be negative

static uint16_t (*ptrRpm)[DATA_ANALYSIS_POINTS] = &rpm;
static uint16_t (*ptrSpeed)[DATA_ANALYSIS_POINTS] = &speed_cmph;
static uint16_t (*ptrBatteryVoltage)[DATA_ANALYSIS_POINTS] = &batteryVoltage_mV;
static uint16_t (*ptrBatteryCurrent)[DATA_ANALYSIS_POINTS] = &batteryCurrent_mA;
static int8_t (*ptrhst)[DATA_ANALYSIS_POINTS] = &heatSinkTemperature_C;
static int8_t (*ptrmt)[DATA_ANALYSIS_POINTS] = &MotorTemperature_C;

uint8_t x_hf;
uint16_t x_tt = 0;                              // for debugging only
void periodicCommunication_hf_communication()
{
    /*************************************************
     *  Get Bus Voltage
     *  Get Bus Current
     *  Get RPM
     *  Get Heatsink Temperature
     *  Get Motor Temperature
     *************************************************/
    //STM32MCP_getRegisterFrame(STM32MCP_MOTOR_1_ID,STM32MCP_BUS_VOLTAGE_REG_ID);
    // Sim Battery Voltage
    *((*ptrBatteryVoltage) + x_hf) = 6000 *sin(M_PI * x_tt /180) + 36000;                      // get battery voltage from MCU:  unit in mV
    //STM32MCP_getRegisterFrame(STM32MCP_MOTOR_1_ID,STM32MCP_BUS_CURRENT_REG_ID);       // Need to create a getRegisterFrame for battery current
    // Sim Battery Current
    *((*ptrBatteryCurrent) + x_hf) = 3000; //rand()%13 * 1000;                                 // get battery current from MCU:  unit in mA
    //STM32MCP_getRegisterFrame(STM32MCP_MOTOR_1_ID,STM32MCP_SPEED_MEASURED_REG_ID);      // is speed in RPM
    // Sim RPM
    *((*ptrRpm) + x_hf) = 350 * sin(M_PI * x_tt / 60) + 380;                                   // get RPM from MCU:  unit in rpm,  188 rpm @ r = 0.1016m => 200 cm/sec = 7 km/hr
    //STM32MCP_getRegisterFrame(STM32MCP_MOTOR_1_ID,STM32MCP_HEATSINK_TEMPERATURE_REG_ID);
    // Sim Heatsink Temp
    *((*ptrhst) + x_hf) = 20 *sin(M_PI * x_tt /180) + 15;                                      // get heat sink temperature from MCU: unit in degrees Celsius
    //STM32MCP_getRegisterFrame(STM32MCP_MOTOR_1_ID,STM32MCP_MOTOR_TEMPERATURE_REG_ID);
    // Sim Motor Temp
    *((*ptrmt) + x_hf) = 30 *sin(M_PI * x_tt /180) + 25;                                       // get motor temperature from MCU: unit in degrees Celsius
    // Calculate Speed from RPM
    *((*ptrSpeed) + x_hf) = round(rpm[x_hf] * 2 * (float) M_PI / 60 * WHEELRADIUS);          // Unit in cm / sec

    x_tt++;

    // ---> go to new function to gab dummy data
   // dataAnalysis_Main();
    x_hf++;
    if (x_hf > DATA_ANALYSIS_POINTS)
    {
        x_hf = 0;
    }

}
/*********************************************************************
 * @fn      periodicCommunication_lf_communication
 *
 * @brief   This function will execute after each lf timer overflow  -> controlled by UDHAL_TIM5.c (timer 5)
 *          Low frequency communication is for communicating Power On Time
 *          low frequency intervals.
 *
 * @param
 *
 * @return  none
 *********************************************************************/
uint8_t x_lf;

void periodicCommunication_lf_communication()
{
    x_lf++;
}
/*********************************************************************
 * @fn      periodicCommunication_getxhf
 *
 * @brief   Returns the value of x_hf
 *
 * @param
 *
 * @return  x_hf
 *********************************************************************/
uint8_t periodicCommunication_getxhf()
{
    return (x_hf);
}
/*********************************************************************
 * @fn      periodicCommunication_getxlf
 *
 * @brief   Returns the value of x_lf
 *
 * @param
 *
 * @return  x_lf
 *********************************************************************/
uint8_t periodicCommunication_getxlf()
{
    return (x_lf);
}
/*********************************************************************
 * @fn      periodicCommunication_setGatt
 *
 * @brief   Set the gatt
 *
 * @param   serviceUUID - the service UUID
 *          charteristics - the charteristics (Not UUID, just the index)
 *          payloadLength - the length of the payload
 *          payload - just the payload
 *
 * @return  TRUE or FALSE
 *********************************************************************/
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
