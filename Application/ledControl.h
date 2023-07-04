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
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
/*********************************************************************
 * CONSTANTS
 */
/*********************************************************************
* CONSTANTS
*/
#define LEDDISPLAYSLAVEADDRESS                          0x29
#define LED_POWER_LIGHT_ON                              60
#define LED_POWER_LIGHT_OFF                             100
#define LEDCONTROL_TASK_STACK_SIZE                      1024
#define LEDCONTROL_TASK_PRIORITY                        5
#define LEDCONTROL_INIT_TIME                            1500
#define LEDCONTROL_REFRESH_TIME                         120

/*********************************************************************
 * @Structure ledControl_LedDisplayManager_t
 *
 * @brief     It defines a set of function pointer that the the library can access and control the device peripheral to manipulate the ALS
 *
 * @data      ledControl_open: Called when the application wants to open the ALS
 *            ledControl_close: Called when the application wants to close the ALS
 *            ledControl_transfer: Called when the application wants to transfer data to the ALS

 */

typedef void (*ledControl_open)(void);
typedef uint8_t (*ledControl_transfer)(uint_least8_t slave_address, void *writeBuffer, size_t writeSize, void *readBuffer, size_t readSize);
typedef void (*ledControl_close)(void);

typedef struct
{
    ledControl_open            ledControl_open;
    ledControl_transfer        ledControl_transfer;
    ledControl_close           ledControl;
}ledControl_ledDisplayManager_t;

/*********************************************************************
 * API FUNCTIONS
 */
extern void lightControl_registerLedDisplay( ledControl_ledDisplayManager_t *ledDisplayI2C );

/*********************************************************************
 *  Global Function declaration
 *
*/
extern void ledControl_init( void );

static void ledControl_taskFxn(UArg a0, UArg a1);

extern void ledControl_setAllOn( void );
extern void ledControl_setAllOff( void );
extern void ledControl_setDashSpeed( uint8_t dashSpeed );
extern void ledControl_setBatteryStatus( uint8_t batteryStatus );
extern void ledControl_setSpeedMode( uint8_t speedMode );
extern void ledControl_setUnitSelectDash( uint8_t UnitSelectDash );
extern void ledControl_setBLEStatus( uint8_t BLEStatus );
extern void ledControl_setErrorCodeWarning( uint8_t errorCode );
extern void ledControl_setLightMode( uint8_t light_mode );
extern void ledControl_setLightStatus( uint8_t light_status );
extern void ledControl_setLEDPower( uint8_t ledPower );
//
#ifdef __cplusplus
}
#endif
//
#endif /* APPLICATION_LEDCONTROL_H_ */
