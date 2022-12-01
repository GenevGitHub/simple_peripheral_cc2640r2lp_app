/******************************************************************************

 @file  lightControl.c

 @brief This file contains the Simple Peripheral sample application for use
        with the CC2650 Bluetooth Low Energy Protocol Stack.

 Group: WCS, BTS
 Target Device: cc2640r2

 ******************************************************************************
 ******************************************************************************
 *****************************************************************************/
/*********************************************************************
 * INCLUDES
 */
#include "lightControl.h"
#include "stdint.h"
/*********************************************************************
 * LOCAL POINTERS
 */
static lightControl_CB_t *lightControl_CB;
static lightControl_ALStimerManager_t *lightControl_ALStimerManager;
static void (*lightModeArray[3])(void) = {light_MODE_OFF, light_MODE_ON, light_MODE_AUTO};
/*********************************************************************
 * LOCAL VARIABLES
 */
uint32_t ALS_Sample = 0;                        // declare as static when not debugging
static uint8_t bitluxIndex = 0x00;
static uint8_t lux_Index = 0x00;
static uint8_t bitMask = 0x00;
uint8_t light_mode = LIGHT_MODE_INITIAL;        // declare as static when not debugging
uint8_t light_status = LIGHT_STATUS_INITIAL;    // declare as static when not debugging
static uint8_t lightStatusNew = LIGHT_STATUS_INITIAL;

// for debugging only
uint32_t ALS_Sample_Array[42] = {288, 346, 300, 280, 280, 288, 105, 180, 299, 0,
                                 12288, 28840, 67283, 85288, 45288, 907288, 2880, 51055, 6745, 699,
                                 0, 283, 281, 270, 283, 288, 288, 287, 287, 1287,
                                 287, 1137, 87, 2887000, 28870, 288700, 28370, 5000, 9000, 6155,
                                 788999, 2887000};
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
uint8_t getLightMode(){
    return light_mode;
}
/*********************************************************************
 * @fn      getLightStatuse
 *
 * @brief   call this function to retrieve the current light status
 *
 * @param   None
 *
 * @return  light_status
 *********************************************************************/
uint8_t getLightStatus(void){
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
    if (ALSCount >= 42){                            // for debugging only
            ALSCount = 0;                           // for debugging only
    }                                               // for debugging only
    ALS_Sample = ALS_Sample_Array[ALSCount];        // for debugging only
    ALSCount++;                                     // for debugging only
//************* convert ALS_Data to luxIndex and compute sumluxIndex  ***************************
    lightControl_ALS_Controller(ALS_Sample);
//
    if (bitluxIndex == 0x00) {
        lightStatusNew = LIGHT_STATUS_OFF;
    }
    else if (bitluxIndex == 0xFF) {
        lightStatusNew = LIGHT_STATUS_ON;
    }
    if (light_status != lightStatusNew) {//if the previous state is not the same as the command state
        light_status = lightStatusNew;  // Sends command to change light state
        // Action: "light status changed"
        //GPIO_write(Board_GPIO_LED, lightCommand);
        // Action: "command change and send new light mode and new light status to APP"
        // Action: LED display -> light on if light_status == ON. light off if light_status == OFF
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
    //Obviously, if light is already OFF, there is no need to send command to dashboard to change the state of light
    if (light_status != LIGHT_STATUS_OFF) { // only when the light state is different to the command.....
    // if light is previously ON, send command to turn OFF light.
        light_status = LIGHT_STATUS_OFF;//this line sends command to change light on/off
        // Action: "light status changed"
        //GPIO_write(Board_GPIO_LED, 0);
        //"command change and send new light mode and new light status to APP"
        // Action: LED display -> light off
    }
    //return 0;
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
    //Obviously, if light is already ON, there is no need to send command to dashboard to change the state of light
    if (light_status != LIGHT_STATUS_ON) {// only when the light state is different to the command.....
    // if light is previously OFF, send command to turn ON light.
        light_status = LIGHT_STATUS_ON;//this line sends command to change light on/off
        // Action: "light status changed"
        //GPIO_write(Board_GPIO_LED, 1);
        // Action: "command change and send new light mode and new light status to APP"
        // Action: LED display -> light on
    }
    //return 0;
}
/*********************************************************************
 * EXTERN FUNCTIONS
 */
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
    // At every POWER ON (SYSTEM START-UP), the light control is in auto mode (LIGHT_MODE_INITIAL), light is off (LIGHT_STATUS_INITIAL).
    //GPIO_setConfig(Board_GPIO_LIGHT, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);  // initiate light control
    light_mode = LIGHT_MODE_INITIAL;
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
void lightControl_change(){
    lux_Index = 0x00;
    bitMask = 0x00;
    light_mode++;   //Toggles to the next light modes
    if(light_mode > 2)
    {                 // the case of changing from AUTO mode to OFF mode
        light_mode = 0;
    }
    if(light_mode != 2)
    {                 // the case of changing from AUTO mode to OFF mode
        lightControl_ALStimerManager->timerStop();    // clock runs immediately at start-up --> stop timer 8
    }
    if(light_mode == 2)
    {               // whenever auto mode is re-selected --> restart timer 8
        lightControl_ALStimerManager->timerStart();
    }
    lightControl_changeLightMode(light_mode);
}
/*********************************************************************
 * @fn      lightCOntrol_changeLightMode
 *
 * @brief   executes light mode change
 *
 * @param   light_mode
 *
 * @return  None
 *********************************************************************/
void lightControl_changeLightMode(){
    (*lightModeArray[light_mode])();
    // Change light_mode on LED display.  If light_mode to AUTO -> turn on "AUTO" light.  Else -> "AUTO" light is off.
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
void lightControl_ALS_Controller(uint32_t luxValue)
{
    //************* convert ALS_Data to luxIndex using bitwise approach  ***************************
    uint8_t shiftBit = lux_Index++ & 0x07;  // cycles from 0 to 7
    uint8_t para = ~(1 << shiftBit); // complement of (1 << shiftBit) = complement bits of the bit of interest
    bitMask = ((luxValue < LUX_THRESHOLD) << shiftBit) & 0xFF; // bitMask is current luxIndex at the shiftBit location
    bitluxIndex &= para;                    // resets the bit of interest to zero before update can be made
    bitluxIndex |= bitMask;                 // replaces the bit location with the current luxIndex
}
/*********************************************************************
 * @fn      lightControl_timerInterrptHandler
 *
 * @brief   It is used to initialize the callback functions such that the library can notify the application when there are updates
 *
 * @param   None
 *
 * @return  None
 */
void lightControl_timerInterruptHandler()
{
    light_MODE_AUTO(); //call light_MODE_AUTO function, the light intensity value will then be passed to lightControl_ALS_Controller(ALS_Sample)
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
