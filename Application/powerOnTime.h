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
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
/*********************************************************************
 * CONSTANTS
 */
/*********************************************************************
* CONSTANTS
*/
#define MINUTE_IN_HOUR                                  60  // 60 minutes in an hour
#define POWERONTIME_TASK_STACK_SIZE                     512
#define POWERONTIME_TASK_PRIORITY                       1
#define POWERONTIME_MINUTE_TIME                         60000

/*********************************************************************
 * API FUNCTIONS
 */
extern void powerOnTime_init( void );
static void powerOnTime_taskFxn(UArg a0, UArg a1);
extern uint16_t powerOnTime_getPowerOnTime( void );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* APPLICATION_POWERONTIME_H_ */
