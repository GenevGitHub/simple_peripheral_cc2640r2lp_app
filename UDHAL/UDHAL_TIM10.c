/*
 * UDHAL_TIM10.c
 *
 *  Created on: 30 Jan 2023
 *      Author: Chee
 */
/*****************************************************************

@file UDHAL_TIM10.c

@brief This library is used by powerOnTime.h to count the device power on time

******************************************************************
 * INCLUDES
 */
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <ti/sysbios/knl/Clock.h>
#include <xdc/runtime/Error.h>
#include "UDHAL/UDHAL_TIM10.h"
#include "Application/powerOnTime.h"
/******************************************************************
    Local Variables
*******************************************************************/
static Clock_Handle Clock01Handle;
static Clock_Params clkParams;
static uint32_t clockTicks01;
static Error_Block eb;
/******************************************************************
    Local Functions
*******************************************************************/
static void UDHAL_TIM10_start();
static void UDHAL_TIM10_stop();
static void UDHAL_TIM10_OVClockFxn01();
/******************************************************************
    Macros
*******************************************************************/
static powerOnTime_timerManager_t powerOnTime_timer =
{
    UDHAL_TIM10_start,
    UDHAL_TIM10_stop
};
/*********************************************************************
 *
 * @fn      UDHAL_TIM10_init
 *
 * @brief   To initialize the power on timer timer. Hierarchy: Called by UHDAL.c
 *
 * @param   None.
 *
 * @return  None.
 */
void UDHAL_TIM10_init(){
    Error_init(&eb);
    clockTicks01 = MILLISECONDS_IN_A_MINUTE * (1000 / Clock_tickPeriod) - 1; // minus 1 is required because one more tick is required to trigger overflow
    Clock01Handle = Clock_create(UDHAL_TIM10_OVClockFxn01, clockTicks01, &clkParams, &eb);
    powerOnTime_registerTimer(&powerOnTime_timer);
}
/*********************************************************************
 *
 * @fn      UDHAL_TIM10_params_init
 *
 * @brief   To initialize timer parameters. Hierarchy: Called by UHDAL.c
 *
 * @param   None.
 *
 * @return  None.
 */
void UDHAL_TIM10_params_init(){
    Clock_Params_init(&clkParams);
    clkParams.period = 0;
    clkParams.startFlag = FALSE;
    clkParams.arg = (UArg)0x0000;
}
/*********************************************************************
 * @fn      UDHAL_TIM10_start
 *
 * @brief   To start the timer for timeout.
 *
 * @param   None.
 *
 * @return  None.
 */
static void UDHAL_TIM10_start(){
    Clock_start(Clock01Handle);
}
/*********************************************************************
 * @fn      UDHAL_TIM10_stop
 *
 * @brief   To stop the timer for flow control timeout.
 *
 * @param   None.
 *
 * @return  None.
 */
static void UDHAL_TIM10_stop(){
    Clock_stop(Clock01Handle);
}

/*********************************************************************
 * @fn      UDHAL_TIM10_OVClockFxn
 *
 * @brief   After timeout, this function is called to count power on time
 *
 *
 * @param   none
 *
 * @return  none
 */
static void UDHAL_TIM10_OVClockFxn01(){
    powerOnTime_processTimerOV();
}


