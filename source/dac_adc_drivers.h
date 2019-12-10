/*
 * @file dac_adc_driver.h
 * @brief Source file for using the DAC and the ADC
 *
 * This source file has functions that initializes the ADC
 * abd the DAC to sample and generate generate a sine wave of 1V - 3V
 * of a period of 5 seconds
 *
 * @authors Rahul Ramaprasad, Prayag Milan Desai
 * @date December 8 2019
 * @verison 1.0
 */


#ifndef DAC_ADC_DRIVERS_H_
#define DAC_ADC_DRIVERS_H_

#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_adc16.h"
#include "fsl_dac.h"

#include "clock_config.h"
#include "pin_mux.h"

//ADC and DAC configurations
#define DEMO_DAC_BASEADDR DAC0
#define DEMO_ADC16_BASEADDR ADC0
#define DEMO_ADC16_CHANNEL_GROUP 0U
#define DEMO_ADC16_USER_CHANNEL 0U /* PTE20, ADC0_SE0 */

//Max value of the ADC
#define VREF_BRD 3.300
//Max bits of the ADC
#define SE_12BIT 4096.0

//#define VREF_BRD 3.300
//#define SE_12BIT 4096.0

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/* Initialize ADC16 & DAC */
void DAC_Initialize(void);

void ADC_Initialize(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/
extern volatile bool g_Adc16ConversionDoneFlag;
extern volatile uint32_t g_Adc16ConversionValue;
extern adc16_channel_config_t g_adc16ChannelConfigStruct;



#endif /* DAC_ADC_DRIVERS_H_ */
