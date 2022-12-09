/******************************************************************************

 @file  motorControl.c

 @brief This file contains the Code of motor control interfacing


 *****************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include <string.h>
#include <stdlib.h>
#include <icall.h>
#include "UDHAL/UDHAL.h"
#include "STM32MCP/STM32MCP.h"
#include "Controller.h"
#include "Dashboard.h"
#include "Battery.h"
#include "motorControl.h"
#include "util.h"
#include "periodicCommunication.h"
#include "simple_peripheral.h"
#include "brakeAndThrottle.h"
#include "ledControl.h"
#include "lightControl.h"
#include "buzzerControl.h"
#include "dataAnalysis.h"
#include "singleButton/singleButton.h"
#include "peripheral.h"
/*********************************************************************
 * CONSTANTS
 */
/*********************************************************************
 * GLOBAL VARIABLES
 */
/*********************************************************************
 * LOCAL VARIABLES
 */
static simplePeripheral_bleCBs_t *motorcontrol_bleCBs;

/**********************************************************************
 *  Local functions
 */
static void motorcontrol_processGetRegisterFrameMsg(uint8_t *txPayload, uint8_t txPayloadLength, uint8_t *rxPayload, uint8_t rxPayloadLength);
static void motorcontrol_rxMsgCb(uint8_t *rxMsg, STM32MCP_txMsgNode_t *STM32MCP_txMsgNode);
static void motorcontrol_exMsgCb(uint8_t exceptionCode);
static void motorcontrol_erMsgCb(uint8_t errorCode);
static void motorcontrol_brakeAndThrottleCB(uint16_t throttleRatio, uint16_t errorMsg);
static void motorcontrol_controllerCB(uint8_t paramID);
static void motorcontrol_dashboardCB(uint8_t paramID);
static void motorcontrol_singleButtonCB(uint8_t messageID);
static void motorcontrol_setGatt(uint16_t serviceUUID, uint8_t charteristics, uint8_t payloadLength, uint8_t* payload);
/*********************************************************************
 * TYPEDEFS
 */
static STM32MCP_CBs_t STM32MCP_CBs =
{
     motorcontrol_rxMsgCb,
     motorcontrol_exMsgCb,
     motorcontrol_erMsgCb
};

static ControllerCBs_t ControllerCBs =
{
     motorcontrol_controllerCB
};

static DashboardCBs_t DashboardCBs =
{
     motorcontrol_dashboardCB
};

static singleButtonCBs_t singleButtonCBs =
{
     motorcontrol_singleButtonCB
};

static brakeAndThrottle_CBs_t brakeAndThrottle_CBs =
{
     motorcontrol_brakeAndThrottleCB
};

//static lightControl_CBs_t lightControl_CBs =
//{
//     motorcontrol_lightControlCB
//};

//static buzzerControl_CBs_t buzzerControl_CBs =
//{
//     motorcontrol_buzzerControlCB
//};

/*********************************************************************
 * @fn      MC_init
 *
 * @brief   Initialization function for the Motor Control Task.
 *
 * @param   none
 *
 * @return  none
 */
void motorcontrol_init(void)
{
    UDHAL_init();
    //Controller_RegisterAppCBs(&ControllerCBs);
    //Dashboard_RegisterAppCBs(&DashboardCBs);

    //STM32MCP_init();
    //STM32MCP_registerCBs(&STM32MCP_CBs);
    //STM32MCP_startCommunication();

    //periodicCommunication_start();

    //brakeAndThrottle_init();
    //brakeAndThrottle_registerCBs(&brakeAndThrottle_CBs);
    //brakeAndThrottle_start();

    singleButton_Init();
    singleButton_registerCBs(&singleButtonCBs);

    dataAnalysis_Init();
    //dataAnalysis_registerCBs(&dataAnalysisCBs);

    lightControl_init();
    //lightControl_registerCBs(&lightControlCBs);

    buzzerControl_init();
    //buzzerControl_registerCBs(&buzzerControlCBs);

    ledControl_Init();

}
/*********************************************************************
 * @fn      motorcontrol_registerCB
 *
 * @brief   When the motor controller sends the get register frame message
 *
 * @param   STM32MCP_rxMsg_t - The memory message to the received message, the size of the message is the second index rxMsg[1]
 *
 * @return  None.
 */
void motorcontrol_registerCB(simplePeripheral_bleCBs_t *obj)
{
    motorcontrol_bleCBs = obj;
}
/*********************************************************************
 * @fn      motorcontrol_processGetRegisterFrameMsg
 *
 * @brief   When the motor controller sends the get register frame message
 *
 * @param   STM32MCP_rxMsg_t - The memory message to the received message, the size of the message is the second index rxMsg[1]
 *
 * @return  None.
 */
