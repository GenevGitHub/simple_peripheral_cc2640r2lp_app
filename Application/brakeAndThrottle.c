/******************************************************************************

 @file  brakeAndThrottle.c

 @brief This file contains the Code of brake and throttle

 *****************************************************************************/
/*********************************************************************
 * INCLUDES
 */
#include "brakeAndThrottle.h"
#include "Dashboard.h"
#include "motorControl.h"
#include "ledControl.h"
#include <stdint.h>
/*********************************************************************
 * CONSTANTS
 */
/*********************************************************************
 * GLOBAL VARIABLES
 */
uint8_t speedMode;
uint16_t adc1Result;
uint16_t adc2Result;
uint16_t throttlePercent;
uint16_t throttlePercent0;
uint16_t IQValue;
uint16_t brakePercent;
uint16_t brakeStatus = 0;
uint16_t brakeADCAvg = BRAKE_ADC_CALIBRATE_L;
uint16_t throttleADCAvg = THROTTLE_ADC_CALIBRATE_L;
uint8_t errorMsg = BRAKE_AND_THROTTLE_NORMAL;
uint8_t speedModeChgFlag = 0;    // SpeedModeChgFlag = 1 when speed mode has changed but instruction is not yet sent to Motor Controller

/*********************************************************************
 * LOCAL VARIABLES
 */
static brakeAndThrottle_timerManager_t *brake_timerManager;
static brakeAndThrottle_adcManager_t *brake_adc1Manager;
static brakeAndThrottle_adcManager_t *brake_adc2Manager;
static brakeAndThrottle_CBs_t *brakeAndThrottle_CBs;

static uint8_t state = 0;
static uint8_t  brakeIndex = 0;
static uint16_t brakeADCValues[BRAKE_AND_THROTTLE_SAMPLES] = {BRAKE_ADC_CALIBRATE_L};
static uint8_t  throttleIndex = 0;
static uint16_t throttleADCValues[BRAKE_AND_THROTTLE_SAMPLES] = {THROTTLE_ADC_CALIBRATE_L};


/**********************************************************************
 *  Local functions
 */
static void brakeAndThrottle_getSpeedModeParams();

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
    speedMode = BRAKE_AND_THROTTLE_SPEED_MODE_LEISURE; // load and Read NVSinternal and get the last speed mode
    brakeAndThrottle_getSpeedModeParams();
    brakeAndThrottle_start();

}
/*********************************************************************
 * @fn      brake_start
 *
 * @brief   Start the brake ADC and timer.  This occurs at Power On
 *
 * @param   none
 *
 * @return  none
 */
uint8_t testpoint = 0;
void brakeAndThrottle_start()
{
    testpoint = 1;
    brake_timerManager->timerStart();
    brake_adc1Manager->brakeAndThrottle_ADC_Open();
    brake_adc2Manager->brakeAndThrottle_ADC_Open();
}
/*********************************************************************
 * @fn      brake_stop
 *
 * @brief   Stop the brake ADC and timer.  This only occurs at Power Off
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
/***************************************************************************************************
 * @fn      brakeAndThrottle_StartAndStopToggle
 *
 * @brief   Initialization/start timer and stop timer for the Brake and Throttle periodic sampling.
 *
 * @param   none
 *
 * @return  none
 ***************************************************************************************************/
void brakeAndThrottle_toggle()
{
    if(state == 0)                                                  // State = 0 initially.  Must be called at Power On to start timer
    {
        brake_timerManager->timerStart();
        brake_adc1Manager->brakeAndThrottle_ADC_Open();             // this is repeated and duplicated in timerStart()
        brake_adc2Manager->brakeAndThrottle_ADC_Open();             // this is repeated and duplicated in timerStart()
        state = 1;
    }
    else if(state == 1)
    {
        brake_timerManager->timerStop();
        brake_adc1Manager->brakeAndThrottle_ADC_Close();             // this is repeated and duplicated in timerStart()
        brake_adc2Manager->brakeAndThrottle_ADC_Close();             // this is repeated and duplicated in timerStart()
        state = 0;
    }
}
/*********************************************************************
 * @fn      brakeAndThrottle_setSpeedMode
 *
 * @brief   To set the speed mode of the escooter
 *
 * @param   speedMode - the speed mode of the escooter
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
 * @fn      brakeAndThrottle_getThrottlePercent
 *
 * @brief   To get the throttle percentage of the escooter
 *
 * @param   none
 *
 * @return  the throttle percentage of the escooter
 */
