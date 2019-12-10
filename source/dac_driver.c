/*
 * @file dac_driver.c
 * @brief Source file for using the DAC
 *
 * This source file has functions that
 * uses the DAC to generate a sine wave of 1V - 3V
 * of a period of 5 seconds
 *
 * @authors Rahul Ramaprasad, Prayag Milan Desai
 * @date December 8 2019
 * @verison 1.0
 */

#include "dac_adc_drivers.h"



/*******************************************************************************
 * Code
 ******************************************************************************/
void DAC_Initialize(void)
{
    dac_config_t dacConfigStruct;

    /* Configure the DAC. */
    /*
     * dacConfigStruct.referenceVoltageSource = kDAC_ReferenceVoltageSourceVref2;
     * dacConfigStruct.enableLowPowerMode = false;
     */
    DAC_GetDefaultConfig(&dacConfigStruct);
    DAC_Init(DEMO_DAC_BASEADDR, &dacConfigStruct);
    DAC_Enable(DEMO_DAC_BASEADDR, true); /* Enable output. */
}

