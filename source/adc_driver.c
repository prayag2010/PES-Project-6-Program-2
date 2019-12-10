/*
 * @file adc_driver.c
 * @brief Source file for using the ADC
 *
 * This source file has functions that
 * uses the ADC to sample a sine wave of 1V - 3V
 * of a period of 5 seconds
 *
 * @authors Rahul Ramaprasad, Prayag Milan Desai
 * @date December 8 2019
 * @verison 1.0
 */

#include "dac_adc_drivers.h"


volatile bool g_Adc16ConversionDoneFlag = false;
volatile uint32_t g_Adc16ConversionValue = 0;
adc16_channel_config_t g_adc16ChannelConfigStruct;

void ADC_Initialize(void)
{
	adc16_config_t adc16ConfigStruct;

	/* Configure the ADC16. */
	    /*
	     * adc16ConfigStruct.referenceVoltageSource = kADC16_ReferenceVoltageSourceVref;
	     * adc16ConfigStruct.clockSource = kADC16_ClockSourceAsynchronousClock;
	     * adc16ConfigStruct.enableAsynchronousClock = true;
	     * adc16ConfigStruct.clockDivider = kADC16_ClockDivider8;
	     * adc16ConfigStruct.resolution = kADC16_ResolutionSE12Bit;
	     * adc16ConfigStruct.longSampleMode = kADC16_LongSampleDisabled;
	     * adc16ConfigStruct.enableHighSpeed = false;
	     * adc16ConfigStruct.enableLowPower = false;
	     * adc16ConfigStruct.enableContinuousConversion = false;
	     */
	    ADC16_GetDefaultConfig(&adc16ConfigStruct);
	#if defined(BOARD_ADC_USE_ALT_VREF)
	    adc16ConfigStruct.referenceVoltageSource = kADC16_ReferenceVoltageSourceValt;
	#endif
	    ADC16_Init(DEMO_ADC16_BASEADDR, &adc16ConfigStruct);

	    /* Make sure the software trigger is used. */
	    ADC16_EnableHardwareTrigger(DEMO_ADC16_BASEADDR, false);
	#if defined(FSL_FEATURE_ADC16_HAS_CALIBRATION) && FSL_FEATURE_ADC16_HAS_CALIBRATION
	    if (kStatus_Success == ADC16_DoAutoCalibration(DEMO_ADC16_BASEADDR))
	    {
	        PRINTF("\r\nADC16_DoAutoCalibration() Done.\n");
	    }
	    else
	    {
	        PRINTF("ADC16_DoAutoCalibration() Failed.\r\n");
	    }
	#endif /* FSL_FEATURE_ADC16_HAS_CALIBRATION */

	    /* Prepare ADC channel setting */
	    g_adc16ChannelConfigStruct.channelNumber = DEMO_ADC16_USER_CHANNEL;
	    g_adc16ChannelConfigStruct.enableInterruptOnConversionCompleted = true;

	#if defined(FSL_FEATURE_ADC16_HAS_DIFF_MODE) && FSL_FEATURE_ADC16_HAS_DIFF_MODE
	    g_adc16ChannelConfigStruct.enableDifferentialConversion = false;
	#endif /* FSL_FEATURE_ADC16_HAS_DIFF_MODE */
}


void ADC0_IRQHandler(void)
{
    g_Adc16ConversionDoneFlag = true;
    /* Read conversion result to clear the conversion completed flag. */
    g_Adc16ConversionValue = ADC16_GetChannelConversionValue(DEMO_ADC16_BASEADDR, DEMO_ADC16_CHANNEL_GROUP);
}
