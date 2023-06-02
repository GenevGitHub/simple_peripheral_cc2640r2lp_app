/*
 * TSL2561.c
 *
 *  Created on: 29 May 2023
 *      Author: User
 */
#include <stdint.h>
#include <ti/drivers/I2C.h>
#include <TSL2561/TSL2561.h>
#include <UDHAL/UDHAL_I2C.h>

static uint_least8_t    TSL2561SlaveAddress = TSL2561_VDD_ADDR;
static uint8_t          iType = T_FN_CL_TYPE;
static uint8_t          timingReg = 0x01;
static uint8_t          iGain;
static uint8_t          tInt;
static uint16_t         calibrationFactorx1000;

//static I2C_Transaction  i2cTransaction1;

static uint8_t          TSL2561_writeBuffer[2];
static uint8_t          TSL2561_readBuffer[2];
static size_t           writeBufferSize;
static size_t           readBufferSize;
static uint8_t          i2cTransferError;
/*********************************************************************
 * Routine:     void l2dConverter_init()
 *
 * Description: Initialization
 *              set GAIN and time integration values
 *
 * Arguments:   None
 *
 * Return:      None
*********************************************************************/
static uint8_t (*ptr_TSL2561_writeBuffer)[2];
static uint8_t (*ptr_TSL2561_readBuffer)[2];

void l2dConverter_init()
{
    ptr_TSL2561_writeBuffer = &TSL2561_writeBuffer;
    ptr_TSL2561_readBuffer = &TSL2561_readBuffer;

    /*******************   Power On TSL2561: Command Register = 0x80 **********************
     *
     *   Control register
     *   Power On = 0x03
     *   Power Off = 0x00
     *
     *********************************************************************/
    // Power on TLS2561 sensor
    writeBufferSize = 2;
    readBufferSize = 0;
    TSL2561_writeBuffer[0] = TSL2561_COMMAND_REG;           // Command Register
    TSL2561_writeBuffer[1] = TSL2561_POWER_ON;           // Control register Power On
    TSL2561_readBuffer[0] = 0;
    TSL2561_readBuffer[1] = 0;

    i2cTransferError = UDHAL_I2C_transfer(TSL2561SlaveAddress, &TSL2561_writeBuffer, writeBufferSize, &TSL2561_readBuffer, readBufferSize);

    if (i2cTransferError == 0){
    /*******************   Set timing register: Command = 0x81 **********************
     *
     *   0x00 = Low gain & TINT = 13.7
     *   0x01 = Low gain & TINT = 101
     *   0x02 = Low gain & TINT = 402
     *   0x03 = Low gain & TINT = manual
     *   0x11 = High gain & TINT = 101
     *
     *********************************************************************/
    // Set Timing Registration on TLS2561 sensor
    writeBufferSize = 2;
    readBufferSize = 0;
    TSL2561_writeBuffer[0] = TSL2561_TIMING_REG;
    TSL2561_writeBuffer[1] = timingReg;
    TSL2561_readBuffer[0] = 0;
    TSL2561_readBuffer[1] = 0;
    i2cTransferError = UDHAL_I2C_transfer(TSL2561SlaveAddress, &TSL2561_writeBuffer, writeBufferSize, &TSL2561_readBuffer, readBufferSize);
    l2dConverter_initTimingReg();

    }

}
/*******************      Basic Operation       **********************
 * Read words (recommended)
 *   0xAC = raw Channel 0
 *   0xAE = raw Channel 1
  Read Byte
 *   0x0C = raw Channel 0 Low Data
 *   0x0D = raw Channel 0 High Data
 *   0x0E = raw Channel 1 Low Data
 *   0x0F = raw Channel 1 High Data
 *
 *********************************************************************/
    // Read Channel 0 via Read Word Protocol
static uint32_t raw_channel0;
static uint32_t raw_channel1;

uint32_t l2dConverter_readChannel(uint8_t readChannel)
{
    if (i2cTransferError == 0){
        writeBufferSize = 1;
        readBufferSize  = 2;
        TSL2561_writeBuffer[0] = readChannel;
        TSL2561_writeBuffer[1] = 0;
        TSL2561_readBuffer[0] = 0;
        TSL2561_readBuffer[1] = 0;
        i2cTransferError = UDHAL_I2C_transfer(TSL2561SlaveAddress, &TSL2561_writeBuffer, writeBufferSize, &TSL2561_readBuffer, readBufferSize);;
        // i2cTransferError = 0 means there was no I2C transfer error
        if (i2cTransferError == 0){
            if (readChannel == READCHANNEL0){
                raw_channel0 = 256 * TSL2561_readBuffer[1] + TSL2561_readBuffer[0];
                return (raw_channel0);
            }
            else if (readChannel == READCHANNEL1){
                raw_channel1 = 256 * TSL2561_readBuffer[1] + TSL2561_readBuffer[0];
                return (raw_channel1);
            }
        }
    }
    return (NULL);
}
/*********************************************************************
 * Routine:     uint32_t l2dConverter_lux()
 *
 * Description: Calculate the sample illuminance (lux)
 *
 * Arguments:   Nil
 *
 * Return:      unsigned integer - the average illuminance (lux)
*********************************************************************/
static uint32_t         luxSample = 0x11111111;         // default value = 286331153 (hexidecimal 0x11111111)

uint32_t l2dConverter_lux()
{
    if (i2cTransferError == 0){
        luxSample = (uint64_t) calibrationFactorx1000 * l2dConverter_calculateLux() / 1000;
    }
    return (luxSample);
}

