/*
 * periodicCommunication.h
 *
 *  Created on: 10 Feb 2022
 *      Author: harrysiu
 */
#ifndef PERIODICCOMMUNICATION_H
#define PERIODICCOMMUNICATION_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "simple_peripheral.h"
/*********************************************************************
*  EXTERNAL VARIABLES
*/
/*********************************************************************
 * CONSTANTS
 */
#define PERIODIC_COMMUNICATION_HF_SAMPLING_TIME      1000
#define PERIODIC_COMMUNICATION_LF_SAMPLING_TIME      6000
#define PERIODIC_COMMUNICATION_ACTIVATE              0x01
#define PERIODIC_COMMUNICATION_DEACTIVATE            0x00
/*********************************************************************
 * MACROS
 */
typedef void (*motorcontrol_timerStart)(void);
typedef void (*motorcontrol_timerSetPeriod)(uint32_t timerPeriod);
typedef void (*motorcontrol_timerStop)(void);
typedef struct
{
    motorcontrol_timerStart timerStart;
    motorcontrol_timerSetPeriod timerSetPeriod;
    motorcontrol_timerStop  timerStop;
}motorcontrol_timerManager_t;


/*********************************************************************
 * FUNCTIONS
 */
/*
 * Task creation function for the Simple Peripheral.
 */
extern void periodicCommunication_start();
extern void periodicCommunication_stop();
extern void periodicCommunication_toggle();
extern void periodicCommunication_register_hfTimer(motorcontrol_timerManager_t *obj);
extern void periodicCommunication_register_lfTimer(motorcontrol_timerManager_t *obj);
extern void periodicCommunication_registerBLE_Gatt(simplePeripheral_bleCBs_t *obj);
extern void periodicCommunication_hf_communication();
extern void periodicCommunication_lf_communication();
/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* PERIODICCOMMUNICATION_H */
