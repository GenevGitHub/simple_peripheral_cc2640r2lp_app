/*
 * ledControl.c
 *
 *  Created on: 30 Nov 2022
 *      Author: User
 */
/******************************************************************************
@file  ledControl.c
*
@brief This file contains the Code of led display control
*
*
*/
/*********************************************************************
* INCLUDES
*/
#include "ledControl.h"
/*********************************************************************
* LOCAL VARIABLES
*/

/*********************************************************************
*
* LOCAL FUNCTIONS
*/
/*********************************************************************
 * @fn      ledControl_Init
 *
 * @brief   It is used to initialize the library
 *
 * @param   none
 *
 * @return  none
 */
void ledControl_Init()
{
    // At Start-Up/initiation, lit all LED on Dashboard. Then, turn off ones that are not required to be on.
}
/*********************************************************************
 * @fn      ledControl_setAllOn
 *
 * @brief   call this function to lit all LED on display
 *
 * @param   Nil
 *
 * @return  Nil
 */
void ledControl_setAllOn(){
    // I2C to command lit all LED on Dashboard
}
/*********************************************************************
 * @fn      ledControl_setAllOff
 *
 * @brief   call this function to turn off all LED on display
 *
 * @param   Nil
 *
 * @return  Nil
 */
void ledControl_setAllOff(){
    // I2C to command turn off all LED on Dashboard
}
/*********************************************************************
 * @fn      ledControl_setDashSpeed
 *
 * @brief   call this function to set Speed indicator on LED display
 *
 * @param   dashSpeed
 *
 * @return  none
 */
void ledControl_setDashSpeed(float dashSpeed){
    // I2C to command set Speed Indicator
}
/*********************************************************************
 * @fn      ledControl_setBatteryStatus
 *
 * @brief   call this function to set battery bar on LED display
 *
 * @param   batteryStatus
 *
 * @return  none
 */
void ledControl_setBatteryStatus(uint8_t batteryStatus){
    // I2C to command set Battery Status
}
/*********************************************************************
 * @fn      ledControl_setSpeedMode
 *
 * @brief   call this function to set speed mode on LED display
 *
 * @param   speedMode
 *
 * @return  none
 */
void ledControl_setSpeedMode(uint8_t speedMode){
    // I2C to command set Speed Mode Status
}
/*********************************************************************
 * @fn      ledControl_setUnitSelectDash
 *
 * @brief   call this function to set Dashboard Display Unit on LED display
 *
 * @param   UnitSelectDash
 *
 * @return  none
 */
void ledControl_setUnitSelectDash(uint8_t UnitSelectDash){
    // I2C to command set Unit Light
}
/*********************************************************************
 * @fn      ledControl_setBLEStatus
 *
 * @brief   call this function to activate Warning Light on LED display
 *
 * @param   BLEStatus
 *
 * @return  none
 */
extern void ledControl_setBLEStatus(uint8_t BLEStatus){
    // I2C to command set BLE Light Status
}
/*********************************************************************
 * @fn      ledControl_setWarningMode
 *
 * @brief   call this function to activate Warning Light on LED display
 *
 * @param   WarningStatus
 *
 * @return  none
 */
void ledControl_setErrorCodeWarning(uint8_t errorCode){
    // I2C to command set Warning Light Status
}
/*********************************************************************
 * @fn      ledControl_setLightMode
 *
 * @brief   call this function to set light mode on LED display
 *
 * @param   light_mode
 *
 * @return  None
 *********************************************************************/
void ledControl_setLightMode(uint8_t light_mode){
    // I2C to command set Light Mode
}
/*********************************************************************
 * @fn      ledcontrol_setLightStatus
 *
 * @brief   call this function to set light status on LED display
 *
 * @param   light_status
 *
 * @return  None
 *********************************************************************/
void ledControl_setLightStatus(uint8_t light_status){
    // I2C to command set Light Status
}


