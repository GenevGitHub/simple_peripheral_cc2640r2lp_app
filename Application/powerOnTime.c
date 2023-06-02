/******************************************************************************
 *
 * @file    powerOnTime.c
 *
 * @brief   This file contains the Code of counting the power on time of the device
 *
 *  Created on: 30 Jan 2023
 *      Author: Chee
 *
 *******************************************************************************/
/*********************************************************************
 * INCLUDES
 */
#include "stdint.h"
#include "powerOnTime.h"
#include "Dashboard.h"
#include "motorControl.h"

/*********************************************************************
 * LOCAL POINTERS
 */
static powerOnTimeCBs_t *powerOnTime_CBs;
static powerOnTime_timerManager_t *powerOnTime_timerManager;


/*********************************************************************
 * LOCAL VARIABLES
 */
static uint16_t powerOnTimeMinute;
//static uint8_t minuteCounter;
//static uint8_t hourCounter;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
//static void powerOnTime_setGatt(uint16_t serviceUUID, uint8_t charteristics, uint8_t payloadLength, uint8_t* payload);
/*********************************************************************
 * @fn      getPowerOnTime
 *
 * @brief   call this function to retrieve the Power On Time
 *
 * @param   None
 *
 * @return  powerOnTimeMinute
 *********************************************************************/
uint16_t powerOnTime_getPowerOnTime(){
    return powerOnTimeMinute;
}
/*********************************************************************
 * @fn      powerOnTime_init
 *
 * @brief   Initialization and initial set up at each Power On.
 *
 * @param   None
 *
 * @return  None
 *********************************************************************/
void powerOnTime_init(){
    // At every POWER ON (SYSTEM START-UP), the power on time is reset to zero
    powerOnTimeMinute = 0;
    //minuteCounter = 0;
    //hourCounter = 0;
    powerOnTime_timerManager -> timerStart();
}
/*********************************************************************
 * @fn      lightControl_registerTimer
 *
 * @brief   It is used to register the timer start and timer stop function to allow the library to control the peripherals
 *
 * @param   None
 *
 * @return  None
 */
void powerOnTime_registerTimer(powerOnTime_timerManager_t *Timer)
{
    powerOnTime_timerManager = Timer;
}

/*********************************************************************
 * @fn      powerOnTime_registerAppCB
 *
 * @brief   Register a callback function in the main application to receive updates about the power on time status
 *
 * @param   object A function in the main application
 *
 * @return  None
 */
void powerOnTime_registerAppCB(powerOnTimeCBs_t *object)
{
    powerOnTime_CBs = object;
}
/*********************************************************************
 * @fn      powerOnTime_Start
 *
 * @brief   Start timer 10
 *
 * @param   none
 *
 * @return  none
 */
void powerOnTime_Start(){
    powerOnTime_timerManager -> timerStart();
}
/*********************************************************************
 * @fn      powerOnTime_Stop
 *
 * @brief   Stop timer 10
 *
 * @param   none
 *
 * @return  none
 */
void powerOnTime_Stop(){
    powerOnTime_timerManager -> timerStop();
}
/*********************************************************************
 * @fn      powerOnTime_processTimerOV
 *
 * @brief   It is used to initialize the callback functions such that the library can notify the application when there are updates
 *
 * @param   None
 *
 * @return  None
 */
void powerOnTime_processTimerOV()
{
    powerOnTime_Start();
    powerOnTimeMinute++;
//    minuteCounter = (uint16_t) powerOnTimeMinute % MINUTE_IN_HOUR;
//    hourCounter = powerOnTimeMinute / MINUTE_IN_HOUR;
    motorcontrol_setGatt(DASHBOARD_SERV_UUID, DASHBOARD_POWER_ON_TIME, DASHBOARD_POWER_ON_TIME_LEN, (uint8_t *) &powerOnTimeMinute);
}
