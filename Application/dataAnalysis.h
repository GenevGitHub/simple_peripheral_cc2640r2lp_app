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
#include <time.h>
//Constants
#define DATA_ANALYSIS_POINTS            21              // Number of data points (time interval) used for numerical integration
#define MCU_SAMPLING_TIME               500             // unit in millisecond - must be the same as and linked to sampling time
#define SPEED_SAMPLING_TIME             250             // unit in millisecond - must be the same as and linked to sampling time
#define NUMUDINDEX                      20             // Number of Usage Data Set stored in flash memory
// Note: Data Analysis Time Interval = (DATA_ANALYSIS_POINTS - 1) x MCU_SAMPLING_TIME
// Option 1:  (25-1) x 250ms = 6000ms.  6000ms x 900 = 5400000ms = 1.5 hours -> 19 bytes per set x 900 = 17.1kB
// Option 2:  (21-1) x 250ms = 5000ms.  5000ms x 900 = 4500000ms = 1.25 hours -> 19 bytes per set x 900 = 17.1kB
// Option 3:  (41-1) x 250ms = 10000ms.  10000ms x 450 = 4500000ms = 1.25 hours -> 19 bytes per set x 450 = 8.55kB
#define WHEELRADIUS                     10.16           // wheel radius in centimeter
#define BCF                             0.9             // Battery Capacity Safety Factor
#define KM2MILE                         0.621           // convert length from km to miles
#define CM2INCH                         0.394           // convert length from cm to feet
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
#define BATTERY_STATUS_INITIAL          GLOWING_AQUA
//  Simulate MCU data
//static uint32_t jjMax = 500;                          // Set jjMax to the number of MCU dummy data to be sent to dashboard
#define jjMax                           5000

//typedef
// This set of data is stored in flash memory
typedef struct usageData{
        uint32_t dataCount;                             // to Cloud - require device parameters
        uint16_t dataTimeStamp;                         // to Cloud - require device parameters
        uint16_t avgBatteryVoltage;                     // to Cloud - require device parameters
        uint8_t avgSpeed;                               // to Cloud - require device parameters. average speed is optional
        uint32_t accumPowerConsumption;                 // to Cloud, App display input - require device parameters
        uint32_t accumMileage;                          // to Cloud, App display input - require device parameters
        uint16_t errorCode;                             // to Cloud, Both LED display and App Display
}UD;
// This set of data is temporary - this set of data is sent to the APP for displaying only and are erased when Device shuts down
typedef struct appData{
        uint16_t batteryPercentage;                     // App display input - require device parameters
        uint8_t batteryStatus;                          // Both LED display and App display input - require device parameters
        float instantEconomy;                           // Disable for now  // App display input - require retrieving saved data
        float economy;                                  // App display input - require retrieving saved data
        uint32_t range;                                 // App display input - require device parameters
        float co2Saved;                                 // App display input
}AD;

//Battery status related Function declaration
extern uint8_t computeBatteryPercentage();
extern uint8_t determineBatteryStatus();
extern uint8_t getBatteryPercentage();
extern uint8_t getUnitSelectDash();
extern void changeUnitSelectDash(void);

//Local Functions declaration
extern void dataAnalysis_Init();
extern uint8_t coefficient_array_init();
extern float LEDSpeed();
extern void dataSim(uint32_t jj);
extern void re_Initialize();
extern void get_AnalysisData();
extern void dataAnalysis_timerInterruptHandler();

//Performance related Function declaration
extern void dummyData();
extern uint32_t computePowerConsumption(); // output in mW-hr
extern uint32_t computeDistanceTravelled();// output in decimeter
extern uint8_t computeAverageSpeed();   // output in km/hr
extern uint32_t computeAvgBatteryVoltage(); // output in mV
extern float computeEconomy();  // unit in W-hr / km
extern uint32_t computeRange(); // output in metres
extern float computeCO2Saved(); // in kg
extern float computeInstantEconomy(); // unit in W-hr / km
//
#ifdef __cplusplus
}
#endif
//
#endif /* APPLICATION_DATAANALYSIS_H_ */
