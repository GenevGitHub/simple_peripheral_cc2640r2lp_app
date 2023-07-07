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
#include "powerOnTime.h"
//#include "lightControl.h"
#include "Dashboard.h"
#include "motorControl.h"

/*********************************************************************
 * LOCAL POINTERS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
uint16_t powerOnTimeMinute = 0;

/*********************************************************************
 * LOCAL FUNCTIONS
 */

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
* @fn      powerOnTime_taskFxn
*
* @brief   Task creation function for the powerOnTime.
*
* @param   None.
*
* @return  None.
**********************************************************************/
Char powerOnTime_TaskStack[POWERONTIME_TASK_STACK_SIZE];
Task_Struct powerOnTime_Task;

uint8_t powerOnTime_taskCount = 0;

static void powerOnTime_taskFxn(UArg a0, UArg a1)
{
    for (; ;)   // infinite for loop, starting at 1 and without exit condition,
    {
        // Task Sleep
        Task_sleep( POWERONTIME_MINUTE_TIME * 1000 / Clock_tickPeriod );
        powerOnTimeMinute++;
        motorcontrol_setGatt(DASHBOARD_SERV_UUID, DASHBOARD_POWER_ON_TIME, DASHBOARD_POWER_ON_TIME_LEN, (uint8_t *) &powerOnTimeMinute);
        //    minuteCounter = (uint16_t) powerOnTimeMinute % MINUTE_IN_HOUR;
        //    hourCounter = powerOnTimeMinute / MINUTE_IN_HOUR;
        powerOnTime_taskCount++;
    }
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
void powerOnTime_init()
{
    // At every POWER ON (SYSTEM START-UP), the power on time is reset to zero

    //powerOnTime_timerManager -> timerStart();

    // Construct Task for light sensor control
    Task_Params powerOnTime_taskParams;
    // Configure task
    Task_Params_init(&powerOnTime_taskParams);
//    powerOnTime_taskParams.stack = powerOnTime_TaskStack;
//    powerOnTime_taskParams.stackSize = POWERONTIME_TASK_STACK_SIZE;
//    powerOnTime_taskParams.priority = POWERONTIME_TASK_PRIORITY;

//    Task_construct(&powerOnTime_Task, powerOnTime_taskFxn, &powerOnTime_taskParams, NULL);

}
