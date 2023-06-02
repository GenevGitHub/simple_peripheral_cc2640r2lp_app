/*
 * dataAnalysis.h
 *
 *  Created on: 13 Oct 2022
 *      Author: Chee
 */
#ifndef APPLICATION_DATAANALYSIS_H_
#define APPLICATION_DATAANALYSIS_H_
//
#ifdef __cplusplus
extern "C"
{
#endif
//
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
//Constants
#define NVS_BUFFER_SIZE                 64
//#define DATA_ANALYSIS_POINTS            13              // Number of data points (time interval) used for numerical integration //  this shall equal DATA_ANALYSIS_POINTS
//#define DATA_ANALYSIS_SAMPLING_TIME     400             // unit in millisecond - must be the same as and linked to sampling time // Is this the same as PERIODIC_COMMUNICATION_HF_SAMPLING_TIME
#define UDARRAYSIZE                     20              // Number of Usage Data Set stored in flash memory
#define UDTRIGGER                       75
// Note: Data Analysis Time Interval = (DATA_ANALYSIS_POINTS - 1) x DATA_ANALYSIS_SAMPLING_TIME
// Option 1:  (21-1) x 400ms = 8000ms.  8000ms x 45 = 360000ms = 6 minutes. 6 minutes x 20 = 120 minutes = 2 hours
// Option 2:  (13-1) x 400ms = 4800ms.  4800ms x 75 = 360000ms = 6 minutes. 6 minutes x 20 = 120 minutes = 2 hours
#define WHEELRADIUS                     10.16           // wheel radius in centimeter
#define BCF                             0.9             // Battery Capacity Safety Factor
#define KM2MILE                         0.621           // convert length from km to miles
#define CM2INCH                         0.3937           // convert length from cm to inch
#define KG2LBS                          2.205           // convert kilogram to pounds
#define SI_UNIT                         0x00
#define IMP_UNIT                        0x01
#define COEFF01                         0.2156          // kg/km
#define COEFF02                         0.000386        // kg/W-hr
//Battery Status Constants
#define BATTERY_PERCENTAGE_INITIAL      100             // Remove this if calculated using start up voltage
#define BATTERY_PERCENTAGE_LL           5               // battery low percentage at which buzzer alert starts
#define BATTERY_PERCENTAGE_LH           8               // battery low percentage at which buzzer alert stops
#define BATTERY_MAX_VOLTAGE             42000           // This could also be defined elsewhere
#define BATTERY_MIN_VOLTAGE             30000           // the battery voltage assumed at 0% charged
#define BATTERY_CRITICALLY_LOW          26000
#define BATTERY_MAX_CAPACITY            355200          // mW-hr
#define LEVEL45                         39600           // equivalent to 80%
#define LEVEL34                         37560           // equivalent to 63%
#define LEVEL23                         35400           // equivalent to 45%
#define LEVEL12                         33360           // equivalent to 28%
#define LEVEL01                         30000           // equivalent to  0%
//Colours
#define GLOWING_AQUA                    0x05
#define GLOWING_GREEN                   0x04
#define YELLOW                          0x03
#define ORANGE                          0x02
#define RED                             0x01
#define FLASHING_RED                    0x00
#define BATTERY_STATUS_INITIAL          0x05
//  Simulate MCU data
#define VOLTAGEID                       0
#define CURRENTID                       1
#define HEATSINKTEMPERATUREID           2
#define MOTORRPMID                      3
#define MOTORTEMPERATUREID              4
#define MOTORCONTROLERRORID             5

#define CRIT_HEATSINKTEMPERATURE_C      100
#define CRIT_MOTORTEMPERATURE_C         125

//typedef
// This set of data is stored in ram, and to be stored in flash (NVS) memory
typedef struct usageData{
        uint32_t UDCounter;                             // to Cloud - require device parameters
        uint32_t totalPowerConsumption_mWh;             // to Cloud, App display input - require device parameters
        uint32_t totalMileage_dm;                       // to Cloud, App display input - require device parameters
}UD;

// This set of data is temporary on the dashboard - this set of data is sent to the APP for displaying when connected with BLE
typedef struct appData{                                 // is appData needed here??
        uint32_t ADCounter;                             // length = 4 . to Cloud - require device parameters
        uint32_t accumPowerConsumption_mWh;             // length = 4 . to Cloud, App display input - require device parameters
        uint32_t accumMileage_dm;                       // length = 4 . to Cloud, App display input - require device parameters
        uint32_t range_m;                               // length = 4 . App display input - require device parameters
        uint32_t co2Saved_g;                            // length = 4 . App display input
        uint16_t instantEconomy_100Whpk;                // length = 2 . Disable for now  // App display input - require retrieving saved data
        uint16_t economy_100Whpk;                       // length = 2 . App display input - require retrieving saved data
        uint16_t avgBatteryVoltage_mV;                  // length = 2 . to Cloud - require device parameters
        uint16_t errorCode;                             // length = 2 . to Cloud, Both LED display and App Display (May have to separate error code into their respective services)
        uint8_t avgSpeed_kph;                           // length = 1 . to Cloud - require device parameters
        int8_t avgHeatSinkTemperature_C;                // temperature can be sub-zero
        uint8_t batteryPercentage;                      // length = 1 (0-100%). App display input - require device parameters
        uint8_t batteryStatus;                          // length = 1 . Both LED display and App display input - require device parameters
        int8_t motorTemperature_C;                      // temperature can be sub-zero
}AD;        // 4-4-4-4-4-2-2-2-2-1-1-1-1-1 decreasing byte size minimizes the amount of struct padding

//Battery status related Function declaration
extern uint8_t computeBatteryPercentage( void );
extern uint8_t determineBatteryStatus( void );
extern uint8_t dataAnalysis_getBatteryPercentage( void );
extern uint8_t dataAnalysis_getUnitSelectDash( void );
extern void dataAnalysis_changeUnitSelectDash( uint8_t unit );
//extern void dataAnalysis_mcData(uint8_t paramID, uint32_t *pValue);

//Global Functions declaration
extern void dataAnalysis_init( void );
extern uint8_t coefficient_array_init( void );
extern void LEDSpeed(uint16_t xCounter);
extern void dataSim();
extern void dataAnalyt( void );
extern void data2UDArray( void );
extern void re_Initialize( void );
extern void get_UDArrayData( void );
extern void dataAnalysis_Main( void );
extern void get_ADArrayData(uint8_t paramID);
void dataAnalysis_motorcontrol_setGatt(void);
extern void dataAnalysis_timerInterruptHandler( void );

//Performance related Function declaration
extern void dummyUDArray( void );
extern uint32_t computePowerConsumption( void ); // output in mW-hr
extern uint32_t computeDistanceTravelled( void );// output in decimeter
extern uint8_t computeAvgSpeed(uint32_t deltaMileage_dm);   // output in km/hr
extern int8_t computeAvgHeatSinkTemperature( void );         // output in degrees celsius
extern uint32_t computeAvgBatteryVoltage( void ); // output in mV
extern uint16_t computeInstantEconomy(uint32_t deltaPowerConsumption_mWh, uint32_t deltaMileage_dm); // unit in W-hr / km x 100
extern uint16_t computeEconomy( void );  // unit in W-hr / km x 100
extern uint32_t computeRange( void ); // output in metres
extern uint32_t computeCO2Saved( void ); // in g
extern int8_t computeMotorTemperature( void ); // in degrees Celsius
//
#ifdef __cplusplus
}
#endif
//
#endif /* APPLICATION_DATAANALYSIS_H_ */
