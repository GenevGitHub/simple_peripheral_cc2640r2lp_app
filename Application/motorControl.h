#ifndef MOTORCONTROL_H
#define MOTORCONTROL_H

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
#define SBP_MC_GATT_EVT                        0x0020
#define SBP_MC_ADV_EVT                         0x0040

/*********************************************************************
 * MACROS
 */
/*********************************************************************
 * FUNCTIONS
 */
extern void motorcontrol_init(void);
extern void motorcontrol_registerCB(simplePeripheral_bleCBs_t *obj);
/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* SIMPLEPERIPHERAL_H */
