/*
 * ledControl.h
 *
 *  Created on: 30 Nov 2022
 *      Author: User
 */

#ifndef APPLICATION_LEDCONTROL_H_
#define APPLICATION_LEDCONTROL_H_
//
#ifdef __cplusplus
extern "C"
{
#endif
//
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>




//Local Functions declaration
extern void ledControl_Init();
extern void ledControl_setDashSpeed(float dashSpeed);
extern void ledControl_setBatteryStatus(uint8_t batteryStatus);
extern void ledControl_setSpeedMode(uint8_t speedMode);
extern void ledControl_setUnitSelectDash(uint8_t UnitSelectDash);
extern void ledControl_setBLEStatus(uint8_t BLEStatus);
extern void ledControl_setErrorCodeWarning(uint8_t errorCode);
extern void ledControl_setLightMode(uint8_t light_mode);
extern void ledControl_setLightStatus(uint8_t light_status);
//
#ifdef __cplusplus
}
#endif
//
#endif /* APPLICATION_LEDCONTROL_H_ */
