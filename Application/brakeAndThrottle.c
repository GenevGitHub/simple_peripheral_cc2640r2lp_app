/******************************************************************************

 @file  brakeAndThrottle.c

 @brief This file contains the Code of brake and throttle

 *****************************************************************************/
/*********************************************************************
 * INCLUDES
 */
#include "brakeAndThrottle.h"
#include <stdint.h>
/*********************************************************************
 * CONSTANTS
 */
/*********************************************************************
 * GLOBAL VARIABLES
 */
/*********************************************************************
 * LOCAL VARIABLES
 */
static brakeAndThrottle_timerManager_t *brake_timerManager;
static brakeAndThrottle_adcManager_t *brake_adc1Manager;
static brakeAndThrottle_adcManager_t *brake_adc2Manager;
static brakeAndThrottle_CBs_t *brakeAndThrottle_CBs;
static uint8_t state = 0;
static uint8_t  brakeIndex = 0;
static uint16_t brakeValues[BRAKE_AND_THROTTLE_SAMPLES] = {BRAKE_ADC_THRESHOLD_L};
static uint8_t  throttleIndex = 0;
static uint16_t throttleValues[BRAKE_AND_THROTTLE_SAMPLES] = {THROTTLE_ADC_THRESHOLD_L};
static uint8_t speedMode;
/**********************************************************************
 *  Local functions
 */
/*********************************************************************
 * @fn      brake_init
 *
 * @brief   Start the brake ADC and timer
 *
 * @param   none
 *
 * @return  none
 */
void brakeAndThrottle_init()
{
    speedMode = BRAKE_AND_THROTTLE_SPEED_MODE_SPORTS;
}
/*********************************************************************
 * @fn      brake_start
 *
 * @brief   Start the brake ADC and timer
 *
 * @param   none
 *
 * @return  none
 */
void brakeAndThrottle_start()
{
    brake_timerManager->timerStart();
    brake_adc1Manager->brakeAndThrottle_ADC_Open();
    brake_adc2Manager->brakeAndThrottle_ADC_Open();
}
/*********************************************************************
 * @fn      brake_stop
 *
 * @brief   Stop the brake ADC and timer
 *
 * @param   none
 *
 * @return  none
 */