static void motorcontrol_processGetRegisterFrameMsg(uint8_t *txPayload, uint8_t txPayloadLength, uint8_t *rxPayload, uint8_t rxPayloadLength)
{
    uint8_t regID = txPayload[0];
    switch(regID)
    {
    case STM32MCP_BUS_VOLTAGE_REG_ID:
        {
            // voltage must be in mV
            uint16_t voltage = *((uint16_t*) rxPayload) * 1000; // rxPayload in V, voltage in mV
            // this is battery percentage.  batteryLevel is used for LED display battery bar - renamed batteryLevel to batteryPercentage.  Chee 13/11/2022
            uint8_t batteryPercentage = (uint8_t) ((((uint32_t)voltage - STM32MCP_SYSTEM_MIMIMUM_VOLTAGE)*100/(STM32MCP_SYSTEM_MAXIMUM_VOLTAGE - STM32MCP_SYSTEM_MIMIMUM_VOLTAGE)) & 0xFF);
            motorcontrol_setGatt(BATTERY_SERV_UUID, BATTERY_BATTERY_LEVEL, BATTERY_BATTERY_LEVEL_LEN, (uint8_t *) &batteryPercentage);
            motorcontrol_setGatt(BATTERY_SERV_UUID, BATTERY_BATTERY_VOLTAGE, BATTERY_BATTERY_VOLTAGE_LEN, (uint8_t *) &voltage);
            break;
        }
        // !!!!!!!!!!!!!!!!!!!!!!!!!  current sensor to be added to MCU.  Reserved case for current measurement
//    case STM32MCP_BUS_CURRENT_REG_ID:
//        {
              // keep current in mV - do not convert it to A
//            uint16_t current = *((uint8_t*) rxPayload) * 1000;
//            motorcontrol_setGatt(BATTERY_SERV_UUID, BATTERY_BATTERY_CURRENT, BATTERY_BATTERY_CURRENT_LEN, (uint8_t *) &current);
//            break;
//        }
    case STM32MCP_HEATSINK_TEMPERATURE_REG_ID:
        {
            uint8_t temperature = (uint8_t) (*((uint8_t*) rxPayload) & 0xFF);
            motorcontrol_setGatt(CONTROLLER_SERV_UUID, CONTROLLER_HEAT_SINK_TEMPERATURE, CONTROLLER_HEAT_SINK_TEMPERATURE_LEN, (uint8_t *) &temperature);
            break;
        }
    case STM32MCP_SPEED_MEASURED_REG_ID:
        {
            int32_t rawRPM = *((int32_t*) rxPayload);
            uint8_t speedNorm = 25;         // !!!!!!!!!!  what is speedNorm?  speed (in cm per sec) = rpm * 2 * pi / 60 * wheelRadius (cm) = rpm / 0.93989.  SpeedNorm = 0.93989 rpm/cm/s
            if(rawRPM >= 0)
            {
                uint16_t rpm = (uint16_t) (rawRPM & 0xFFFF);
                uint16_t speed = rpm/(uint16_t)speedNorm;       // !!!!!!!! how does rpm/speedNorm give us speed?  speed (in cm/s) = 2 * pi * rpm / 60 * wheelRadius (in cm)
                motorcontrol_setGatt(CONTROLLER_SERV_UUID, CONTROLLER_MOTOR_RPM, CONTROLLER_MOTOR_RPM_LEN, (uint8_t *) &rpm);
                motorcontrol_setGatt(CONTROLLER_SERV_UUID, CONTROLLER_MOTOR_SPEED, CONTROLLER_MOTOR_SPEED_LEN, (uint8_t *) &speed);
            }
            break;
        }
    default:
            break;
    }
}
/*********************************************************************
 * @fn      motorcontrol_rxMsgCb
 *
 * @brief   When the motor controller sends the feedback message back, it reaches here
 *
 * @param   rxMsg - The memory message to the received message, the size of the message is the second index rxMsg[1]
 *          motorID - The motor ID described in STM32MCP
 *          frameID - The frame ID described in STM32MCP
 *
 * @return  None.
 */
