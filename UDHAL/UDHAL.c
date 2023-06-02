/*
 *  @file  UDHAL.c
 *  @brief This file contain the functions about
 *  Created on: 26 Jan 2022
 *      Author: harrysiu
 */
/*********************************************************************
 * INCLUDES
 */
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "UDHAL/UDHAL_GPIO.h"
//#include "UDHAL/UDHAL_GPIO2.h"
#include "UDHAL/UDHAL_I2C.h"
//#include "UDHAL/UDHAL_NVSINT.h"
#include "UDHAL/UDHAL_TIM1.h"
//#include "UDHAL/UDHAL_TIM2.h"
#include "UDHAL/UDHAL_TIM3.h"
#include "UDHAL/UDHAL_TIM4.h"
#include "UDHAL/UDHAL_TIM5.h"
#include "UDHAL/UDHAL_TIM6.h"
#include "UDHAL/UDHAL_TIM7.h"
#include "UDHAL/UDHAL_TIM8.h"
#include "UDHAL/UDHAL_TIM9.h"
#include "UDHAL/UDHAL_TIM10.h"
#include "UDHAL/UDHAL_ADC1.h"
#include "UDHAL/UDHAL_ADC2.h"
#include "UDHAL/UDHAL_UART.h"
/*********************************************************************
 * LOCAL VARIABLES
 */
/*********************************************************************
 * LOCAL FUNCTIONS
 */
/*********************************************************************
 * Marco
 */
/*********************************************************************
 * @fn      UDHAL_init
 *
 * @brief   It is used to initialize all peripherals
 *
 * @param   None
 *
 * @return  None
 */
void UDHAL_init()
{
    UDHAL_GPIO_init();
    UDHAL_GPIO_params_init();
    //UDHAL_GPIO2_init();
    //UDHAL_GPIO2_params_init();
    UDHAL_I2C_init();
    UDHAL_I2C_params_init();
    //UDHAL_NVSINT_init();
    //UDHAL_NVSINT_params_init();
    UDHAL_TIM1_init();
    UDHAL_TIM1_params_init();
    //UDHAL_TIM2_init();
    //UDHAL_TIM2_params_init();
    UDHAL_TIM3_init();              // single button source
    UDHAL_TIM3_params_init();       //  single button header
    UDHAL_TIM4_init();              // periodic communication hf source
    UDHAL_TIM4_params_init();       // periodic communication hf header
    UDHAL_TIM5_init();              // periodic communication lf source
    UDHAL_TIM5_params_init();       // periodic communication lf header
    UDHAL_TIM6_init();              // STM32 source
    UDHAL_TIM6_params_init();       // STM32 header
    UDHAL_TIM7_init();              // singleButton source
    UDHAL_TIM7_params_init();       // singleButton header
    UDHAL_TIM8_init();              // light control source
    UDHAL_TIM8_params_init();       // light control header
    UDHAL_TIM9_init();              // buzzer control source
    UDHAL_TIM9_params_init();       // buzzer control header
    UDHAL_TIM10_init();             // power on time source
    UDHAL_TIM10_params_init();      // power on time header
    UDHAL_ADC1_init();
    UDHAL_ADC1_params_init();
    UDHAL_ADC2_init();
    UDHAL_ADC2_params_init();
    UDHAL_UART_init();
    UDHAL_UART_params_init();
}
