/**********************************************************************************************
 * Filename:       lightControl.h
 *
 * Description:    This file contains the Dashboard service definitions and
 *                 prototypes.
 *
 *************************************************************************************************/
//
#ifndef LIGHTCONTROL_H_
#define LIGHTCONTROL_H_
//
#ifdef __cplusplus
extern "C"
{
#endif
//
/*********************************************************************
 * INCLUDES
 */
#include<stdio.h>
#include<stdint.h>
/*********************************************************************
 * CONSTANTS
 */
/*********************************************************************
* CONSTANTS
*/
#define LUX_THRESHOLD                       350  // light shall be ON when light intensity is consistently below this lux value
#define ALS_NUMSAMPLES                      8    // The number of samples used for light intensity evaluation --> must be 8
#define ALS_SAMPLING_TIME                   250  // in milliseconds, is the time between ALS samples, i.e. sampling time
// lightControl timer is timer 8
// For example, total time interval between AUTO light ON/OFF is (ALS_NUMSPLES - 1) * ALS_SAMPLING_TIME => (8-1) * 250 = 1750 milliseconds
// For example, total time interval between AUTO light ON/OFF is (ALS_NUMSPLES - 1) * ALS_SAMPLING_TIME => (8-1) * 300 = 2100 milliseconds
#define LIGHT_MODE_OFF                      0x00
#define LIGHT_MODE_ON                       0x01
#define LIGHT_MODE_AUTO                     0x02
#define LIGHT_MODE_INITIAL                  LIGHT_MODE_AUTO

#define LIGHT_STATUS_OFF                    0x00
#define LIGHT_STATUS_ON                     0x01
#define LIGHT_STATUS_INITIAL                LIGHT_STATUS_OFF
//ALS stands for ambient light sensor
/*********************************************************************
 * TYPEDEFS
 */
/*********************************************************************
 * @Structure lightControl_CB_t
 *
 * @brief     It store the callback function in the application. Will be pointed if the library has updates
 */
typedef void (*lightControl_CB_t)(uint8_t status);

/*********************************************************************
 * @Structure lightControl_GPIOManager_t
 *
 * @brief     It defines a set of function pointer that the the library can access and control the device peripheral to manipulate the light
 *
 * @data      lightInit: Called when the application wants to init the light peripheral
 *            lightSetHigh: Called when the application wants to set the light peripheral to high
 *            lightSetLow: Called when the application wants to set the light peripheral to low
 *            lightClose: Called when the application wants to close the light peripheral
 */
typedef void (*lightInit)(void);
typedef void (*lightSetHigh)(void);
typedef void (*lightSetLow)(void);
typedef void (*lightClose)(void);
typedef struct
{
    lightInit lightInit;
    lightSetHigh lightSetHigh;
    lightSetLow lightSetLow;
    lightClose lightClose;
}lightControl_lightManager_t;

/*********************************************************************
* MACROS
*/
typedef void (*lightControl_ALStimerStart)(void);
typedef void (*lightControl_ALStimerStop)(void);
typedef struct
{
    lightControl_ALStimerStart  timerStart;
    lightControl_ALStimerStop  timerStop;
}lightControl_ALStimerManager_t;
/*********************************************************************
 * @Structure lightControl_ALSManager_t
 *
 * @brief     It defines a set of function pointer that the the library can access and control the device peripheral to manipulate the ALS
 *
 * @data      ALS_init: Called when the application wants to init the ALS
 *            ALS_read: Called when the application wants to read the ALS
 *            ALS_close: Called when the application wants to close the ALS
 */
typedef void (*ALS_init)(void);
typedef void (*ALS_read)(uint32_t *data);
typedef void (*ALS_close)(void);
typedef struct
{
    ALS_init ALS_init;
    ALS_read ALS_read;
    ALS_close ALS_close;
}lightControl_ALSManager_t;

/*********************************************************************
 * API FUNCTIONS
 */
extern void lightControl_init();
extern void lightControl_registerAppCB(lightControl_CB_t *object);  // what does this do?
//extern void lightControl_registerLightManager(lightControl_lightManager_t *object);   // what does this do?
extern void lightControl_registerTimer(lightControl_ALStimerManager_t *ALSTimer);
extern void lightControl_timerInterruptHandler();
static void light_MODE_AUTO(void);
static void light_MODE_OFF(void);
static void light_MODE_ON(void);
static void lightControl_ALS_Controller(uint32_t luxValue);
static void lightControl_changeLightMode();
extern void lightControl_change();
extern uint8_t getLightMode(void);
extern uint8_t getLightStatus(void);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* LIGHTCONTROL_H_ */
