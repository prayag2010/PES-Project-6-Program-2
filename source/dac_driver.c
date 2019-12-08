/*
 * dac_driver.c
 *
 *  Created on: 08-Dec-2019
 *      Author: prayag
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

