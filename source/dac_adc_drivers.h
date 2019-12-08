/*
 * dac_adc_drivers.h
 *
 *  Created on: 08-Dec-2019
 *      Author: prayag
 */

#ifndef DAC_ADC_DRIVERS_H_
#define DAC_ADC_DRIVERS_H_

#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_adc16.h"
#include "fsl_dac.h"

#include "clock_config.h"
#include "pin_mux.h"

#define DEMO_DAC_BASEADDR DAC0



#define VREF_BRD 3.300
#define SE_12BIT 4096.0

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/* Initialize ADC16 & DAC */
void DAC_Initialize(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/
extern volatile bool g_Adc16ConversionDoneFlag;
extern volatile uint32_t g_Adc16ConversionValue;
extern adc16_channel_config_t g_adc16ChannelConfigStruct;



#endif /* DAC_ADC_DRIVERS_H_ */
