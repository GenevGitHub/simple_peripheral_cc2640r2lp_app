/*
 * dataAnalysis.c
 *
 *  Created on: 13 Oct 2022
 *      Author: Chee
 */
/******************************************************************************
@file  distance_energy.c
@brief This file contains the functions for calculating:
    (1) distance travelled
    (2) power consumed
    (3) Average Speed
    (4) Average Battery Voltage
    (5) battery percentage
    (6) battery status
    (7) Instantaneous economy
    (8) Economy
    (9) Available Range
    (10) CO2 Saved
    The data are packaged and sent to mobile app, to LED display, to Cloud storage and saved in flash memory
*****************************************************************************/
/*********************************************************************
* INCLUDES
*/
#include "dataAnalysis.h"
#include "buzzerControl.h"
/*********************************************************************
* LOCAL VARIABLES
*/
// Simpson's 1/3 rule coefficients
static uint8_t coefficient_array[DATA_ANALYSIS_POINTS] = {0};       // same size as DATA_ANALYSIS_POINTS
//Default battery status at each POWER ON
uint8_t batteryStatus =  BATTERY_STATUS_INITIAL;
//
uint8_t UnitSelectDash = SI_UNIT;  // Keep the last units selected by user in memory, the same units is used on restart
uint8_t UnitSelectApp = SI_UNIT;    // Mobile app allow user to select the desired display unit - the App units and Dash units are NOT linked
//
static uint16_t rpm[DATA_ANALYSIS_POINTS] = {0};                    //revolutions per minute data collected every time interval
static uint16_t speed[DATA_ANALYSIS_POINTS] = {0};                  //rpm is converted to cm per second (cm/s)
static uint16_t batteryCurrent[DATA_ANALYSIS_POINTS] = {0};         //battery current data collected every time interval
static uint16_t batteryVoltage[DATA_ANALYSIS_POINTS] = {0};         //battery voltage data collected every time interval
//Pointers to various arrays
static uint8_t (*ptrc)[DATA_ANALYSIS_POINTS] = &coefficient_array;
static uint16_t (*ptrrpm)[DATA_ANALYSIS_POINTS] = &rpm;
static uint16_t (*ptrSp)[DATA_ANALYSIS_POINTS] = &speed;
static uint16_t (*ptrvb)[DATA_ANALYSIS_POINTS] = &batteryVoltage;
static uint16_t (*ptrcb)[DATA_ANALYSIS_POINTS] = &batteryCurrent;

static uint16_t xCount = 0;                     // At initialization, xCount begins at 0. After the 1st set, xCount begins at index 1.
static uint8_t stateX = 0;
static uint8_t stateY;                          // stateY is used for initialisation only
static uint8_t startup;
static time_t t;
static uint16_t UDIndex;                        // the last UDindex saved
static uint16_t UDIndexOldest;
static uint32_t DataCountStatus = 10;           // totalDataCount must be stored in memory - recall at every Power On - the last saved totalDataCount
static UD UDArray[NUMUDINDEX] = {0};            // UDArray is the "storage array", stores the last 1.5 hours of usage data and is saved in flash memory.  UDArray are zero arrays at new.
static AD ADArray = {0};                        //Since this data set is temporary, array struct is not necesary.  ADArray data that are displayed on the mobile app.
static float lengthConvFactorDash;
static uint16_t batteryVoltageStartUp;

uint8_t batteryPercentageInitial;
uint16_t avgBatteryVoltage;
static uint8_t batteryPercentage;

