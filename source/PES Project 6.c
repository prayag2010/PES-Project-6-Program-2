/**
 * @file    PES Project 6.c
 * @brief   Application entry point.
 */
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL25Z4.h"
#include "fsl_debug_console.h"
#include <math.h>
#include "dac_adc_drivers.h"
#include "loggerFunctions.h"
#include "ledControl.h"
#include "dma_driver.h"


/* Standard includes. */
#include <assert.h>
#include <string.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

/* Freescale includes. */
#include "fsl_device_registers.h"

#include "PES_PROJECT_6.h"


/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* The software timer period. */
#define SW_TIMER_PERIOD_MS (100 / portTICK_PERIOD_MS)
#define ADC_TIMER_PERIOD_MS (100 / portTICK_PERIOD_MS)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/* The callback function. */
static void SwTimerCallback(TimerHandle_t xTimer);
static void ADCTimerCallback(TimerHandle_t xTimer);


void generateLUT(void);
void ADCqueueInit(void);


/* Logger queue handle */
QueueHandle_t adcQueue = NULL;

#define MAXADCQITEMS 64
#define ADCQELEMENTSIZE (sizeof(float))

//LUT holding sine DAC outputs
uint16_t sinDAC[50] = {0};
//The LUT index that specifies the current element
//being sent to the DAC
uint8_t DACcounter = 0;
//buffer transferred to by DMA
uint32_t destDMA[64] = {0};


/*
 * @brief   Application entry point.
 */
int main(void)
{

	/* Init board hardware. */
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitBootPeripherals();
	/* Init FSL debug console. */
	BOARD_InitDebugConsole();

	SystemCoreClockUpdate();

	Init_RGB_LEDs();
	ledOff();

	EnableIRQ(ADC0_IRQn);

	log_message(DEBUG, __func__, "Generating LUT for sine wave");
	log_message(RELEASE, __func__, "Generating LUT for sine wave");
	generateLUT();
	log_message(DEBUG, __func__, "Initializing the DAC");
	DAC_Initialize();
	ADC_Initialize();
	DMA_Initialize();
	//	ADCqueueInit();

	//	for(int i = 0; i < 50; i++){
	//		PRINTF("%d\n", sinDAC[i]);
	//	}

	TimerHandle_t SwTimerHandle = NULL;
	TimerHandle_t ADCTimerHandle = NULL;

	log_message(DEBUG, __func__, "Creating and running a software timer to run every 100mS");
	SwTimerHandle = xTimerCreate("SwTimer",          /* Text name. */
			SW_TIMER_PERIOD_MS, /* Timer period. */
			pdTRUE,             /* Enable auto reload. */
			0,                  /* ID is not used. */
			SwTimerCallback);   /* The callback function. */

	ADCTimerHandle = xTimerCreate("ADCTimer",          /* Text name. */
			ADC_TIMER_PERIOD_MS,	 /* Timer period. */
			pdTRUE,            		 /* Enable auto reload. */
			0,               	     /* ID is not used. */
			ADCTimerCallback);   	 /* The callback function. */
	/* Start timer. */
	xTimerStart(SwTimerHandle, 0);
	xTimerStart(ADCTimerHandle, 0);

	/* Start scheduling. */
	log_message(DEBUG, __func__, "Updating signals from DAC every 100mS");
	log_message(RELEASE, __func__, "Updating signals from DAC every 100mS");

	adcQueue = xQueueCreate(64, sizeof(uint32_t));
	if(adcQueue == NULL){
		printf("Queue creation failed\n");
	} else {
		printf("Queue creation passed\n");
	}

	vTaskStartScheduler();

	return 0 ;
}

void generateLUT(void)
{
	//holds sin values (-1 to 1)
	double sinLUT[50] = {0};
	for(int i = 0; i < 50; i++){
		//fills a full sin wave of 50 points from 0 to 1
		sinLUT[i] = sin((double)M_PI * (i / (double)24.5));
		//Holds the scaled sin values for the DAC (in range of 1V - 3V)
		sinDAC[i] = (uint16_t)(((double)(sinLUT[i] + (double)1)/(double)2 * ((double)2479)) + (double)1239);
	}
}

static void ADCTimerCallback(TimerHandle_t xTimer)
{

	g_Adc16ConversionDoneFlag = false;
	ADC16_SetChannelConfig(DEMO_ADC16_BASEADDR, DEMO_ADC16_CHANNEL_GROUP, &g_adc16ChannelConfigStruct);

	while (!g_Adc16ConversionDoneFlag)
	{
	}

	//get the ADC value
	uint32_t ui = g_Adc16ConversionValue;
	//	xQueueSend(adcQueue, &ui, 0);
	//	uint32_t temp;
	//	xQueueReceive(adcQueue, &temp, 0);
	//	uint32_t* tt = adcQueue;
	//	printf("Pointer: %d\n", tt);
	//	printf("Received: %d\n", temp);


	if(xQueueSend(adcQueue, &ui, 0) == errQUEUE_FULL){
		printf("Queue Full, dumping all elements\n\n");

		uint32_t* tt;
		tt = adcQueue;
//		for(int i = 0; i < 64; i++){
//			tt = adcQueue;
//			printf("Custom %d: %d\n", i, tt[i + 20]);
//		}


		DMA_PrepareTransfer(&transferConfig, &tt[20], sizeof(tt[20]), destDMA, sizeof(destDMA[0]), 64 * sizeof(uint32_t),
				kDMA_MemoryToMemory);
		DMA_SubmitTransfer(&g_DMA_Handle, &transferConfig, kDMA_EnableInterrupt);
		DMA_StartTransfer(&g_DMA_Handle);

		uint32_t temp;
		while(xQueueReceive(adcQueue, &temp, 0) != errQUEUE_EMPTY);
	}

//	PRINTF("%d\r\n", ui);
}

static void SwTimerCallback(TimerHandle_t xTimer)
{
	//Turning the LED to blue
	ledOff();
	greenLED();

	//For logger time keeping
	tenth++;

	//Output the LUT value to the DAC
	DAC_SetBufferValue(DEMO_DAC_BASEADDR, 0U, sinDAC[DACcounter++]);

	//Reset the DAC LUT index back to zero if limit reached
	if(DACcounter > 49)
		DACcounter = 0;

	//Turning the blue LED off
	ledOff();
}

//void ADCqueueInit(void)
//{
//	adcQueue = xQueueCreate(32, sizeof(float));
//	if(adcQueue == NULL){
//		printf("Queue creation failed\n");
//	} else {
//		printf("Queue creation passed\n");
//	}
//}


