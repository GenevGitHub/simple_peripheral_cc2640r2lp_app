/*******************************************************************
 * UDHAL_NVSINT.h
 *
 * Description:     header file for NVS internal
 *
 *  Created on: 21 Mar 2023
 *      Author: Chee
 *
 *******************************************************************/

#ifndef UDHAL_UDHAL_NVSINT_H_
#define UDHAL_UDHAL_NVSINT_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
/* Driver Header files */

#include <ti/drivers/NVS.h>

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
/* Example/Board Header files */
#include "Board.h"
/*********************************************************************
*  EXTERNAL VARIABLES
*/

/*********************************************************************
 * CONSTANTS
 */
/*********************************************************************
 * MACROS
 */
/*********************************************************************
 * FUNCTIONS
 */

extern void UDHAL_NVSINT_init();
//extern void UDHAL_NVSINT_params_init(NVS_Params *ptrnvsParams);
extern void UDHAL_NVSINT_params_init();
//static void UDHAL_NVSINT_open(NVS_Params *ptrnvsParams);
extern void UDHAL_NVSINT_open();
extern void UDHAL_NVSINT_erase();
extern void UDHAL_NVSINT_write(size_t nvsOffset, void *ptrwriteBuffer, size_t writeBufferSize);
extern void UDHAL_NVSINT_read(size_t nvsOffset, void *ptrreadBuffer, size_t readBufferSize);
extern void UDHAL_NVSINT_close();
/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif


#endif /* UDHAL_UDHAL_NVSINT_H_ */
