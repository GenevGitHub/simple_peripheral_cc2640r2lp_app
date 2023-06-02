/*
 * UDHAL_TIM10.h
 *
 * This library is used for powerOnTime.h to count the device power on time
 *  Created on:   06 Nov 2022 by Chee Tong
 *  Last Updated: 06 Nov 2022 by Chee Tong
 */

#ifndef UDHAL_UDHAL_TIM10_H_
#define UDHAL_UDHAL_TIM10_H_

#ifdef _cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
*  EXTERNAL VARIABLES
*/

/*********************************************************************
 * CONSTANTS
 */
//  Timer 10 repetitively starts at the following period (in milliseconds)
#define MILLISECONDS_IN_A_MINUTE                  60000
/*********************************************************************
 *
 * MACROS
 */
/*********************************************************************
 * FUNCTIONS
 */
extern void UDHAL_TIM10_init();
extern void UDHAL_TIM10_params_init();

/*********************************************************************
*********************************************************************/

#ifdef _cplusplus
}
#endif


#endif /* UDHAL_UDHAL_TIM10_H_ */