static void motorcontrol_rxMsgCb(uint8_t *rxMsg, STM32MCP_txMsgNode_t *STM32MCP_txMsgNode)
{
    uint8_t frameID = STM32MCP_txMsgNode->txMsg[0] & 0x1F;
    uint8_t rxPayloadLength = rxMsg[1];
    uint8_t *rxPayload = (uint8_t*)malloc(sizeof(uint8_t)*rxPayloadLength);
    memcpy(rxPayload, rxMsg + 2, rxPayloadLength);

    uint8_t txPayloadLength = STM32MCP_txMsgNode->size;
    uint8_t *txPayload = (uint8_t*)malloc(sizeof(uint8_t)*txPayloadLength);
    memcpy(txPayload, STM32MCP_txMsgNode->txMsg + 2, txPayloadLength);

    switch(frameID)
    {
    case STM32MCP_SET_REGISTER_FRAME_ID:
        break;
    case STM32MCP_GET_REGISTER_FRAME_ID:
        motorcontrol_processGetRegisterFrameMsg(txPayload, txPayloadLength, rxPayload, rxPayloadLength);
        break;
    case STM32MCP_EXECUTE_COMMAND_FRAME_ID:
    case STM32MCP_GET_BOARD_INFO_FRAME_ID:
    case STM32MCP_EXEC_RAMP_FRAME_ID:
    case STM32MCP_GET_REVUP_DATA_FRAME_ID:
    case STM32MCP_SET_REVUP_DATA_FRAME_ID:
    case STM32MCP_SET_CURRENT_REFERENCES_FRAME_ID:
        break;
    default:
        break;
    }
    free(rxPayload);
    free(txPayload);
}
/*********************************************************************
 * @fn      motorcontrol_exMsgCb
 *
 * @brief   When the motor controller sends the exception back, it reaches here
 *
 * @param   exceptionCode - The exception code described in STM32MCP
 *
 * @return  None.
 */
static void motorcontrol_exMsgCb(uint8_t exceptionCode)
{
    switch(exceptionCode)
        {
        case STM32MCP_QUEUE_OVERLOAD:
        case STM32MCP_EXCEED_MAXIMUM_RETRANSMISSION_ALLOWANCE:
            break;
        default:
            break;
        }
}
/*********************************************************************
 * @fn      motorcontrol_erMsgCb
 *
 * @brief   When the motor controller sends the error back, it reaches here
 *
 * @param   exceptionCode - The error code described in STM32MCP
 *
 * @return  None.
 */
static void motorcontrol_erMsgCb(uint8_t errorCode)
{
    switch(errorCode)
    {
    case STM32MCP_BAD_FRAME_ID:
    case STM32MCP_WRITE_ON_READ_ONLY:
    case STM32MCP_READ_NOT_ALLOWED:
    case STM32MCP_BAD_TARGET_DRIVE:
    case STM32MCP_OUT_OF_RANGE:
    case STM32MCP_BAD_COMMAND_ID:
    case STM32MCP_OVERRUN_ERROR:
    case STM32MCP_TIMEOUT_ERROR:
    case STM32MCP_BAD_CRC:
        break;
    default:
        break;
    }
}
/*********************************************************************
 * @fn      motorcontrol_brakeAndThrottleCB
 *
 * @brief   When the brake and throttle completed the adc conversion, it sends message here
 *
 * @param   throttleRatio - how much torque in percentage the escooter should reach
 *          errorMsg - The error Msg
 *
 * @return  None.
 */
static void motorcontrol_brakeAndThrottleCB(uint16_t throttleRatio, uint16_t errorMsg)
{
    if((errorMsg == BRAKE_AND_THROTTLE_NORMAL) && (throttleRatio >= 20))
    {
        uint16_t rpm = (uint16_t) (BRAKE_AND_THROTTLE_MAXIMUMN_SPEED*throttleRatio/100) & 0xFFFF;
        STM32MCP_executeRampFrame(STM32MCP_MOTOR_1_ID,rpm,200);
        STM32MCP_executeCommandFrame(STM32MCP_MOTOR_1_ID, STM32MCP_START_MOTOR_COMMAND_ID);
    }
    else
    {
        STM32MCP_executeCommandFrame(STM32MCP_MOTOR_1_ID, STM32MCP_STOP_MOTOR_COMMAND_ID);
    }
}
/*********************************************************************
 * @fn      motorcontrol_controllerCB
 *
 * @brief   When the motor controller sends the error back, it reaches here
 *
 * @param   exceptionCode - The error code described in STM32MCP
 *
 * @return  None.
 */
