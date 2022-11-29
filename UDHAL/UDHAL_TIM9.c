/*
 * UDHAL_TIM9.c
 *
 *  Created on: 14 Nov 2022
 *      Author: Chee
 */
/*****************************************************************

@file UDHAL_TIM9.c

@brief This library is used for buzzer.h to control buzzer timer at battery low state

******************************************************************
 * INCLUDES
 */
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <ti/sysbios/knl/Clock.h>
#include <xdc/runtime/Error.h>
#include "UDHAL/UDHAL_TIM9.h"
#include "Application/buzzerControl.h"
/******************************************************************
    Local Variables
*******************************************************************/
static Clock_Handle ClockHandle;
static Clock_Params clkParams;
static uint32_t clockTicks;
static Error_Block eb;
/******************************************************************
    Local Functions
*******************************************************************/
static void UDHAL_TIM9_start();
static void UDHAL_TIM9_stop();
static void UDHAL_TIM9_OVClockFxn();
/******************************************************************
    Macros
*******************************************************************/
static buzzerControl_timerManager_t buzzerControl_timer =
{
    UDHAL_TIM9_start,
    UDHAL_TIM9_stop
};
/*********************************************************************
 *
 * @fn      UDHAL_TIM9_init
 *
 * @brief   To initialize the buzzer timer. Hierarchy: Called by UHDAL.c
 *
 * @param   None.
 *
 * @return  None.
 */
void UDHAL_TIM9_init(){
    Error_init(&eb);
    //buzzer duration times: BATTERYLOW_TIME = 100ms Repetitive @BUZZER_TIMER_BATTERYLOW_PERIOD
    clockTicks = BUZZER_TIMER_BATTERYLOW_PERIOD * (1000 / Clock_tickPeriod) - 1; // minus 1 is required because one more tick is required to trigger overflow
    ClockHandle = Clock_create(UDHAL_TIM9_OVClockFxn, clockTicks, &clkParams, &eb);
    buzzerControl_registerTimer(&buzzerControl_timer);
}
/*********************************************************************
 *
 * @fn      UDHAL_TIM9_params_init
 *
 * @brief   To initialize timer parameters. Hierarchy: Called by UHDAL.c
 *
 * @param   None.
 *
 * @return  None.
 */
void UDHAL_TIM9_params_init(){
    Clock_Params_init(&clkParams);
    clkParams.period = BUZZER_TIMER_BATTERYLOW_PERIOD;   //period can be re-assigned
    clkParams.startFlag = FALSE;
    clkParams.arg = (UArg)0x0000;
}
/*********************************************************************
 * @fn      UDHAL_TIM9_start
 *
 * @brief   To start the timer for timeout.
 *
 * @param   None.
 *
 * @return  None.
 */
static void UDHAL_TIM9_start(){
    Clock_start(ClockHandle);  // Clock starts when battery percentage is below 5%.  Use function pointer timerManager -> timerStart();
}
/*********************************************************************
 * @fn      UDHAL_TIM9_stop
 *
 * @brief   To stop the timer for flow control timeout.
 *
 * @param   None.
 *
 * @return  None.
 */
static void UDHAL_TIM9_stop(){
    Clock_stop(ClockHandle);   // Clock stops when battery percentage is above 8%.  Use function pointer timerManager -> timerStart();
}

/*********************************************************************
 * @fn      UDHAL_TIM9_OVClockFxn
 *
 * @brief   After timeout, this function is called to command buzzer to make a short beep
 *
 *
 * @param   none
 *
 * @return  none
 */
static void UDHAL_TIM9_OVClockFxn(){
    buzzerControl_processTimerOV();
}




