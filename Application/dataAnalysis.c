/*
 * dataAnalysis.c
 *
 *  Created on: 13 Oct 2022
 *      Author: Chee
 */
/******************************************************************************
@file  dataAnalysis.c
@brief This file contains the functions for calculating:
    (1) distance travelled
    (2) power consumed
    (3) Average Speed
    (4) Average Battery Voltage
    (5) battery percentage
    (6) battery status
    (7) Instantaneous economy
    (8) Economy
    (9) Range
    (10) CO2 Saved
    The data are packaged and sent to mobile app, to LED display, to Cloud storage and saved in flash memory
*****************************************************************************/
/*********************************************************************
* INCLUDES
*/
#include "dataAnalysis.h"
#include "buzzerControl.h"
#include "ledControl.h"
#include "periodicCommunication.h"
/*********************************************************************
* LOCAL VARIABLES
*/
static uint16_t DATA_ANALYSIS_SAMPLING_TIME = PERIODIC_COMMUNICATION_HF_SAMPLING_TIME;
// Simpson's 1/3 rule coefficients
static uint8_t coefficient_array[DATA_ANALYSIS_POINTS] = {0};       // same size as DATA_ANALYSIS_POINTS
//Default battery status at each POWER ON
uint8_t batteryStatus =  BATTERY_STATUS_INITIAL;
//Default Unit Settings
uint8_t UnitSelectDash = SI_UNIT;  // Keep the last units selected by user in memory, the same units is used on restart
uint8_t UnitSelectApp = SI_UNIT;    // Mobile app allow user to select the desired display unit - the App units and Dash units are NOT linked
//
static uint16_t rpm[DATA_ANALYSIS_POINTS] = {0};                    //revolutions per minute data collected every time interval
static uint16_t speed[DATA_ANALYSIS_POINTS] = {0};                  //rpm is converted to cm per second (cm/s)
static uint16_t batteryCurrent[DATA_ANALYSIS_POINTS] = {0};         //battery current data collected every time interval
static uint16_t batteryVoltage[DATA_ANALYSIS_POINTS] = {0};         //battery voltage data collected every time interval
static uint8_t (*ptrc)[DATA_ANALYSIS_POINTS] = &coefficient_array;
static uint16_t (*ptrrpm)[DATA_ANALYSIS_POINTS] = &rpm;
static uint16_t (*ptrSp)[DATA_ANALYSIS_POINTS] = &speed;
static uint16_t (*ptrvb)[DATA_ANALYSIS_POINTS] = &batteryVoltage;
static uint16_t (*ptrcb)[DATA_ANALYSIS_POINTS] = &batteryCurrent;
//
static uint16_t xCount = 1;                     // At initialization, xCount begins at 0. After the 1st set, xCount begins at index 1.
static time_t t;
static uint32_t UDDataCounter = 0;          // At new, UDDataCounter = 0
static uint16_t UDIndex;                        // the last UDindex saved
static uint16_t UDIndexPrev;
static UD UDArray[UDARRAYSIZE] = {0};            // UDArray is the "storage array", stores the last 1.5 hours of usage data and is saved in flash memory.  UDArray are zero arrays at new.
static UD (*ptrUDArray)[UDARRAYSIZE] = &UDArray; // Provides a pointer option
static AD ADArray = {0};                        //Since this data set is temporary, array struct is not necesary.  ADArray data that are displayed on the mobile app.

