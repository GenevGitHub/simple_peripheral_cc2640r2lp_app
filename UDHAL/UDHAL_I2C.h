/*
 * UDHAL_I2C.h
 *  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!IMPORTANT!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 *
 *
 *
 *
 *                                          Before you start using this library, you must read UM1052(Pg161-178).
 *
 *
 *
 *
 *  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!IMPORTANT!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 *  Created on:   20 Aug 2021 by Siu Yeung Yik
 *  Last Updated: 22 Oct 2021 by Siu Yeung Yik
 */

#ifndef UDHAL_I2C_UDHAL_I2C_H_
#define UDHAL_I2C_UDHAL_I2C_H_

#ifdef __cplusplus
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
/*********************************************************************
 * MACROS
 */
/*********************************************************************
 * FUNCTIONS
 */
extern void UDHAL_I2C_init();
extern void UDHAL_I2C_params_init();
extern void UDHAL_I2C_open();
extern void UDHAL_I2C_close();
extern uint8_t UDHAL_I2C_transfer(uint_least8_t slave_address, void *writeBuffer, size_t writeSize, void *readBuffer, size_t readSize);
extern uint8_t UDHAL_I2C_getI2CStatus();
/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* UDHAL_I2C_UDHAL_I2C_H_ */