static uint32_t AccumMileagePrev;              // unit in decimeters.  This is the previous data on the total distance travelled
static uint32_t AccumPowerConsumedPrev;        // unit in milli-W-hr.  This is the previous data on the total power consumption
static uint32_t AccumMileage0;                 // unit in decimeters.  This is the oldest data on total distance travelled stored in storage array
static uint32_t AccumPowerConsumed0;           // unit in milli-W-hr.  This is the oldest data on total power consumed stored in storage array
/*********************************************************************
*
* LOCAL FUNCTIONS
*/
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
        // get data from MCU at every MCU_SAMPLING_TIME interval
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
    // At the instant of POWER ON, we need to obtain BATTERY status for LED display
    batteryVoltageStartUp = 32000;
    avgBatteryVoltage = batteryVoltageStartUp;
    batteryPercentageInitial = computeBatteryPercentage();
    batteryStatus = determineBatteryStatus();
    dummyData();    //retrieve data from memory
    //initialize Simpson's 1/3 rule coefficient_array
    coefficient_array_init();
    //Unit Selection
    if (UnitSelectDash == IMP_UNIT) {lengthConvFactorDash = KM2MILE;}
    else if (UnitSelectDash == SI_UNIT) {lengthConvFactorDash = 1;}
    startup = 1;
    srand((unsigned) time(&t));
    // Action: Send battery status to LED display
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
        //
        /*if (stateX == 0) {*((*ptrvb))=BATTERY_MAX_VOLTAGE;}     // This line is only used for Data Simulation, shall be deleted if battery voltage is obtained from MCU/dashboard
        // Simulates and fills the input data arrays with dummy MCU data
        *((*ptrrpm)+ xCount) = rand()%801;                      // unit in rpm = get rpm
        *((*ptrSp)+ xCount) = round((*((*ptrrpm)+ xCount))*2*(float) M_PI / 60 * WHEELRADIUS);       // Unit in cm / sec
        *((*ptrcb)+ xCount) = rand()%13 * 1000;                 // unit in mA = get battery current in mA
        *((*ptrvb)+ xCount) = *((*ptrvb)) - xCount * 10;         // unit in mV = get battery voltage in mV
        // **** Action:  covert speed to the selected dashboard unit (dashSpeed) then sent to led display
        float dashSpeed = ((float) speed[xCount] * 0.036 * lengthConvFactorDash);  // dash speed is in km/hour
        // Action: Send dashSpeed to LED display immediately
        // Increment xCount by 1
        xCount++;
        //  Data analysis is carried out periodically for computing distance travelled, power consumed, economy etc.... Data are stored in fixed size arrays.
        //  Once the arrays are filled with fresh data, data parsing / computations are carried out.  The OR statement accommodates the case of SHUT DOWN where the last array is not fully filled.
        if ((xCount >= DATA_ANALYSIS_POINTS)||(jj >= jjMax-1)){  // (jj >= jjMax -1) simulates the case of device Shut Down with partially fill arrays
                //
                get_AnalysisData();
                // Compute UDArray Data
                UDArray[UDIndex].dataCount = DataCountStatus;           // totalDataCount is total count of all computed data sets
                UDArray[UDIndex].dataTimeStamp = jj;                    // jj is a dummy value - actual timeStamp should be assigned
                UDArray[UDIndex].accumPowerConsumption = AccumPowerConsumedPrev + computePowerConsumption();
                UDArray[UDIndex].accumMileage = AccumMileagePrev + computeDistanceTravelled();
                UDArray[UDIndex].avgSpeed = computeAverageSpeed();    // average speed is optional
                UDArray[UDIndex].avgBatteryVoltage = computeAvgBatteryVoltage();
                UDArray[UDIndex].errorCode = 7;
                // Compute ADArray Data
                ADArray.batteryStatus = determineBatteryStatus();
                ADArray.batteryPercentage = computeBatteryPercentage();
                ADArray.instantEconomy = computeInstantEconomy();
                ADArray.economy = computeEconomy();
                ADArray.range = computeRange();
                ADArray.co2Saved = computeCO2Saved();
                // Re-initialize arrays after data analysis
                re_Initialize();
                DataCountStatus++;
                UDIndex++;
                UDIndexOldest++;
                //  Action: Send battery status to led display
                //  Action: Send UDArray and ADArray Struct to Mobile App
        }
        */
}
//******************Dummy data simulates data retrieve from flashing memory***********
// Description: The past 1.5 hours of usage data are stored on flash memory
//              Economy calculations require past data on distance travelled and power consumption to be known, which
//              will require data to be retrieved from memory.
//              Below is a set of dummy data that simulates some data assigned and stored in memory
//************************************************************************************
extern void dummyData(){
//  dummy data for testing assigning data to array storage
        UDArray[0].dataCount = 0;
        UDArray[0].accumMileage =100;
        UDArray[0].accumPowerConsumption = 150;
        UDArray[1].dataCount = 1;
        UDArray[1].accumMileage =200;
        UDArray[1].accumPowerConsumption = 250;
        UDArray[2].dataCount = 2;
        UDArray[2].accumMileage =300;
        UDArray[2].accumPowerConsumption = 350;
        UDArray[3].dataCount = 3;
        UDArray[3].accumMileage =400;
        UDArray[3].accumPowerConsumption = 450;
        UDArray[4].dataCount = 4;
        UDArray[4].accumMileage =500;
        UDArray[4].accumPowerConsumption = 550;
        UDArray[5].dataCount = 5;
        UDArray[5].accumMileage =600;
        UDArray[5].accumPowerConsumption = 650;
        UDArray[6].dataCount = 6;
        UDArray[6].accumMileage =700;
        UDArray[6].accumPowerConsumption = 750;
        UDArray[7].dataCount = 7;
        UDArray[7].accumMileage =800;
        UDArray[7].accumPowerConsumption = 850;
        UDArray[8].dataCount = 8;
        UDArray[8].accumMileage =900;
        UDArray[8].accumPowerConsumption = 950;
        UDArray[9].dataCount = 9;
        UDArray[9].accumMileage =1000;
        UDArray[9].accumPowerConsumption = 1050;
}
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
/*
 * @fn      computePowerConsumption
 *
 * @brief   This function calculates the change in power consumption of the e_scooter
 *          over the time interval using Simpson's 1/3 Rule
 *
 * @param   none
 *
 * @return  energy consumption value (unit milli W-hr) in type: uint32_t
*/
extern uint32_t computePowerConsumption(){
    uint32_t deltaPowerConsumption = 0; // ensure deltaPowerConsumption is zero at start of cal
    for(uint8_t ii=0; ii < DATA_ANALYSIS_POINTS; ii++){                          // Simpson's 1/3 rule
        uint32_t tempholder = (batteryVoltage[ii] * batteryCurrent[ii])/1000;         // required to avoid bite size limitation issues
        deltaPowerConsumption += (*((*ptrc)+ ii)) * tempholder;
    }
    deltaPowerConsumption = round((float)deltaPowerConsumption/3000*MCU_SAMPLING_TIME/3600);       // output in milli-W-hr
    return deltaPowerConsumption;
}
/*
 * @fn      computeDistanceTravelled
 *
 * @brief   This function calculates the change of distance_travelled of the e_scooter
 *          over a period using Simpson's Rule.  The distance is in decimeter.
 *
 * @param   none
 *
 * @return  distanceTravelled (unit in decimeters) in type: uint32_t
*/
extern uint32_t computeDistanceTravelled(){     // Simpson's 1/3 rule
    uint32_t deltaDistanceTravelled = 0;                                    // always reset deltaDistanceTravelled to zero at start of cal
    for(uint8_t x=0; x<DATA_ANALYSIS_POINTS; x++){
         deltaDistanceTravelled += ((*((*ptrc)+ x)) * speed[x]);            // output is in centimeter/second
    }
    deltaDistanceTravelled = round((float)deltaDistanceTravelled*MCU_SAMPLING_TIME/30000);          // output in decimeter
    return deltaDistanceTravelled;
}
/*
 * @fn      computeAverageSpeed
 *
 * @brief   This function calculates the average speed over the given time interval in km/hr.
 *
 * @param   none
 *
  * @return  avgSpeed in km/hr
*/
extern uint8_t computeAverageSpeed(){
    static uint8_t avgSpeed = 0; // output in km/hr
    // Safeguard from stack overflow due to division by 0
    if (xCount < 1) {avgSpeed = 0;}
    else {
        avgSpeed = round((float)(UDArray[UDIndex].accumMileage - AccumMileagePrev)/ (MCU_SAMPLING_TIME * (xCount - 1)) * 360);
    }
    return avgSpeed; // output in km/hr
}
/*
 * @fn      computeAvgBatteryVoltage
 *
 * @brief   This function calculates the average of the battery voltage over the given time interval in mV.
 *
 * @param   none
 *
 * @return  AvgBatteryVoltage in milli Volt
*/
extern uint32_t computeAvgBatteryVoltage(){
        // Safeguard from stack overflow due to division by 0
        avgBatteryVoltage = 0;
        for(uint8_t i=0; i < xCount; i++){
            avgBatteryVoltage += batteryVoltage[i];
        }
        avgBatteryVoltage = round((float) avgBatteryVoltage / (xCount));           // output in mV

        return avgBatteryVoltage;
}
/*
 * @fn      computeBatteryPercentage
 *
 * @brief   This function computes the battery percentage based on the average battery voltage.
 *
 * @param   none
 *
 * @return  batteryPercentage
*/
extern uint8_t computeBatteryPercentage(){
    uint8_t TIM9_Status = 0;
    if(avgBatteryVoltage > BATTERY_MAX_VOLTAGE) {avgBatteryVoltage = BATTERY_MAX_VOLTAGE;}
    else if(avgBatteryVoltage < BATTERY_MIN_VOLTAGE) {avgBatteryVoltage = BATTERY_MIN_VOLTAGE;}
    batteryPercentage = round((avgBatteryVoltage - BATTERY_MIN_VOLTAGE))* 100 /(BATTERY_MAX_VOLTAGE - BATTERY_MIN_VOLTAGE);

    if (batteryPercentage < BATTERY_PERCENTAGE_LL){
        TIM9_Status = 1;
        buzzerControl_Start();    //call buzzerControl.c -> buzzerControl_Start()
    }
    if (TIM9_Status == 1){
        if (batteryPercentage > BATTERY_PERCENTAGE_LH){
            TIM9_Status = 0;
            buzzerControl_Stop();     //call buzzerControl.c -> buzzerControl_Stop()
        }
    }
    return batteryPercentage;
}
/*
 * @fn      determineBatteryStatus
 *
 * @brief   This function returns the battery status based on the average battery voltage.
 *
 * @param   none
 *
 * @return  batteryStatus
*/
extern uint8_t determineBatteryStatus(){
    if (avgBatteryVoltage > LEVEL45) {return GLOWING_AQUA;}
    else if (avgBatteryVoltage <= LEVEL45 && avgBatteryVoltage > LEVEL34) {return GLOWING_GREEN;}
    else if (avgBatteryVoltage <= LEVEL34 && avgBatteryVoltage > LEVEL23) {return YELLOW;}
    else if (avgBatteryVoltage <= LEVEL23 && avgBatteryVoltage > LEVEL12) {return ORANGE;}
    else if (avgBatteryVoltage <= LEVEL12 && avgBatteryVoltage > LEVEL01) {return RED;}
    else {return FLASHING_RED;}
}
/*
 * @fn      getBatteryPercentage
 *
 * @brief   This function returns the battery percentage anytime that it is called.
 *
 * @param   none
 *
 * @return  batteryPercent
*/
extern uint8_t getBatteryPercentage()
{
    return batteryPercentage;
}
/*
 * @fn      computeInstantEconomy
 *
 * @brief   Calculate the instantaneous instantEconomy - Not used for now.
 *
 * @param   none
 *
 * @return  instantEconomy (in W-hr/km)
*/
extern float computeInstantEconomy(){
    float instantEconomy = 0; // unit in W-hr / km
    //******** Safeguard from stack overflow due to division by 0
    if ((UDArray[UDIndex].accumMileage-AccumMileagePrev)<=0){
        instantEconomy=0;}
    else {instantEconomy = (float)(UDArray[UDIndex].accumPowerConsumption-AccumPowerConsumedPrev) * 10 / (UDArray[UDIndex].accumMileage-AccumMileagePrev);} // unit in W-hr / km
    return instantEconomy;
}
/*
 * @fn      computeEconomy
 *
 * @brief   This function calculates the economy (i.e.moving average of Whr/km) over the most recent 1.5 hours (1.5 = MCU_SAMPLING_TIME x DATA_EVALUATING_POINTS x NUMINDEX / 1000 / 3600).
 *
 * @param   none
 *
 * @return  economy (in W-hr/km)
*/
extern float computeEconomy(){
        float economy = 0;      // unit in W-hr / km
        // Safeguard from stack overflow due to division by 0
        if ((UDArray[UDIndex].accumMileage - AccumMileage0) > 0) {economy = (float)(UDArray[UDIndex].accumPowerConsumption - AccumPowerConsumed0) * 10 / (UDArray[UDIndex].accumMileage - AccumMileage0);}  // Unit in W-hr / km
        else {economy = 9999.99;}
        return economy; // Unit in W-hr / km
}
/*
 * @fn      computeRange
 *
 * @brief   This function calculates the range remaining in km
 *
 * @param   none
 *
 * @return  Range
*/
extern uint32_t computeRange(){
        float range = 0;
        // Safeguard from stack overflow due to division by 0
        if (ADArray.economy > 0) {range = ((float) ADArray.batteryPercentage * BATTERY_MAX_CAPACITY * BCF / ADArray.economy / 100);}       // output in metres
        else {range = 0;}
        return range;   // output in metres
}
/*
 * @fn      computeCO2Saved
 *
 * @brief   This function calculates the hypertheical CO2Saved when compared to driving an average car
 *
 * @param   none
 *
 * @return  co2Saved
*/
extern float computeCO2Saved(){
        float co2Saved = 0; // in kg
        // Safeguard from stack overflow due to division by 0
        if (UDArray[UDIndex].accumMileage > 0){
            co2Saved = ((float) UDArray[UDIndex].accumMileage / 10000) * (COEFF01 - ((float) (UDArray[UDIndex].accumPowerConsumption) / (float) (UDArray[UDIndex].accumMileage) * 0.10) * COEFF02);  // in kg
        }
        else {co2Saved = 0;}
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
        //******** Re-initialize arrays after completing each computation loop
        for(uint8_t kk=0; kk<DATA_ANALYSIS_POINTS; kk++){           // carries the last set of datapoints to the 1st position of the next dataset
                if(kk == 0){
                        *(*(ptrrpm)) = *(*(ptrrpm)+DATA_ANALYSIS_POINTS-1);
                        *(*(ptrSp)) = *(*(ptrSp)+DATA_ANALYSIS_POINTS-1);
                        if (*((*ptrvb)) <= BATTERY_MIN_VOLTAGE){
                                *(*(ptrvb)) = BATTERY_MAX_VOLTAGE;      // Simulates recharged battery once battery voltage is below the minimum battery voltage
                        }
                        else {
                                *(*(ptrvb)) = *(*(ptrvb)+DATA_ANALYSIS_POINTS-1);
                        }
                        *(*(ptrcb)) = *(*(ptrcb)+DATA_ANALYSIS_POINTS-1);
                }
                else {
                        *(*(ptrrpm)+kk) = 0;
                        *(*(ptrSp)+kk) = 0;
                        *(*(ptrvb)+kk) = 0;
                        *(*(ptrcb)+kk) = 0;
                }
        }
        AccumPowerConsumedPrev = UDArray[UDIndex].accumPowerConsumption;
        AccumMileagePrev = UDArray[UDIndex].accumMileage;
        xCount = 1;  // Once computation completes (xCount >= DATA_ANALYSIS_POINTS), xCount is reset back to 1
        // At every power on, stateX = 0 by default. After completing the first computation loop, stateX is reset to 1 for the rest of computation loops.
        stateX = 1;
}
/*
 * @fn      get_AnalysisData
 *
 * @brief   conditions and get data ready for data analysis at the STARTUP and NON-STARTUP stuations
 *
 * @param   Nil
 *
 * @return  Nil
*/
extern void get_AnalysisData(){
        //
        if (DataCountStatus >= NUMUDINDEX) {stateY = 1;}
        else {stateY = 0;}
        if (startup == 1){
        // this is only require at POWER ON
            UDIndex = DataCountStatus % NUMUDINDEX;  // this index the starting location (index) of the storage array
            UDIndexOldest = UDIndex + 1;            // the Index lcoating the oldest dataset location in storage array
        }
        //UDIndex cannot be greater than NUMUDINDEX since it is a remainder of the division
        if (UDIndex >= NUMUDINDEX) {UDIndex = 0;}  // loops UDIndex back to 0 when it reaches the end
        if (UDIndexOldest >= NUMUDINDEX){UDIndexOldest = 0;} // loops UDIndexOldest back to 0 when it reaches the end
        if (startup == 1){
            if (stateY == 0){
                if (UDIndex == 0){
                        AccumMileagePrev = 0;
                        AccumPowerConsumedPrev = 0;
                }
                else {  AccumMileagePrev = UDArray[UDIndex - 1].accumMileage;    //
                        AccumPowerConsumedPrev = UDArray[UDIndex - 1].accumPowerConsumption;
                }
                AccumMileage0 = 0;
                AccumPowerConsumed0 = 0;
            }
            else {
                // if UDIndex is at the start (0) of the array
                if (UDIndex == 0){
                        AccumMileagePrev = UDArray[UDIndex + NUMUDINDEX].accumMileage;
                        AccumPowerConsumedPrev = UDArray[UDIndex + NUMUDINDEX].accumPowerConsumption;
                        AccumMileage0 = UDArray[UDIndex + 1].accumMileage;
                        AccumPowerConsumed0 = UDArray[UDIndex + 1].accumPowerConsumption;
                }
                else {  AccumMileagePrev = UDArray[UDIndex - 1].accumMileage;
                        AccumPowerConsumedPrev = UDArray[UDIndex - 1].accumPowerConsumption;
                        // if UDIndex is at the end (NUMUDINDEX) of the array
                        if (UDIndex == NUMUDINDEX - 1){AccumMileage0 = UDArray[0].accumMileage;
                                AccumPowerConsumed0 = UDArray[0].accumPowerConsumption;
                        }
                        else{AccumMileage0 = UDArray[UDIndex + 1].accumMileage;
                                AccumPowerConsumed0 = UDArray[UDIndex + 1].accumPowerConsumption;
                        }
                }
            }
            startup = 0;
        }
        else {
                if (stateY == 0) {
                        AccumPowerConsumed0 = 0;
                        AccumMileage0 = 0;
                }
                else {  AccumPowerConsumed0 = UDArray[UDIndexOldest].accumPowerConsumption;
                        AccumMileage0 = UDArray[UDIndexOldest].accumMileage;
                }
        }
}
/*********************************************************************
 * @fn      getUnitSelectDash
 *
 * @brief   call this function to retrieve the current Unit setting
 *
 * @param   None
 *
 * @return  UnitSelectDash
 *********************************************************************/
extern uint8_t getUnitSelectDash(){
    return UnitSelectDash;
}
/*********************************************************************
 * @fn      changeUnitSelectDash
 *
 * @brief   call this function to change/toggle the current active Unit
 *
 * @param   None
 *
 * @return  None
 *********************************************************************/
extern void changeUnitSelectDash(){
    //UnitSelectDash = dashUnit;
    if (UnitSelectDash == IMP_UNIT) {
        UnitSelectDash = SI_UNIT;
        lengthConvFactorDash = 1;
    }
    else if (UnitSelectDash == SI_UNIT) {
        UnitSelectDash = IMP_UNIT;
        lengthConvFactorDash = KM2MILE;
    }
    //return 0;
}
