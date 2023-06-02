/******************************************************************************

 @file  lightControl.c

 @brief This file contains the Code of light control interfacing

 Group: WCS, BTS
 Target Device: cc2640r2

 ******************************************************************************
 ******************************************************************************
 *****************************************************************************/
/*********************************************************************
 * INCLUDES
 */
#include "lightControl.h"
#include "ledControl.h"
#include <ti/drivers/GPIO.h>
#include "stdint.h"
#include "motorControl.h"
#include "Dashboard.h"
#include "lightSensorControl.h"
#include "Board.h"
#include <ti/drivers/GPIO.h>
#include "UDHAL/UDHAL_I2C.h"

//#include <ti/posix/ccs/unistd.h>
/*********************************************************************
 * LOCAL POINTERS
 */
static lightControl_CB_t *lightControl_CB;
static lightControl_ALStimerManager_t *lightControl_ALStimerManager;
static void (*lightModeArray[3])(void) = {light_MODE_OFF, light_MODE_ON, light_MODE_AUTO};
/*********************************************************************
 * LOCAL VARIABLES
 */
uint32_t luxValue = 0;                        // declare as static when not debugging
static uint8_t bitluxIndex = 0x00;
//static uint8_t lux_Index = 0x00;
//static uint8_t bitMask = 0x00;
uint8_t light_mode;        // declare as static when not debugging
static uint8_t light_mode_Index;
uint8_t light_status = LIGHT_STATUS_INITIAL;    // declare as static when not debugging
static uint8_t lightStatusNew = LIGHT_STATUS_INITIAL;
static uint8_t I2C_Status;

// for debugging/testing only
uint32_t ALS_Sample_Array[100] = {288, 346, 300, 280, 280, 288, 105, 180, 299, 0, 2000, 10, 2000, 10, 20, 10, 30, 2000, 10, 20,
                                 2000, 10, 2000, 10, 20, 10, 30, 12288, 28840, 67283, 85288, 45288, 907288, 2880, 51055, 6745, 699, 2000, 10, 20,
                                 0, 283, 281, 270, 283, 288, 288, 287, 287, 1287, 2000, 10, 2000, 10, 20, 10, 30, 2000, 10, 20,
                                 287, 1137, 87, 2887000, 28870, 288700, 28370, 5000, 9000, 6155, 2000, 10, 2000, 10, 20, 10, 30, 2000, 10, 20,
                                 788999, 2887000, 2000, 10, 2000, 10, 20, 10, 30, 2000, 10, 20, 2000, 10, 2000, 10, 20, 10, 30, 2000};
/*********************************************************************
 * LOCAL FUNCTIONS
 */
/*********************************************************************
 * @fn      getLightMode
 *
 * @brief   call this function to retrieve the current light mode
 *
 * @param   None
 *
 * @return  light_mode
 *********************************************************************/
uint8_t lightControl_getLightMode(){
    return light_mode;
}
/*********************************************************************
 * @fn      getLightStatus
 *
 * @brief   call this function to retrieve the current light status
 *
 * @param   None
 *
 * @return  light_status
 *********************************************************************/
uint8_t lightControl_getLightStatus(void){
    return light_status;
}
/*********************************************************************
 * @fn      light_MODE_AUTO
 *
 * @brief   Execute AUTO light mode routine
 *
 * @param   light_status
 *
 * @return  light_status
 *********************************************************************/
uint8_t ALSCount = 0;                               // for debugging only
void light_MODE_AUTO(){

    if (ALSCount >= 100){                            // for debugging only
            ALSCount = 0;                           // for debugging only
    }                                               // for debugging only
    luxValue = ALS_Sample_Array[ALSCount];        // for debugging only
    ALSCount++;                                     // for debugging only

    // call light sensor -> measure light level (Lux) data

//************* convert ALS_Data to luxIndex and compute sumluxIndex  ***************************
    lightControl_ALS_Controller(luxValue);
//
/*    if (bitluxIndex == 0x00) {
        lightStatusNew = LIGHT_STATUS_OFF;
    }
    else if (bitluxIndex == 0xFF) {
        lightStatusNew = LIGHT_STATUS_ON;
    }
*/
    if (bitluxIndex == 0) {
        lightStatusNew = LIGHT_STATUS_OFF;
    }
    else if (bitluxIndex == ALS_NUMSAMPLES) {
        lightStatusNew = LIGHT_STATUS_ON;
    }

    if (light_status != lightStatusNew) {
        light_status = lightStatusNew;
        lightControl_mclightStatusChg();
    }
    lightControl_ALStimerManager->timerStart();

    //return 0;
}
/*********************************************************************
 * @fn      light_MODE_OFF
 *
 * @brief   Execute OFF light mode routine
 *
 * @param   None
 *
 * @return  None
 *********************************************************************/
void light_MODE_OFF(){
    if (light_status != LIGHT_STATUS_OFF) {
        light_status = LIGHT_STATUS_OFF;
        lightControl_mclightStatusChg();
    }
}
/*********************************************************************
 * @fn      light_MODE_ON
 *
 * @brief   Execute ON light mode routine
 *
 * @param   None
 *
 * @return  None
 *********************************************************************/
void light_MODE_ON(){
    if (light_status != LIGHT_STATUS_ON) {
        light_status = LIGHT_STATUS_ON;
        lightControl_mclightStatusChg();
    }
}
/*********************************************************************
 * EXTERN FUNCTIONS
 */
/*********************************************************************
 * @fn      lightControl_mclightStatusChg
 *
 * @brief   Notify light status change
 *
 * @param   None
 *
 * @return  None
 *********************************************************************/