void brakeAndThrottle_stop()
{
    brake_timerManager->timerStop();
    brake_adc1Manager->brakeAndThrottle_ADC_Close();
    brake_adc2Manager->brakeAndThrottle_ADC_Close();
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
void brakeAndThrottle_toggle()
{
    if(state == 0)
    {
        brake_timerManager->timerStart();
        brake_adc1Manager->brakeAndThrottle_ADC_Open();
        brake_adc2Manager->brakeAndThrottle_ADC_Open();
        state = 1;
    }
    else if(state == 1)
    {
        brake_timerManager->timerStop();
        brake_adc1Manager->brakeAndThrottle_ADC_Close();
        brake_adc2Manager->brakeAndThrottle_ADC_Close();
        state = 0;
    }
}
/*********************************************************************
 * @fn      brakeAndThrottle_setSpeedMode
 *
 * @brief   To set the speed mode of the escootter
 *
 * @param   speedMode - the speed mode of the escootter
 *
 * @return  none
 */
void brakeAndThrottle_setSpeedMode(uint8_t speedMode)
{
    speedMode = speedMode;
}
/*********************************************************************
 * @fn      brakeAndThrottle_getSpeedMode
 *
 * @brief   To get the speed mode of the escotter
 *
 * @param   none
 *
 * @return  the speedmode of the escootter
 */
uint8_t brakeAndThrottle_getSpeedMode()
{
    return speedMode;
}
/*********************************************************************
 * @fn      brakeAndThrottle_swapSpeedMode
 *
 * @brief   To swap the speed Mode of the escooter
 *
 * @param   none
 *
 * @return  none
 */
void brakeAndThrottle_swapSpeedMode()
{
    if(speedMode == BRAKE_AND_THROTTLE_SPEED_MODE_AMBLE)
    {
        speedMode = BRAKE_AND_THROTTLE_SPEED_MODE_SPORTS;
    }
    else if(speedMode == BRAKE_AND_THROTTLE_SPEED_MODE_SPORTS)
    {
        speedMode = BRAKE_AND_THROTTLE_SPEED_MODE_ECO;
    }
    else if(speedMode == BRAKE_AND_THROTTLE_SPEED_MODE_ECO)
    {
        speedMode = BRAKE_AND_THROTTLE_SPEED_MODE_AMBLE;
    }
    //Save the current setting
}
/*********************************************************************
 * @fn      brakeAndThrottle_registerCBs
 *
 * @brief   When the ADC completed the conversion, it makes a callback to the main function
 *
 * @param   none
 *
 * @return  none
 */
void brakeAndThrottle_registerCBs(brakeAndThrottle_CBs_t *obj)
{
    brakeAndThrottle_CBs = obj;
}
/*********************************************************************
 * @fn      brake_registerTimer
 *
 * @brief   Initialization timer for the brake ADC sampling.
 *
 * @param   none
 *
 * @return  none
 */
void brakeAndThrottle_registerTimer(brakeAndThrottle_timerManager_t *obj)
{
    brake_timerManager = obj;
}
/*********************************************************************
 * @fn      brake_registerADC1
 *
 * @brief   Initialization timer for the brake ADC sampling.
 *
 * @param   none
 *
 * @return  none
 */
void brakeAndThrottle_registerADC1(brakeAndThrottle_adcManager_t *obj)
{
    brake_adc1Manager = obj;
}
/*********************************************************************
 * @fn      brake_registerADC2
 *
 * @brief   Initialization timer for the brake ADC sampling.
 *
 * @param   none
 *
 * @return  none
 */
void brakeAndThrottle_registerADC2(brakeAndThrottle_adcManager_t *obj)
{
    brake_adc2Manager = obj;
}
/*********************************************************************
 * @fn      brake_registerADC2
 *
 * @brief   Initialization timer for the brake ADC sampling.
 *
 * @param   none
 *
 * @return  none
 */
void brakeAndThrottle_convertion(brakeAndThrottle_adcManager_t *obj)
{
    brake_adc2Manager = obj;
}
/*********************************************************************
 * @fn      brakeAndThrottle_ADC_conversion
 *
 * @brief   This function perform ADC conversion
 *
 * @param
 */

void brakeAndThrottle_ADC_conversion()
{
    uint16_t result1;
    brake_adc1Manager->brakeAndThrottle_ADC_Convert(&result1);
    brakeValues[brakeIndex++] = result1;
    if(brakeIndex >= BRAKE_AND_THROTTLE_SAMPLES)
        brakeIndex = 0;

    uint16_t result2;
    brake_adc2Manager->brakeAndThrottle_ADC_Convert(&result2);
    throttleValues[throttleIndex++] = result2;
    if(throttleIndex >= BRAKE_AND_THROTTLE_SAMPLES)
        throttleIndex = 0;


    uint16_t brakeTotal = 0;
    uint16_t throttleTotal = 0;
    for(uint8_t index = 0; index < BRAKE_AND_THROTTLE_SAMPLES; index++)
    {
        brakeTotal += brakeValues[index];
        throttleTotal += throttleValues[index];
    }


    uint16_t brakeAvg = brakeTotal/BRAKE_AND_THROTTLE_SAMPLES;
    uint16_t throttleAvg = throttleTotal/BRAKE_AND_THROTTLE_SAMPLES;

    uint8_t errorMsg = BRAKE_AND_THROTTLE_NORMAL;
    if((brakeAvg > BRAKE_ADC_THRESHOLD_H) || (brakeAvg < BRAKE_ADC_THRESHOLD_L))
    {
        errorMsg |= BRAKE_ERROR;
    }
    if((throttleAvg > THROTTLE_ADC_THRESHOLD_H) || (throttleAvg < THROTTLE_ADC_THRESHOLD_L))
    {
        errorMsg |= THROTTLE_ERROR;
    }
    uint8_t reductionRatio = 0;
    if(speedMode == BRAKE_AND_THROTTLE_SPEED_MODE_ECO)
    {
        reductionRatio = BRAKE_AND_THROTTLE_SPEED_MODE_ECO_REDUCTION_RATIO;
    }
    else if(speedMode == BRAKE_AND_THROTTLE_SPEED_MODE_AMBLE)
    {
        reductionRatio = BRAKE_AND_THROTTLE_SPEED_MODE_AMBLE_REDUCTION_RATIO;
    }
    else if(speedMode == BRAKE_AND_THROTTLE_SPEED_MODE_SPORTS)
    {
        reductionRatio = BRAKE_AND_THROTTLE_SPEED_MODE_SPORTS_REDUCTION_RATIO;
    }

    uint8_t throttleRatio = (uint8_t) ((throttleAvg - THROTTLE_ADC_CALIBRATE_L)*reductionRatio/(THROTTLE_ADC_CALIBRATE_H - THROTTLE_ADC_CALIBRATE_L)) & 0xFF;
    uint8_t brakeRatio = (uint8_t) ((brakeAvg - BRAKE_ADC_CALIBRATE_L)*100/(BRAKE_ADC_CALIBRATE_H - BRAKE_ADC_CALIBRATE_L)) & 0xFF;
    if((throttleRatio > HARD_BRAKING_THROTTLE_PERCENTAGE) && (brakeRatio > HARD_BRAKING_BRAKE_PERCENTAGE))
    {
        errorMsg |= HARD_BRAKING_ERROR;
    }

    brakeAndThrottle_CBs->brakeAndThrottle_CB(throttleRatio, errorMsg);
}
