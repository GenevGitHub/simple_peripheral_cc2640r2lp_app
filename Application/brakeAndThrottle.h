/*
 * brake.h
 *
 *  Created on: 14 Feb 2022
 *      Author: harrysiu
 */

#ifndef APPLICATION_BRAKEANDTHROTTLE_H_
#define APPLICATION_BRAKEANDTHROTTLE_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include <stdint.h>
/*********************************************************************
*  EXTERNAL VARIABLES
*/
/*********************************************************************
 * CONSTANTS
 */
#define BRAKE_AND_THROTTLE_ADC_SAMPLING_PERIOD                    100
#define BRAKE_AND_THROTTLE_SAMPLES                                10
//Speed modes
#define BRAKE_AND_THROTTLE_SPEED_MODE_AMBLER                      0x00
#define BRAKE_AND_THROTTLE_SPEED_MODE_LEISURE                     0x01
#define BRAKE_AND_THROTTLE_SPEED_MODE_SPORTS                      0x02
#define BRAKE_AND_THROTTLE_MAXIMUMN_SPEED                         1500
//Speed mode reduction ratio
#define BRAKE_AND_THROTTLE_SPEED_MODE_AMBLER_REDUCTION_RATIO      60
#define BRAKE_AND_THROTTLE_SPEED_MODE_LEISURE_REDUCTION_RATIO     80
#define BRAKE_AND_THROTTLE_SPEED_MODE_SPORTS_REDUCTION_RATIO      100
//Hard braking definition
#define HARD_BRAKING_THROTTLE_PERCENTAGE                          5
#define HARD_BRAKING_BRAKE_PERCENTAGE                             5
//Throttle calibration values
#define THROTTLE_ADC_CALIBRATE_H                                  2400
#define THROTTLE_ADC_CALIBRATE_L                                  810
//Throttle error thresholds
#define THROTTLE_ADC_THRESHOLD_H                                  2600
#define THROTTLE_ADC_THRESHOLD_L                                  700
//Brake calibration values
#define BRAKE_ADC_CALIBRATE_H                                     2500
#define BRAKE_ADC_CALIBRATE_L                                     850
//Brake error thresholds
#define BRAKE_ADC_THRESHOLD_H                                     2700
#define BRAKE_ADC_THRESHOLD_L                                     700
//Error message
#define BRAKE_AND_THROTTLE_NORMAL                                 0x00
#define BRAKE_ERROR                                               0x01
#define THROTTLE_ERROR                                            0x02
#define HARD_BRAKING_ERROR                                        0x04
/*********************************************************************
 * MACROS
 */
typedef void (*brakeAndThrottle_timerStart)(void);
typedef void (*brakeAndThrottle_timerStop)(void);
typedef struct
{
    brakeAndThrottle_timerStart timerStart;
    brakeAndThrottle_timerStop  timerStop;
}brakeAndThrottle_timerManager_t;

typedef void (*brakeAndThrottle_ADC_Open)(void);
typedef void (*brakeAndThrottle_ADC_Convert)(uint16_t*);
typedef void (*brakeAndThrottle_ADC_Close)(void);
typedef struct
{
    brakeAndThrottle_ADC_Open       brakeAndThrottle_ADC_Open;
    brakeAndThrottle_ADC_Convert    brakeAndThrottle_ADC_Convert;
    brakeAndThrottle_ADC_Close      brakeAndThrottle_ADC_Close;
}brakeAndThrottle_adcManager_t;

typedef void (*brakeAndThrottle_CB_t)(uint16_t, uint16_t);
typedef struct
{
    brakeAndThrottle_CB_t       brakeAndThrottle_CB;
}brakeAndThrottle_CBs_t;
/*********************************************************************
 * FUNCTIONS
 */
/*
 * Task creation function for the Simple Peripheral.
 */
extern void brakeAndThrottle_init();
extern void brakeAndThrottle_start();
extern void brakeAndThrottle_stop();
extern void brakeAndThrottle_toggle();
extern void brakeAndThrottle_setSpeedMode(uint8_t speedMode);
extern uint8_t brakeAndThrottle_getSpeedMode();
extern void brakeAndThrottle_toggleSpeedMode();
extern void brakeAndThrottle_registerCBs(brakeAndThrottle_CBs_t *obj);
extern void brakeAndThrottle_registerTimer(brakeAndThrottle_timerManager_t *obj);
extern void brakeAndThrottle_registerADC1(brakeAndThrottle_adcManager_t *obj);
extern void brakeAndThrottle_registerADC2(brakeAndThrottle_adcManager_t *obj);
extern void brakeAndThrottle_ADC_conversion();

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* APPLICATION_BRAKEANDTHROTTLE_H_ */