static float lengthConvFactorDash;
//static uint8_t batteryPercentageInit;
static uint32_t avgBatteryVoltage = 0;
static uint8_t UDTriggerCounter = 0;
static uint32_t ADDataCounter = 0;
static uint32_t sumDeltaMileage;              // unit in decimeters.  This is the previous data on the total distance travelled
static uint32_t sumDeltaPowerConsumed;        // unit in milli-W-hr.  This is the previous data on the total power consumption
static uint32_t totalMileagePrev;              // unit in decimeters.  This is the previous data on the total distance travelled
static uint32_t totalPowerConsumedPrev;        // unit in milli-W-hr.  This is the previous data on the total power consumption
static uint32_t totalMileage0;                 // unit in decimeters.  This is the oldest data on total distance travelled stored in storage array
static uint32_t totalPowerConsumed0;           // unit in milli-W-hr.  This is the oldest data on total power consumed stored in storage array
/*********************************************************************
*
* LOCAL FUNCTIONS
*/
/*
 * @fn      coefficient_array_init
 *
 * @brief   Used to initialize the coefficient_array used in Simpson's 1/3 rule
 *
 * @param   Nil
 *
 * @return  Nil
*/
extern uint8_t coefficient_array_init(){ //change number of data points if necessary
    for(uint8_t x = 0; x < DATA_ANALYSIS_POINTS; x++){
        if(x==0 || x == DATA_ANALYSIS_POINTS-1){
            *((*ptrc)+ x) = 1;
        }
        else {
            if(x%2==1){
                *((*ptrc)+ x)=4;
            }
            else {
                *((*ptrc)+ x)=2;
            }
        }
    }
    return 0;
}
//******************Dummy data simulates data retrieve from flashing memory***********
// Description: Usage data at 6 minutes intervals over the last 2 hours [20 sets] are stored on flash memory
//              Economy calculations require past data on distance travelled and power consumption to be known, which
//              will require data to be retrieved from memory.
//              Below is a set of dummy data that simulates some data assigned and stored in memory
//************************************************************************************
void dummyUDArray(){
//************* dummy data for testing assigning data to array storage **************
//        UDDataCounter = 9;            // Foes with Test case 01 -  totalDataCount must be stored in memory - recall at every Power On - the last saved totalDataCount
        UDDataCounter = 39;           // Goes with Test Case 02 - totalDataCount must be stored in memory - recall at every Power On - the last saved totalDataCount
        UnitSelectDash = 0;
// Test case 00
//        (*ptrUDArray)[0].UDCounter = 0;                       // pointer option
//        (*ptrUDArray)[0].totalMileage = 0;                    // pointer option
//        (*ptrUDArray)[0].totalPowerConsumption = 0;           // pointer option
 /*
 // Test case 01
        UDArray[0].UDCounter = 0;
        UDArray[0].totalMileage = 0;
        UDArray[0].totalPowerConsumption = 0;
        UDArray[1].UDCounter = 1;
        UDArray[1].totalMileage =100;
        UDArray[1].totalPowerConsumption = 150;
        UDArray[2].UDCounter = 2;
        UDArray[2].totalMileage = 200;
        UDArray[2].totalPowerConsumption = 250;
        UDArray[3].UDCounter = 3;
        UDArray[3].totalMileage = 300;
        UDArray[3].totalPowerConsumption = 350;
        UDArray[4].UDCounter = 4;
        UDArray[4].totalMileage = 400;
        UDArray[4].totalPowerConsumption = 450;
        UDArray[5].UDCounter = 5;
        UDArray[5].totalMileage = 500;
        UDArray[5].totalPowerConsumption = 550;
        UDArray[6].UDCounter = 6;
        UDArray[6].totalMileage = 600;
        UDArray[6].totalPowerConsumption = 650;
        UDArray[7].UDCounter = 7;
        UDArray[7].totalMileage =700;
        UDArray[7].totalPowerConsumption = 750;
        UDArray[8].UDCounter = 8;
        UDArray[8].totalMileage = 800;
        UDArray[8].totalPowerConsumption = 850;
        (*ptrUDArray)[9].UDCounter = 9;                             // pointer option
        (*ptrUDArray)[9].totalMileage = 900;                    // pointer option
        (*ptrUDArray)[9].totalPowerConsumption = 950;          // pointer option
        */
 ///*
 // Test case 02
        UDArray[0].UDCounter = 20;
        UDArray[0].totalMileage = 21221;
        UDArray[0].totalPowerConsumption = 29589;
        UDArray[1].UDCounter = 21;
        UDArray[1].totalMileage = 23705;
        UDArray[1].totalPowerConsumption = 33246;
        UDArray[2].UDCounter = 22;
        UDArray[2].totalMileage = 25602;
        UDArray[2].totalPowerConsumption = 36127;
        UDArray[3].UDCounter = 23;
        UDArray[3].totalMileage = 27584;
        UDArray[3].totalPowerConsumption = 38703;
        UDArray[4].UDCounter = 24;
        UDArray[4].totalMileage =28530;
        UDArray[4].totalPowerConsumption = 40640;
        UDArray[5].UDCounter = 25;
        UDArray[5].totalMileage = 30549;
        UDArray[5].totalPowerConsumption = 42959;
        UDArray[6].UDCounter = 26;
        UDArray[6].totalMileage = 32503;
        UDArray[6].totalPowerConsumption = 46034;
        UDArray[7].UDCounter = 27;
        UDArray[7].totalMileage = 34485;
        UDArray[7].totalPowerConsumption = 48962;
        UDArray[8].UDCounter = 28;
        UDArray[8].totalMileage = 36366;
        UDArray[8].totalPowerConsumption = 51871;
        (*ptrUDArray)[9].UDCounter = 29;                             // pointer option
        (*ptrUDArray)[9].totalMileage = 38138;                    // pointer option
        (*ptrUDArray)[9].totalPowerConsumption = 54542;          // pointer option
        UDArray[10].UDCounter = 30;
        UDArray[10].totalMileage = 40060;
        UDArray[10].totalPowerConsumption = 57159;
        UDArray[11].UDCounter = 31;
        UDArray[11].totalMileage = 42033;
        UDArray[11].totalPowerConsumption = 59610;
        UDArray[12].UDCounter = 32;
        UDArray[12].totalMileage = 43805;
        UDArray[12].totalPowerConsumption = 61845;
        UDArray[13].UDCounter = 33;
        UDArray[13].totalMileage = 45754;
        UDArray[13].totalPowerConsumption = 63879;
        UDArray[14].UDCounter = 34;
        UDArray[14].totalMileage = 47706;
        UDArray[14].totalPowerConsumption = 67064;
        UDArray[15].UDCounter = 35;
        UDArray[15].totalMileage = 49701;
        UDArray[15].totalPowerConsumption = 70200;
        UDArray[16].UDCounter = 36;
        UDArray[16].totalMileage = 51678;
        UDArray[16].totalPowerConsumption = 72962;
        UDArray[17].UDCounter = 37;
        UDArray[17].totalMileage = 57329;
        UDArray[17].totalPowerConsumption = 75800;
        UDArray[18].UDCounter = 38;
        UDArray[18].totalMileage = 55735;
        UDArray[18].totalPowerConsumption = 78439;
        (*ptrUDArray)[19].UDCounter = 39;                          // pointer option
        (*ptrUDArray)[19].totalMileage = 57592;                    // pointer option
        (*ptrUDArray)[19].totalPowerConsumption = 80931;           // pointer option
 //*/
}
/*********************************************************************
 * @fn      dataAnalysis_timerInterrptHandler
 *
 * @brief   It is used to initialize the callback functions such that the library can notify the application when there are updates
 *
 * @param   None
 *
 * @return  None
 */