uint16_t brakeAndThrottle_getThrottlePercent()
{
    return throttlePercent;
}
/*********************************************************************
 * @fn      brakeAndThrottle_getBrakePercent
 *
 * @brief   To get the brake percentage of the escooter
 *
 * @param   none
 *
 * @return  the brake percentage of the escooter
 */
uint16_t brakeAndThrottle_getBrakePercent()
{
    return brakePercent;
}


/*********************************************************************
 * @fn      brakeAndThrottle_getSpeedModeParams
 *
 * @brief   Get speed Mode parameters
 *
 * @param   speedMode
 *
 * @return  none
 */
static uint16_t torqueIQ;
static uint8_t reductionRatio;
static uint16_t rampRate;
static uint16_t allowableSpeed;
void brakeAndThrottle_getSpeedModeParams()
{
    switch(speedMode)
    {
    case BRAKE_AND_THROTTLE_SPEED_MODE_AMBLE:                       // Amble mode
        {
            speedMode = BRAKE_AND_THROTTLE_SPEED_MODE_AMBLE;
            reductionRatio = BRAKE_AND_THROTTLE_SPEED_MODE_REDUCTION_RATIO_AMBLE;
            //torqueIQ = BRAKE_AND_THROTTLE_TORQUEIQ_AMBLE;
            torqueIQ = reductionRatio * BRAKE_AND_THROTTLE_TORQUEIQ_MAX / 100;
            rampRate = BRAKE_AND_THROTTLE_RAMPRATE_AMBLE;
            allowableSpeed = BRAKE_AND_THROTTLE_MAXSPEED_AMBLE;
            break;
        }
    case BRAKE_AND_THROTTLE_SPEED_MODE_LEISURE:                 // Leisure mode
        {
            speedMode = BRAKE_AND_THROTTLE_SPEED_MODE_LEISURE;
            reductionRatio = BRAKE_AND_THROTTLE_SPEED_MODE_REDUCTION_RATIO_LEISURE;
            //torqueIQ = BRAKE_AND_THROTTLE_TORQUEIQ_LEISURE;
            torqueIQ = reductionRatio * BRAKE_AND_THROTTLE_TORQUEIQ_MAX / 100;
            rampRate = BRAKE_AND_THROTTLE_RAMPRATE_LEISURE;
            allowableSpeed = BRAKE_AND_THROTTLE_MAXSPEED_LEISURE;
            break;
        }
    case BRAKE_AND_THROTTLE_SPEED_MODE_SPORTS:                  // Sports mode
        {
            speedMode = BRAKE_AND_THROTTLE_SPEED_MODE_SPORTS;
            reductionRatio = BRAKE_AND_THROTTLE_SPEED_MODE_REDUCTION_RATIO_SPORTS;
            //torqueIQ = BRAKE_AND_THROTTLE_TORQUEIQ_SPORTS;
            torqueIQ = reductionRatio * BRAKE_AND_THROTTLE_TORQUEIQ_MAX / 100;
            rampRate = BRAKE_AND_THROTTLE_RAMPRATE_SPORTS;
            allowableSpeed = BRAKE_AND_THROTTLE_MAXSPEED_SPORTS;
            break;
        }
    default:
        break;
    }
}
/*********************************************************************
 * @fn      brakeAndThrottle_toggleSpeedMode
 *
 * @brief   To swap / toggle the speed Mode of the e-scooter
 *
 * @param   none
 *
 * @return  none
 */
