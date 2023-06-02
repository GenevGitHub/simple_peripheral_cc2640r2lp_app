/**********************************************************************************************
 * powerOnTime.h
 *
 * Description:    This file contains the Dashboard service definitions and
 *                 prototypes.
 *
 *  Created on: 30 Jan 2023
 *      Author: Chee
 *
 **********************************************************************************************/

#ifndef APPLICATION_POWERONTIME_H_
#define APPLICATION_POWERONTIME_H_
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
#define MINUTE_IN_HOUR                       60  // 60 minutes in an hour


typedef void (*powerOnTime_timerStart)(void);
typedef void (*powerOnTime_timerStop)(void);
typedef struct{
    powerOnTime_timerStart timerStart;    // this is a function pointer
    powerOnTime_timerStop timerStop;      // this is a function pointer
}powerOnTime_timerManager_t;


typedef void (*powerOnTimeCB_t)(uint8_t messageID);
typedef struct{
    powerOnTimeCB_t powerOnTimeCB_t;    // this is a function pointer
}powerOnTimeCBs_t;

/*********************************************************************
 * API FUNCTIONS
 */
extern void powerOnTime_init( void );
extern void powerOnTime_registerAppCB(powerOnTimeCBs_t *object);  // what does this do?
extern void powerOnTime_registerTimer( powerOnTime_timerManager_t *timer );
extern void powerOnTime_processTimerOV( void );
extern void powerOnTime_Start( void );
extern void powerOnTime_Stop( void );
extern uint16_t powerOnTime_getPowerOnTime( void );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* APPLICATION_POWERONTIME_H_ */