/*********************************************************************************************
 * Routine:     uint32_t l2dConverter_calculateLux()
 *
 * Description: Calculate the approximate illuminance (lux) given the measured raw
 *              channel values of the TSL2560. The equation is implemented as a piece-wise
 *              linear approximation.
 *
 * Arguments:   Nil
 *
 * Return:      unsigned integer - the approximate illuminance (lux)
**********************************************************************************************/
uint64_t channel1;
uint64_t channel0;

uint32_t l2dConverter_calculateLux()
{
    // first, scale the channel values depending on the gain and integration time
    // 16X, 402mS is nominal.
    // scale if integration time is NOT 402 msec
    uint64_t chScale;

    // the channel scale is selected depending on the tInt selected
    switch(tInt)
    {
    case TINT0:
        chScale = CHSCALE_TINT0;
        break;
    case TINT1:
        chScale = CHSCALE_TINT1;
        break;
    default:
        chScale = (1 << CH_SCALE);  // 1 shift bit by 10
        break;
    }
    // scale if gain is NOT 16X
    if (!iGain){            //(if iGAIN == 0, scale = 16, else scale = 1)
            chScale = chScale << 4; // scale 1X to 16X
    }
    // shift bits are used throughout to take care/eliminate use of floating point variables
    channel0 = (raw_channel0 * chScale) >> CH_SCALE;
    channel1 = (raw_channel1 * chScale) >> CH_SCALE;

    uint64_t ratio1 = 0;
    // find the ratio of the channel values (Channel 1/Channel 0)
    // protect against divide by zero
    if (channel0 != 0) {ratio1 = (channel1 << (RATIO_SCALE+1)) / channel0;}

    uint64_t ratio;
    ratio = (ratio1 + 1) >> 1;
    uint32_t bb;
    uint32_t mm;
    switch(iType)
    {
    case T_FN_CL_TYPE:
    {
        if (ratio <= K1C){
            bb = B1T;
            mm = M1T;
        }
        else if (ratio <= K2T){
            bb = B2T;
            mm = M2T;
            }
        else if (ratio <= K3T){
            bb = B3T;
            mm = M3T;
        }
        else if (ratio <= K4T){
            bb = B4T;
            mm = M4T;
        }
        else if (ratio <= K5T){
            bb = B5T;
            mm = M5T;
            }
        else if (ratio <= K6T){
            bb = B6T;
            mm = M6T;
        }
        else if (ratio <= K7T){
            bb = B7T;
            mm = M7T;
        }
        else if (ratio > K8T){
            bb = B8T;
            mm = M8T;
        }
        break;
    }
    case CS_TYPE:
    {
        if (ratio <= K1C){
            bb = B1C;
            mm = M1C;
        }
        else if (ratio <= K2C){
            bb = B2C;
            mm = M2C;
            }
        else if (ratio <= K3C){
            bb = B3C;
            mm = M3C;
            }
        else if (ratio <= K4C){
            bb = B4C;
            mm = M4C;
            }
        else if (ratio <= K5C){
            bb = B5C;
            mm = M5C;
        }
        else if (ratio <= K6C){
            bb = B6C;
            mm = M6C;
        }
        else if (ratio <= K7C){
            bb = B7C;
            mm = M7C;
            }
        else if (ratio > K8C){
            bb = B8C;
            mm = M8C;
            }
        break;
    }
    default:
        break;
    }
    uint64_t temp;
    temp = ((channel0 * bb) - (channel1 * mm));

    temp += (1 << (LUX_SCALE - 1));
    uint32_t lux;
    lux = (temp >> LUX_SCALE);

    return(lux);
}

/*******************   Initialize light sensor internal timing register integration parameters **********************
     *
     *   0x00 = Low gain & TINT = 13.7
     *   0x01 = Low gain & TINT = 101
     *   0x02 = Low gain & TINT = 402
     *   0x03 = Low gain & TINT = manual
     *   0x11 = High gain & TINT = 101
     *
*********************************************************************************************************************/
void l2dConverter_initTimingReg()
{
    switch(timingReg)
    {
    case 0x00:
        {
            iGain = GAINSCALE01;            // we are only interested in detecting day-light -> scale fixed to 1
            tInt = INTEGRATIONTIME013;      // integration time = 13.7 ms
            calibrationFactorx1000 = 9721;  // calibration factor is based on in-house statistical measurements of TSL2561 vs LX-107 light meter
            break;
        }
    case 0x01:
        {
            iGain = GAINSCALE01;            // we are only interested in detecting day-light -> scale fixed to 1
            tInt = INTEGRATIONTIME101;      // integration time = 101ms
            calibrationFactorx1000 = 10370;  // calibration factor is based on in-house statistical measurements of TSL2561 vs LX-107 light meter
            break;
        }
    case 0x02:
        {
            iGain = GAINSCALE01;            // we are only interested in detecting day-light -> scale fixed to 1
            tInt = INTEGRATIONTIME402;      // integration time = 402ms
            calibrationFactorx1000 = 11181; // calibration factor is based on in-house statistical measurements of TSL2561 vs LX-107 light meter
            break;
        }
    case 0x03:
        {
            break;
        }
    case 0x11:
        {
            iGain = GAINSCALE16;            // This is high gain (16x) we are only interested in detecting day-light -> we should not select this option
            tInt = INTEGRATIONTIME101;      // integration time = 101ms
            calibrationFactorx1000 = 43;    // calibration factor is based on in-house statistical measurements of TSL2561 vs LX-107 light meter
            break;
        }
    default:
        {
            iGain = GAINSCALE01;            // we are only interested in detecting day-light -> scale fixed to 1
            tInt = INTEGRATIONTIME101;      // integration time = 101ms
            calibrationFactorx1000 = 10370;  // calibration factor is based on in-house statistical measurements of TSL2561 vs LX-107 light meter
            break;
        }
    }

}
