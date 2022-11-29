/**********************************************************************************************
 * Filename:       lightControl.h
 *
 * Description:    This file contains the TSL25403 register and functions
 *
 *
 *************************************************************************************************/


#ifndef TSL25403_H_
#define TSL25403_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "stdint.h"
/*********************************************************************
* CONSTANTS
*/
//Device address
#define TSL25403_DEVICE_ADDRESS             0x39

//Register address
#define TSL25403_REGISTER_ENABLE            0x80
#define TSL25403_REGISTER_ATIME             0x81
#define TSL25403_REGISTER_WTIM              0x83
#define TSL25403_REGISTER_AILTL             0x84
#define TSL25403_REGISTER_AILTH             0x85
#define TSL25403_REGISTER_AIHTL             0x86
#define TSL25403_REGISTER_AIHTH             0x87
#define TSL25403_REGISTER_PERS              0x8C
#define TSL25403_REGISTER_CFG0              0x8D
#define TSL25403_REGISTER_CFG1              0x90
#define TSL25403_REGISTER_REVID             0x91
#define TSL25403_REGISTER_ID                0x92
#define TSL25403_REGISTER_STATUS            0x93
#define TSL25403_REGISTER_VISDATAL          0x94
#define TSL25403_REGISTER_VISDATAH          0x95
#define TSL25403_REGISTER_IRDATAL           0x96
#define TSL25403_REGISTER_IRDATAH           0x97
#define TSL25403_REGISTER_REVID2            0x9E
#define TSL25403_REGISTER_CFG2              0x9F
#define TSL25403_REGISTER_CFG3              0xAB
#define TSL25403_REGISTER_AZ_CONFIG         0xD6
#define TSL25403_REGISTER_INTENAB           0xDD


/*********************************************************************
 * TYPEDEFS
 */
/*********************************************************************
 * @Structure TSL25403_I2C_data_t
 *
 * @brief     It defines a set of function pointer that the the library can access and control device I2C
 *
 * @data      address, the device address
 *            datasize, the size of the array
 *            data, the pointer to the data
 */
typedef struct
{
    uint8_t address;
    uint8_t reg;
    uint8_t datasize;
    uint8_t *data;
}TSL25403_I2C_data_t;
/*********************************************************************
 * @Structure TSL25403_I2C_t
 *
 * @brief     It defines a set of function pointer that the the library can access and control device I2C
 *
 * @data      TSL25403_I2C_Read: A pointer to the I2C read function
 *            TSL25403_I2C_Write: A point to the I2C write function
 */
typedef void (*TSL25403_I2C_Read)(TSL25403_I2C_data_t *I2C_Obj);
typedef void (*TSL25403_I2C_Write)(TSL25403_I2C_data_t *I2C_Obj);
typedef struct
{
    TSL25403_I2C_Read TSL25403_I2C_Read;
    TSL25403_I2C_Write TSL25403_I2C_Write;
}TSL25403_I2C_t;
/*********************************************************************
 * @Structure TSL25403_I2C_t
 *
 * @brief     It defines a set of function pointer that the the library can access and control device I2C
 *
 * @data      TSL25403_I2C_Read: A pointer to the I2C read function
 *            TSL25403_I2C_Write: A point to the I2C write function
 */
typedef void (*TSL25403_AppCB)(TSL25403_I2C_data_t *I2C_readData);
typedef struct
{
    TSL25403_AppCB TSL25403_AppCB;
}TSL25403_AppCB_t;
/*********************************************************************
 * API FUNCTIONS
 */
extern void TSL25403_init(TSL25403_I2C_t *I2C_Obj);
extern void TSL25403_registerI2C(TSL25403_I2C_t *I2C_Obj);
extern void TSL25403_registerAppCB(TSL25403_AppCB_t *appCB);
extern void TSL25403_I2C_Transfer(TSL25403_I2C_data_t *I2C_Data_Obj);
/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* TSL25403_H_ */
