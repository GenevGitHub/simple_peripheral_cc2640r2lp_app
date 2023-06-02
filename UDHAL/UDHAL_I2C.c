/*
 * @file   UDHAL_I2C.c
 * @brief This file contain the functions about
 *  Created on: 11 Mar 2023
 *      Author: User
 */
/*********************************************************************
 * INCLUDES
 */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
/* Driver Header files */
//#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>
#include "UDHAL/UDHAL_I2C.h"
#include "Application/lightControl.h"
#include "Board.h"
/*********************************************************************
 * LOCAL VARIABLES
 */
/*********************************************************************
 * LOCAL FUNCTIONS
 */
static I2C_Handle       i2cHandle;
static I2C_Params       i2cParams;
static I2C_Transaction  i2cTransaction = {0};

static uint8_t          i2cOpenStatus = 1;                   // i2cOpenStatus = 1 indicates i2c opened successfully. i2cOpenStatus = 0 indicates i2c did not open successfully
static uint8_t          i2cTransferStatus = 0;
static int8_t           i2cTransferError = 0;
/*********************************************************************
 * LOCAL FUNCTIONS
 */
//static uint8_t UDHAL_I2C_getI2CStatus();

/* ********************************************************************
 * @fn:     UDHAL_I2C_init
 *
 * @brief: It is used to initialize I2C
 *
 * Arguments:   None
 *
 * Return:      None
 ******************************************************************** */
void UDHAL_I2C_init()
{
    I2C_init();
}
/*********************************************************************
 * @fn      UDHAL_I2C_params_init
 *
 * @brief   It is used to initialize the I2C paramaters
 *
 * @param   None
 *
 * @return  None
 */
void UDHAL_I2C_params_init()
{
    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;
    UDHAL_I2C_open();
}

/*********************************************************************
 * @fn:     UDHAL_I2C_open
 *
 * @brief: open I2C communication
 *
 * Arguments:   None
 *
 * Return:      None
*********************************************************************/
void UDHAL_I2C_open(){

    i2cHandle = I2C_open(Board_I2C0, &i2cParams);
    if (!i2cHandle) {
        // I2C opened error
        i2cOpenStatus = 0;
        //  add error protocol here
        //  Action:  when i2c open fails, need to disable LED display and Light auto mode function
    }
    else {
        // I2C successfully opened
        i2cOpenStatus = 1;
    }
}
/*********************************************************************
 * @fn:     UDHAL_I2C_getI2CStatus
 *
 * @brief:  return I2C open status
 *
 * Arguments:   None
 *
 * Return:      None
*********************************************************************/
uint8_t UDHAL_I2C_getI2CStatus(){
    return (i2cOpenStatus);
    //return (0);
}
/*********************************************************************
 * @fn:     UDHAL_I2C_close
 *
 * @brief: close I2C communication
 *
 * Arguments:   None
 *
 * Return:      None
*********************************************************************/
void UDHAL_I2C_close(){
    I2C_close(i2cHandle);
}
/*********************************************************************
 * @fn:     UDHAL_I2C_transfer()
 *
 * @brief: I2C transfer
 *
 * Arguments:   Peripheral slave address,
 *              pointer address of write buffer
 *              write size
 *              pointer address of read buffer
 *              read size
 *
 * Return:      None
*********************************************************************/
uint8_t UDHAL_I2C_transfer(uint_least8_t slave_address, void *writeBuffer, size_t writeSize, void *readBuffer, size_t readSize)
{
    if (i2cOpenStatus == 1)
    {
//        /*
        i2cTransaction.slaveAddress = slave_address;
        i2cTransaction.writeBuf   = writeBuffer;
        i2cTransaction.writeCount = writeSize;
        i2cTransaction.readBuf    = readBuffer;
        i2cTransaction.readCount  = readSize;
//*/
        i2cTransferStatus = I2C_transfer(i2cHandle, &i2cTransaction);
        if (!i2cTransferStatus){
            i2cTransferError = 1;
            //error handle for i2c transfer error
        }

    }
    return i2cTransferError;

}