extern void dataAnalysis_timerInterruptHandler()
{
        // get data from MCU at every DATA_ANALYSIS_SAMPLING_TIME interval
}
/*
 * @fun      dataAnalysis_Init
 *
 * @brief   Initialization and determines the starting conditions from saved data
 *
 * @param   Nil
 *
 * @return  Nil
*/
extern void dataAnalysis_Init(){
    uint16_t batteryVoltageStartUp;  //= get Battery Voltage ;
    uint16_t batteryCurrentStartUp;
    // At the instant of POWER ON, we need to obtain BATTERY status for LED display
    // dashboard will demand MCU to obtain one battery voltage and current measurement immediately at STARTUP
    batteryVoltageStartUp = 40000;  //or = get Battery Voltage in mV;
    batteryCurrentStartUp = 3000;     //or = get Battery Current in mA;
    //
    avgBatteryVoltage = batteryVoltageStartUp;
    ADArray.batteryPercentage = round((float)(batteryVoltageStartUp - BATTERY_MIN_VOLTAGE))* 100 /(BATTERY_MAX_VOLTAGE - BATTERY_MIN_VOLTAGE);
    ADArray.batteryStatus = determineBatteryStatus();
    dummyUDArray();
    get_UDArrayData();
    if (UnitSelectDash == IMP_UNIT) {lengthConvFactorDash = KM2MILE;}
    else if (UnitSelectDash == SI_UNIT) {lengthConvFactorDash = 1;}
    srand((unsigned) time(&t)); // delete this later!!!!
    *((*ptrrpm)) = 0;         // unit in rpm = get rpm
    *((*ptrSp)) = round((*((*ptrrpm))) * 2 * (float) M_PI / 60 * WHEELRADIUS);          // Unit in cm / sec
    *((*ptrcb)) = batteryCurrentStartUp; //rand()%13 * 1000;                 // unit in mA = get battery current in mA
    *((*ptrvb)) = batteryVoltageStartUp; //*((*ptrvb)) - xCount * 10;         // unit in mV = get battery voltage in mV
    dataAnalyt();
    //ledControl_setBatteryStatus(ADArray.batteryStatus);     // Send battery status to LED display
    ADDataCounter = 0;
    ledControl_setUnitSelectDash(UnitSelectDash);       // Send Unit Select to LED display
}
/*
 * @fn      get_UDArrayData
 *
 * @brief   conditions and get data ready for data analysis at the STARTUP and NON-STARTUP stuations
 *
 * @param   Nil
 *
 * @return  Nil
*/
extern void get_UDArrayData(){
    UDIndexPrev = UDDataCounter % UDARRAYSIZE;
    if (UDIndexPrev == (UDARRAYSIZE - 1)) {
            UDIndex = 0;
            }
    else {
            UDIndex = UDIndexPrev + 1;
            }
    totalMileage0 = UDArray[UDIndex].totalMileage;
    totalPowerConsumed0 = UDArray[UDIndex].totalPowerConsumption;
    totalMileagePrev = UDArray[UDIndexPrev].totalMileage;
    totalPowerConsumedPrev = UDArray[UDIndexPrev].totalPowerConsumption;
}
/*
 * @fn      LEDSpeed
 *
 * @brief   calculate speed in Km/hr and display on dash board
 *
 * @param   xCount
 *
 * @return  Nil
*/
extern void LEDSpeed(uint16_t xCounter){
    float dashSpeed = ((float) speed[xCounter] * 0.036 * lengthConvFactorDash);
    ledControl_setDashSpeed(dashSpeed); // Send dashSpeed to LED display and App immediately
}
/*
 * @fn      dataAnalyt
 *
 * @brief   Data Evaluation function:  Call by dataSim() and when Power-OFF
 *
 * @param   Nil
 *
 * @return  Nil
*/
void dataAnalyt(){
    uint32_t deltaPowerConsumption, deltaMileage;
    ADArray.ADCounter = ADDataCounter;            // totalDataCount is total count of all computed datasets
    deltaPowerConsumption = computePowerConsumption();
    deltaMileage = computeDistanceTravelled();
    sumDeltaPowerConsumed += deltaPowerConsumption;
    sumDeltaMileage += deltaMileage;
    ADArray.accumPowerConsumption = totalPowerConsumedPrev + sumDeltaPowerConsumed;
    ADArray.accumMileage = totalMileagePrev + sumDeltaMileage;
    ADArray.avgSpeed = computeAverageSpeed(deltaMileage);
    ADArray.avgBatteryVoltage = computeAvgBatteryVoltage();
    ADArray.errorCode = 7;
    ADArray.batteryStatus = determineBatteryStatus();
    ADArray.batteryPercentage = computeBatteryPercentage();
    ADArray.instantEconomy = computeInstantEconomy(deltaPowerConsumption, deltaMileage);
    ADArray.economy = computeEconomy();
    ADArray.range = computeRange();
    ADArray.co2Saved = computeCO2Saved();
    re_Initialize();    // Re-initialize arrays after data analysis
    ledControl_setBatteryStatus(batteryStatus);     // Send battery status and errorCode to led display
    ledControl_setErrorCodeWarning(ADArray.errorCode);
    // Action Required: Send ADArray to Mobile App.  ADArray cold be stored in Mobile memory, then Mobile could send ADArray to cloud when WIFI is available.
    UDTriggerCounter++;
    ADDataCounter++;
}
/*
 * @fn      data2UDArray
 *
 * @brief   Save data to UDArray:  Called by dataAnalyt() and when Power-OFF
 *
 * @param   Nil
 *
 * @return  Nil
*/
void data2UDArray(){
    // Stored data to UDArray
    UDDataCounter++;
    UDArray[UDIndex].UDCounter = UDDataCounter;            // totalDataCount is total count of all computed datasets
    UDArray[UDIndex].totalPowerConsumption = ADArray.accumPowerConsumption;
    UDArray[UDIndex].totalMileage = ADArray.accumMileage;
    if (UDDataCounter >= 4292400000){ //reset UDDataCounter if it reaches 4292400000 counts
            UDDataCounter = 0;
    }
    get_UDArrayData();
    UDTriggerCounter = 0;
    sumDeltaPowerConsumed = 0;
    sumDeltaMileage = 0;
}
/*
 * @fn      dataSim
 *
 * @brief   Data analysis and simulates MCU data coming to the dashboard
 *
 * @param   jj
 *
 * @return  Nil
*/
extern void dataSim(uint32_t jj){
    //uint32_t deltaPowerConsumption, deltaMileage;
    // Simulates and fills the input data arrays with dummy MCU data
    *((*ptrrpm)+ xCount) = 188; //rand()%801;                      // unit in rpm = get rpm
    *((*ptrSp)+ xCount) = 200; //round((*((*ptrrpm)+ xCount))*2*(float) M_PI / 60 * WHEELRADIUS);       // Unit in cm / sec
    *((*ptrcb)+ xCount) = 3000; //rand()%13 * 1000;                 // unit in mA = get battery current in mA
    *((*ptrvb)+ xCount) = 40000; //*((*ptrvb)) - xCount * 10;         // unit in mV = get battery voltage in mV
    LEDSpeed(xCount);       // covert speed to the selected dashboard unit (dashSpeed) then sent to led display
    xCount++;
    // ************  Data analysis is carried out periodically for computing distance travelled, power consumed, economy etc.... Data are stored in fixed size arrays.
    // ************  Once the arrays are filled with fresh data, data parsing / computations are carried out.  The OR statement accomodates the case of SHUT DOWN where the last array is not fully filled.
    if (xCount >= DATA_ANALYSIS_POINTS){ // and when Power OFF
        dataAnalyt();
    }
    // The UDArray stores total Power Consumed and total distance travelled data in memory
    // It is stored once every (DATA_ANALYSIS_SAMPLING_TIME x (DATA_ANALYSIS_POINTS -1) x UDTRIGGER / 1000) seconds.
    // e.g. ( 300 ms x (31 -1) x 40 / 1000) = 360 seconds = 6 minutes
    if (UDTriggerCounter >= UDTRIGGER){     // and when Power_OFF)
        data2UDArray();
    }
}
/*
 * @fn      computePowerConsumption
 *
 * @brief   This function calculates the change in power consumption of the e_scooter
 *          over the time interval using Simpson's 1/3 Rule
 *
 * @param   AccumPowerConsumed
 *
 * @return  energy consumption value (unit milli W-hr) in type: uint32_t
*/
uint32_t computePowerConsumption(){
    uint32_t deltaPowerConsumption = 0;
    for(uint8_t ii=0; ii < DATA_ANALYSIS_POINTS; ii++){
        uint32_t tempholder = (batteryVoltage[ii] * batteryCurrent[ii]) * 0.0001;         // required to avoid possible byte size limitation issue
        deltaPowerConsumption += (*((*ptrc)+ ii)) * tempholder;
    }
    deltaPowerConsumption = round((float)deltaPowerConsumption/3000*DATA_ANALYSIS_SAMPLING_TIME/3600);       // output in milli-W-hr
    return deltaPowerConsumption;
}
/*
 * @fn      computeDistanceTravelled
 *
 * @brief   This function calculates the change of distance_travelled of the e_scooter
 *          over a period using Simpson's Rule.  The distance is in decimeter.
 *
 * @param   AccumMileage
 *
 * @return  distanceTravelled (unit in decimeters) in type: uint32_t
*/
uint32_t computeDistanceTravelled(){
    uint32_t deltaDistanceTravelled = 0;
    for(uint8_t x=0; x<DATA_ANALYSIS_POINTS; x++){
         deltaDistanceTravelled += ((*((*ptrc)+ x)) * speed[x]);            // for computational accuracy reasons, calculations are performed in centimeter/second
    }
    deltaDistanceTravelled = round((float)deltaDistanceTravelled*DATA_ANALYSIS_SAMPLING_TIME/30000);          // output is then converted to decimeter
    return deltaDistanceTravelled;
}
/*
 * @fn      averageSpeed
 *
 * @brief   This function calculates the average speed over the given time interval in km/hr.
 *
 * @param   DeltaDistanceTravelled
 *
  * @return  avgSpeed in km/hr
*/
uint8_t computeAverageSpeed(uint32_t deltaMileage){
    static uint8_t avgSpeed = 0; // output in km/hr
    if (xCount < 1) {
        avgSpeed = 0;
        // errorCode = speedError;
    } // Safeguard from stack overflow due to division by 0
    else {
        avgSpeed = round((float)(deltaMileage)/ (DATA_ANALYSIS_SAMPLING_TIME * (xCount - 1)) * 360); // output in km/hr
    }
    return avgSpeed; // output rounded off to nearest km/hr
}
/*
 * @fn      avgBatteryVoltage
 *
 * @brief   This function calculates the average of the battery voltage over the given time interval in mV.
 *
 * @param   batteryVoltage in mV
 *
 * @return  AvgBatteryVoltage in milli Volt
*/
uint32_t computeAvgBatteryVoltage(){
        avgBatteryVoltage = 0;
        for(uint8_t i=0; i < xCount; i++){
            avgBatteryVoltage += batteryVoltage[i];                             // Average is in mV
        }
        if (xCount < 1){    // Safeguard from stack overflow due to division by 0, but xCount is always greater than or equal 1
            avgBatteryVoltage = 0;
            //errorCode = batteryError;
        }
        else {avgBatteryVoltage = round((float) avgBatteryVoltage / (xCount));}        // output in mV
        return avgBatteryVoltage;
}
/*
 * @fn      computeBatteryPercentage
 *
 * @brief   This function computes the battery percentage based on the average battery voltage.
 *
 * @param   avgBatteryVoltage
 *
 * @return  batteryPercentage
*/
uint8_t computeBatteryPercentage(){
    if(avgBatteryVoltage > BATTERY_MAX_VOLTAGE) {avgBatteryVoltage = BATTERY_MAX_VOLTAGE;}      // ensure battery % cannot exceed 100%
    else if(avgBatteryVoltage < BATTERY_MIN_VOLTAGE) {avgBatteryVoltage = BATTERY_MIN_VOLTAGE;} // ensure battery % cannot be less than 0%
    uint8_t batteryPercentage = round((avgBatteryVoltage - BATTERY_MIN_VOLTAGE))* 100 /(BATTERY_MAX_VOLTAGE - BATTERY_MIN_VOLTAGE);
    return batteryPercentage;
}
/*
 * @fn      determineBatteryStatus
 *
 * @brief   This function returns the battery status based on the average battery voltage.
 *
 * @param   avgBatteryVoltage
 *
 * @return  batteryStatus
*/
uint8_t determineBatteryStatus(){
    if (avgBatteryVoltage > LEVEL45) {return GLOWING_AQUA;}
    else if (avgBatteryVoltage <= LEVEL45 && avgBatteryVoltage > LEVEL34) {return GLOWING_GREEN;}
    else if (avgBatteryVoltage <= LEVEL34 && avgBatteryVoltage > LEVEL23) {return YELLOW;}
    else if (avgBatteryVoltage <= LEVEL23 && avgBatteryVoltage > LEVEL12) {return ORANGE;}
    else if (avgBatteryVoltage <= LEVEL12 && avgBatteryVoltage > LEVEL01) {return RED;}
    else {return FLASHING_RED;}
}
/*
 * @fn      computeInstantEconomy
 *
 * @brief   Calculate the instananeous instantEconomy - Not used for now.
 *
 * @param   None
 *
 * @return  instantEconomy (in W-hr/km)
*/
float computeInstantEconomy(uint32_t deltaPowerConsumption, uint32_t deltaMileage){
    float instantEconomy = 0; // unit in W-hr / km
    if ((deltaMileage)<=0){
        instantEconomy=0;}      //******** Safeguard from stack overflow due to division by 0
    else {instantEconomy = (float)(deltaPowerConsumption) * 10 / deltaMileage;} // unit in W-hr / km
    return instantEconomy;
}
/*
 * @fn      computeEconomy
 *
 * @brief   This function calculates the economy (i.e.moving average of Whr/km) over the most recent 1.5 hours (1.5 = DATA_ANALYSIS_SAMPLING_TIME x DATA_EVALUATING_POINTS x NUMINDEX / 1000 / 3600).
 *
 * @param   None
 *
 * @return  economy (in W-hr/km)
*/
float computeEconomy(){
        float economy = 0;      // unit in W-hr / km
        if ((ADArray.accumMileage - totalMileage0) > 0) {economy = (float)(ADArray.accumPowerConsumption - totalPowerConsumed0) * 10 / (ADArray.accumMileage - totalMileage0);}  // Unit in W-hr / km
        else {economy = 9999.99;}           // Safeguard from stack overflow due to division by 0
        return economy; // Unit in W-hr / km
}
/*
 * @fn      computeRange
 *
 * @brief   This function calculates the range remaining in km
 *
 * @param   batteryCap, avgWhr (in Whr/km)
 *
 * @return  Range
*/
uint32_t computeRange(){
        float range = 0;
        if (ADArray.economy > 0) {range = ((float) ADArray.batteryPercentage * BATTERY_MAX_CAPACITY * BCF / ADArray.economy / 100);}       // output in metres
        else {range = 0;}           // Safeguard from stack overflow due to division by 0
        return range;   // output in metres
}
/*
 * @fn      computeCO2Saved
 *
 * @brief   This function calculates the hypertheical CO2Saved when compared to driving an average car
 *
 * @param   AccumPowerConsumed, AccumMileage
 *
 * @return  co2Saved
*/
float co2Saved = 0; // in kg
float computeCO2Saved(){
        //float co2Saved = 0; // in kg
        if (ADArray.accumMileage > 0){
            co2Saved = ((float) ADArray.accumMileage / 10000) * (COEFF01 - ((float) (ADArray.accumPowerConsumption) / (float) (ADArray.accumMileage) * 0.10) * COEFF02);  // in kg
        }
        else {co2Saved = 0;}            // Safeguard from stack overflow due to division by 0
        return co2Saved;
}
/*
 * @fn      re_Initialize
 *
 * @brief   This function re-initialize the data arrays after each data analysis loop.
 *
 * @param   none
 *
 * @return  none
*/
extern void re_Initialize(){
    //  Re-initialize arrays after completing each computation loop
            for(uint8_t kk=0; kk<DATA_ANALYSIS_POINTS; kk++){           // carry over the last datapoints to the 1st position [0] of the new dataset and reset all other to zero
                    if(kk == 0){
                            *(*(ptrrpm)) = *(*(ptrrpm)+DATA_ANALYSIS_POINTS-1);
                            *(*(ptrSp)) = *(*(ptrSp)+DATA_ANALYSIS_POINTS-1);
                            *(*(ptrcb)) = *(*(ptrcb)+DATA_ANALYSIS_POINTS-1);
                            *(*(ptrvb)) = *(*(ptrvb)+DATA_ANALYSIS_POINTS-1);
                    }
                    else {
                            *(*(ptrrpm)+kk) = 0;
                            *(*(ptrSp)+kk) = 0;
                            *(*(ptrcb)+kk) = 0;
                            *(*(ptrvb)+kk) = 0;
                    }
            }
            xCount = 1;  // Once computation completes (xCount >= DATA_ANALYSIS_POINTS), xCount is reset back to 1
            // At every power on, stateX = 0 by default. After completing the first computation loop, stateX is reset to 1 for the rest of computation loops.
}
/*********************************************************************
 * @fn      dataAnalysis_getUnitSelectDash
 *
 * @brief   call this function to retrieve the current Unit setting
 *
 * @param   None
 *
 * @return  UnitSelectDash
 *********************************************************************/
extern uint8_t dataAnalysis_getUnitSelectDash(){
    return UnitSelectDash;
}
/*********************************************************************
 * @fn      dataAnalysis_changeUnitSelectDash
 *
 * @brief   call this function to change/toggle the current active Unit
 *
 * @param   None
 *
 * @return  None
 *********************************************************************/
extern void dataAnalysis_changeUnitSelectDash(){
    if (UnitSelectDash == IMP_UNIT) {
        UnitSelectDash = SI_UNIT;
        lengthConvFactorDash = 1;
    }
    else if (UnitSelectDash == SI_UNIT) {
        UnitSelectDash = IMP_UNIT;
        lengthConvFactorDash = KM2MILE;
    }
    ledControl_setUnitSelectDash(UnitSelectDash);
}