void lightControl_mclightStatusChg(void){
    // switch LED display brightness depending on light status
    uint8_t Board_GPIO_LED_state;
    uint8_t ledPower;
    switch(light_status)
    {
    case LIGHT_STATUS_ON:
            {
                Board_GPIO_LED_state = Board_GPIO_LED_ON;
                ledPower = LED_POWER_LIGHT_ON;
                break;
            }
    case LIGHT_STATUS_OFF:
            {
                Board_GPIO_LED_state = Board_GPIO_LED_OFF;
                ledPower = LED_POWER_LIGHT_OFF;
                break;
            }
    default:
        break;
    }
    GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_state);
    motorcontrol_setGatt(DASHBOARD_SERV_UUID, DASHBOARD_LIGHT_STATUS, DASHBOARD_LIGHT_STATUS_LEN, (uint8_t *) &light_status);
    ledControl_setLEDPower(ledPower);
    ledControl_setLightStatus(light_status);
}
/*********************************************************************
 * @fn      lightControl_initialization
 *
 * @brief   Initialization and initial set up at each Power On.  AUTO mode at every POWER ON
 *
 * @param   None
 *
 * @return  None
 *********************************************************************/
void lightControl_init(){

    //** I2C must be initiated before lightControl
    //** At every POWER ON (SYSTEM START-UP), "IF I2C communication is SUCCESSFUL", the light control is in auto mode (LIGHT_MODE_INITIAL), light is off (LIGHT_STATUS_INITIAL).
    //** "IF I2C communication is NOT successful, we have to disable auto mode.
    I2C_Status = UDHAL_I2C_getI2CStatus();

    if (I2C_Status == 1) {
        light_mode = LIGHT_MODE_AUTO;
        light_mode_Index = 2;
    }
    else if (I2C_Status == 0) {
        light_mode = LIGHT_MODE_OFF;
        light_mode_Index = 1;
    }

    //GPIO_setConfig(Board_GPIO_LIGHT, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);  // initiate light control

    light_status = LIGHT_STATUS_INITIAL;
    lightStatusNew = LIGHT_STATUS_INITIAL;

    if (light_mode == 2){
        lightControl_ALStimerManager->timerStart();
    }
}
/*********************************************************************
 * @fn      lightControl_Change
 *
 * @brief   Toggles and change to the next light mode, then call to execute light mode change
 *
 * @param   None
 *
 * @return  None
 *********************************************************************/
void lightControl_lightModeChange(){

    //lux_Index = 0x00;
    //bitMask = 0x00;
    light_mode++;

    if(light_mode > light_mode_Index)
    {
        light_mode = 0;
    }

    if(light_mode != 2)
    {
        lightControl_ALStimerManager->timerStop();
    }
    if(light_mode == 2)
    {
        lightControl_ALStimerManager->timerStart();
    }
    motorcontrol_setGatt(DASHBOARD_SERV_UUID, DASHBOARD_LIGHT_MODE, DASHBOARD_LIGHT_MODE_LEN, (uint8_t *) &light_mode);
    ledControl_setLightMode(light_mode);
    (*lightModeArray[light_mode])();
}
/*********************************************************************
 * @fn      lightControl_ALS_Controller
 *
 * @brief   Execute ON light mode routine for auto mode. Put it inside I2C interrupt
 *
 * @param   None
 *
 * @return  None
 *********************************************************************/
static uint8_t  luxIndex = 0;
static uint8_t  luxBit[ALS_NUMSAMPLES] = {0};

void lightControl_ALS_Controller(uint32_t luxValue)
{
    // ************* convert ALS_Data to luxIndex using bitwise approach  ***************************
    // This only works for multiples of 8 bits
    /*
    uint8_t shiftBit = lux_Index++ & 0x07;  // cycles from 0 to 7
    uint8_t para = ~(1 << shiftBit); // complement of (1 << shiftBit) = complement bits of the bit of interest
    bitMask = ((luxValue < LUX_THRESHOLD) << shiftBit) & 0xFF; // bitMask is current luxIndex at the shiftBit location
    bitluxIndex &= para;                    // resets the bit of interest to zero before update can be made
    bitluxIndex |= bitMask;                 // replaces the bit location with the current luxIndex
    */
    //
    uint8_t ii;
    bitluxIndex = 0;        // Resets bitluxIndex to 0

    // If the measured lux value is less than the LUX Threshold, luxBit = 1 (ON), else luxBit = 0 (OFF)
    if (luxValue < LUX_THRESHOLD){
        luxBit[luxIndex] = 1;
    }
    else {
        luxBit[luxIndex] = 0;
    }
    // Sum 3 consecutive luxBit values (= bitluxIndex)
    for (ii = 1; ii <= ALS_NUMSAMPLES; ii++){
        bitluxIndex += luxBit[ii];
    }

    luxIndex++;
    // reset LuxIndex to 0 if LuxIndex is greater than ALS_NUMSAMPLES
    if (luxIndex > ALS_NUMSAMPLES){
        luxIndex = 0;
    }

}
/*********************************************************************
 * @fn      lightControl_timerInterruptHandler
 *
 * @brief   It is used to initialize the callback functions such that the library can notify the application when there are updates
 *
 * @param   None
 *
 * @return  None
 */
void lightControl_timerInterruptHandler()
{
    light_MODE_AUTO(); //call light_MODE_AUTO function, the light intensity value will then be passed to lightControl_ALS_Controller(luxValue)
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
void lightControl_registerTimer(lightControl_ALStimerManager_t *ALSTimer)
{
    lightControl_ALStimerManager = ALSTimer;
}

/*********************************************************************
 * @fn      lightControl_registerAppCB
 *
 * @brief   Register a callback function in the main application to receive updates about the light status
 *
 * @param   object A function in the main application
 *
 * @return  None
 */
void lightControl_registerAppCB(lightControl_CB_t *object)
{
    lightControl_CB = object;
}