uint8_t resetValue;
static void motorcontrol_controllerCB(uint8_t paramID)
{
    switch(paramID)
    {
        case CONTROLLER_TRIP_RESET:
        {
            Controller_GetParameter(CONTROLLER_TRIP_RESET, &resetValue);
            break;
        }
        default:
            break;
    }
}
/*********************************************************************
 * @fn      motorcontrol_dashboardCB
 * @brief   When the client (The mobile app) writes the Dashboard GATT server
 *          It will post the paraID of the changed characteristic value
 *
 * @param   paramID: the paramID of the characteristics
 *
 * @return  TRUE or FALSE
 */
static void motorcontrol_dashboardCB(uint8_t paramID)       // !!!!!!!!!!!!!!!!!!!!!! what does this function do?
{
    uint8_t lightmode;      // = LIGHT_MODE_INITIAL;
    uint8_t lightstatus;    // = LIGHT_STATUS_INITIAL;
    switch(paramID)
    {
    case DASHBOARD_ERROR_CODE:

        break;
    case DASHBOARD_SPEED_MODE:

        break;
    case DASHBOARD_LIGHT_MODE:
        Dashboard_GetParameter(DASHBOARD_LIGHT_MODE, &lightmode);
        if(lightmode == LIGHT_MODE_OFF)
        {
            lightstatus = LIGHT_STATUS_OFF;
        }
        else if(lightmode == LIGHT_MODE_ON)
        {
            lightstatus = LIGHT_STATUS_ON;
        }
        else if(lightmode == LIGHT_MODE_AUTO)
        {
            lightstatus = getLightStatus();
        }
        ledControl_setLightMode(lightmode);
        ledControl_setLightStatus(lightstatus);
        break;
//    case DASHBOARD_BLE_STATUS:

//        break;
//    case DASHBOARD_BATTERY_STATUS:

//        break;
//    case DASHBOARD_UNITSELECT:

//        break;

//    case DASHBOARD_DASH_SPEED:

//        break;
    default:
        break;

    }
}
/*********************************************************************
 * @fn      motorcontrol_singleButtonCB
 * @brief   Set the gatt
 *
 * @param   serviceUUID - the service UUID
 *          charteristics - the charteristics (Not UUID, just the index)
 *          payload - just the payload
 *
 * @return  TRUE or FALSE
 */
uint8_t advertEnable = FALSE;
uint8_t messageid;  // for debugging only
static void motorcontrol_singleButtonCB(uint8_t messageID)
{
    messageid = messageID;  // for debugging only
    //uint8_t lightmode;
    switch(messageID)
    {
    case SINGLE_BUTTON_SINGLE_LONG_PRESS_MSG:       // case = 0x01
    {    // toggle Power ON/OFF or Enter/Exit Sleep Mode
        // ICallPlatform_pwrNotify(unsigned int eventType, uintptr_t eventArg, uintptr_t clientArg)

    }
        break;
    case SINGLE_BUTTON_SINGLE_SHORT_PRESS_MSG:      // case = 0x02
    {
        lightControl_change();
    }
        break;
    case SINGLE_BUTTON_SINGLE_SHORT_LONG_PRESS_MSG: // case = 0x03
    {
        //GAPRole_GetParameter(GAPROLE_CONNHANDLE, &connectionHandle);
        if (advertEnable == FALSE){
            advertEnable = TRUE;
            GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t), &advertEnable);
            //newState = GAPROLE_WAITING;
            //SimplePeripheral_processStateChangeEvt(newState);
        }
        else if (advertEnable == TRUE){
            advertEnable = FALSE;
            GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t), &advertEnable); // this causes the firmware to stop
        }
    }
        break;
    case SINGLE_BUTTON_DOUBLE_SHORT_PRESS_MSG:      // case = 0x04
    {   // toggle speed modes
        brakeAndThrottle_toggleSpeedMode();
    }
        break;
    case SINGLE_BUTTON_TREBLE_SHORT_PRESS_MSG:      // case = 0x05
    {   // toggle units
        dataAnalysis_changeUnitSelectDash();
    }
        break;
    default:                                        // case 0x00 and all other cases
        break;
    }
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
static void motorcontrol_setGatt(uint16_t serviceUUID, uint8_t charteristics, uint8_t payloadLength, uint8_t* payload)
{
    uint8_t *theMessage = ICall_malloc(sizeof(uint8_t)*(payloadLength + 4));
    if (theMessage)
    {
        theMessage[0] = (serviceUUID >> 0) & 0xFF;
        theMessage[1] = (serviceUUID >> 8) & 0xFF;
        theMessage[2] = charteristics;
        theMessage[3] = payloadLength;
        memcpy(theMessage + 4, payload, payloadLength);
        motorcontrol_bleCBs->enqueueMsg(SBP_MC_GATT_EVT, 0, theMessage);
    }
}