void brakeAndThrottle_toggleSpeedMode()
{
    speedModeChgFlag = 1;

    if(speedMode == BRAKE_AND_THROTTLE_SPEED_MODE_AMBLE)                       // Amble mode to Leisure mode
    {
        speedMode = BRAKE_AND_THROTTLE_SPEED_MODE_LEISURE;
        reductionRatio = BRAKE_AND_THROTTLE_SPEED_MODE_REDUCTION_RATIO_LEISURE;
        //torqueIQ = BRAKE_AND_THROTTLE_TORQUEIQ_LEISURE;
        torqueIQ = reductionRatio * BRAKE_AND_THROTTLE_TORQUEIQ_MAX / 100;
        rampRate = BRAKE_AND_THROTTLE_RAMPRATE_LEISURE;
        allowableSpeed = BRAKE_AND_THROTTLE_MAXSPEED_LEISURE;
    }
    else if(speedMode == BRAKE_AND_THROTTLE_SPEED_MODE_LEISURE)                 // Leisure mode to Sports mode
    {
        speedMode = BRAKE_AND_THROTTLE_SPEED_MODE_SPORTS;
        reductionRatio = BRAKE_AND_THROTTLE_SPEED_MODE_REDUCTION_RATIO_SPORTS;
        //torqueIQ = BRAKE_AND_THROTTLE_TORQUEIQ_SPORTS;
        torqueIQ = reductionRatio * BRAKE_AND_THROTTLE_TORQUEIQ_MAX / 100;
        rampRate = BRAKE_AND_THROTTLE_RAMPRATE_SPORTS;
        allowableSpeed = BRAKE_AND_THROTTLE_MAXSPEED_SPORTS;
    }
    else if(speedMode == BRAKE_AND_THROTTLE_SPEED_MODE_SPORTS)                  // Sports mode back to Amble mode
    {
        speedMode = BRAKE_AND_THROTTLE_SPEED_MODE_AMBLE;
        reductionRatio = BRAKE_AND_THROTTLE_SPEED_MODE_REDUCTION_RATIO_AMBLE;
        //torqueIQ = BRAKE_AND_THROTTLE_TORQUEIQ_AMBLE;
        torqueIQ = reductionRatio * BRAKE_AND_THROTTLE_TORQUEIQ_MAX / 100;
        rampRate = BRAKE_AND_THROTTLE_RAMPRATE_AMBLE;
        allowableSpeed = BRAKE_AND_THROTTLE_MAXSPEED_AMBLE;
    }
    //Save the current setting
    ledControl_setSpeedMode(speedMode);  // update speed mode displayed on dash board
    motorcontrol_setGatt(DASHBOARD_SERV_UUID, DASHBOARD_SPEED_MODE, DASHBOARD_SPEED_MODE_LEN, (uint8_t *) &speedMode);  //update speed mode on client (App)

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
 *          This function is called when timer6 overflows
 *
 * @param
 */
void brakeAndThrottle_ADC_conversion()
{
    //uint16_t adc1Result;                           // result1 is a holder of the ADC reading
    brake_adc1Manager->brakeAndThrottle_ADC_Convert(&adc1Result);
    //adc1Result = (float) 375 * sin(3.14159265 * kk * BRAKE_AND_THROTTLE_ADC_SAMPLING_PERIOD / 1000 / 15) + 1170;    // dummy ADC brake data
    // stores the last "BRAKE_AND_THROTTLE_SAMPLES" data points
    brakeADCValues[brakeIndex++] = adc1Result;
    if(brakeIndex >= BRAKE_AND_THROTTLE_SAMPLES)
        brakeIndex = 0;

    //uint16_t adc2Result;                           // result1 is a holder of the ADC reading
    brake_adc2Manager->brakeAndThrottle_ADC_Convert(&adc2Result);
    //adc2Result = (float) 375 * sin(3.14159265 * kk * BRAKE_AND_THROTTLE_ADC_SAMPLING_PERIOD / 1000 / 10) + 1170;  // dummy ADC throttle data
    // stores the last "BRAKE_AND_THROTTLE_SAMPLES" data points
    throttleADCValues[throttleIndex++] = adc2Result;

    if(throttleIndex >= BRAKE_AND_THROTTLE_SAMPLES)
        throttleIndex = 0;

    // Sum the most recent "BRAKE_AND_THROTTLE_SAMPLES" number of data points.
    uint16_t brakeADCTotal = 0;
    uint16_t throttleADCTotal = 0;
    for(uint8_t index = 0; index < BRAKE_AND_THROTTLE_SAMPLES; index++)
    {
        brakeADCTotal += brakeADCValues[index];
        throttleADCTotal += throttleADCValues[index];
    }
    // the sampling interval is defined by "BRAKE_AND_THROTTLE_ADC_SAMPLING_PERIOD"
    // the number of samples is defined by "BRAKE_AND_THROTTLE_SAMPLES"
    // calculate moving average brake and throttle adc values
    //uint16_t
    brakeADCAvg = brakeADCTotal/BRAKE_AND_THROTTLE_SAMPLES;
    //uint16_t
    throttleADCAvg = throttleADCTotal/BRAKE_AND_THROTTLE_SAMPLES;

    // Check whether brake ADC reading is logical, if illogical, errorMsg = error
    // These Conditions could be met when throttle or brake signals are not connected
    if(brakeADCAvg > BRAKE_ADC_THRESHOLD_H)                // brakeADCAvg cannot be smaller than threshold_L nor greater threshold_H
    // These Conditions could be met when throttle or brake signals are not connected
    {
        errorMsg = BRAKE_ERROR;
    }
    if(brakeADCAvg < BRAKE_ADC_THRESHOLD_L)
    {
        //errorMsg = BRAKE_ERROR;
    }

    if(brakeADCAvg > BRAKE_ADC_CALIBRATE_H){
        brakeADCAvg = BRAKE_ADC_CALIBRATE_H;
    }
    if(brakeADCAvg < BRAKE_ADC_CALIBRATE_L){
        brakeADCAvg = BRAKE_ADC_CALIBRATE_L;
    }
    // Check whether throttle ADC reading is logical, if illogical, errorMsg = error
    // These Conditions could be met when throttle or brake signals are not connected, or supply voltage error
    if(throttleADCAvg > THROTTLE_ADC_THRESHOLD_H) {
       errorMsg = THROTTLE_ERROR;
    }
    if (throttleADCAvg < THROTTLE_ADC_THRESHOLD_L)    // throttleADCAvg cannot be smaller than threshold_L nor greater threshold_H.
    {
       //errorMsg = THROTTLE_ERROR;
    }
    if(throttleADCAvg > THROTTLE_ADC_CALIBRATE_H){
        throttleADCAvg = THROTTLE_ADC_CALIBRATE_H;
    }
    if(throttleADCAvg < THROTTLE_ADC_CALIBRATE_L){
        throttleADCAvg = THROTTLE_ADC_CALIBRATE_L;
    }
    // brakePercent is in percentage - has value between 0 - 100 %
    //uint16_t
    brakePercent = (uint16_t) ((brakeADCAvg - BRAKE_ADC_CALIBRATE_L) * 100 / (BRAKE_ADC_CALIBRATE_H - BRAKE_ADC_CALIBRATE_L));

    /********************** Brake Power Off Protect State Machine  *******************************************************************************
     *              if brake is pressed, i.e. brakePercent is greater than a certain value (15%), for safety purposes,
     *              dashboard will instruct motor controller to cut power to motor.
     *              Once power to motor is cut, both the brake & throttle must be released before power delivery can be resumed
    **********************************************************************************************************************************************/
    if ((brakeStatus == 1) && (throttlePercent >= throttlePercent0 * THROTTLEPERCENTREDUCTION)) {                           // condition when rider has not release the throttle
        if ((throttlePercent0 == 0) && (brakePercent <= BRAKEPERCENTTHRESHOLD)) {
            brakeStatus = 0;                                                                                                // if brake is not pulled
        }
        else {
            brakeStatus = 1;                                                                                                // if brake is pulled
        }
    }
    else if ((brakeStatus == 0) && (brakePercent > BRAKEPERCENTTHRESHOLD)) {                                                // condition when rider pulls on the brake
        brakeStatus = 1;
        throttlePercent0 = throttlePercent;
    }
    else if ((throttlePercent < throttlePercent0 * THROTTLEPERCENTREDUCTION) && (brakePercent <= BRAKEPERCENTTHRESHOLD)) {  // condition when rider releases the throttle && brake is released
        brakeStatus = 0;
    }
    // *********************************************************************************************************************************************
    // throttlePercent is in percentage - has value between 0 - 100 %
    //uint16_t
    throttlePercent = (uint16_t) ((throttleADCAvg - THROTTLE_ADC_CALIBRATE_L) * 100 / (THROTTLE_ADC_CALIBRATE_H - THROTTLE_ADC_CALIBRATE_L));

    if (errorMsg == 0) {
        if (brakeStatus == 1){
            IQValue = 0;
        }
        else {
            IQValue = BRAKE_AND_THROTTLE_TORQUEIQ_MAX * reductionRatio * throttlePercent / 10000;
        }
    }
    else {
        IQValue = 0;
    }

    //Send the throttle signal to STM32 Motor Controller
    brakeAndThrottle_CBs -> brakeAndThrottle_CB(allowableSpeed, throttlePercent, errorMsg);

    // The following is a safety critical routine/condition
    // Firmware only allows speed mode change when throttle is not pressed concurrently
    // If speed mode is changed && throttle is not pressed or is released, firmware will then send instructions & new Speed Mode parameters to STM32
    if ((speedModeChgFlag == 1) && (adc2Result <= THROTTLE_ADC_CALIBRATE_L)) {
        motorcontrol_speedModeChgCB(torqueIQ, allowableSpeed, rampRate);
        speedModeChgFlag = 0;
    }

    //Sends brake signal to the controller for tail light toggling
    //Do it as you like !!!

    //Send brake percentage for Regen Brake....
    //Do it as you like
}

