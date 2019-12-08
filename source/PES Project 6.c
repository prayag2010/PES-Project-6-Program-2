/**
 * @file    PES Project 6.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL25Z4.h"
#include "fsl_debug_console.h"
#include <math.h>
#include "dac_adc_drivers.h"


/* Standard includes. */
#include <assert.h>
#include <string.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

/* Freescale includes. */
#include "fsl_device_registers.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* The software timer period. */
#define SW_TIMER_PERIOD_MS (100 / portTICK_PERIOD_MS)
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/* The callback function. */
static void SwTimerCallback(TimerHandle_t xTimer);


void generateLUT(void);

uint16_t sinDAC[50] = {0};
#define M_PI 3.14159265358979323846

uint8_t DACcounter = 0;


/*
 * @brief   Application entry point.
 */
int main(void) {

	/* Init board hardware. */
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitBootPeripherals();
	/* Init FSL debug console. */
	BOARD_InitDebugConsole();
	SystemCoreClockUpdate();

	generateLUT();
	DAC_Initialize();


	for(int i = 0; i < 50; i++){
		PRINTF("%d\n", sinDAC[i]);
	}

	TimerHandle_t SwTimerHandle = NULL;

	SwTimerHandle = xTimerCreate("SwTimer",          /* Text name. */
			SW_TIMER_PERIOD_MS, /* Timer period. */
			pdTRUE,             /* Enable auto reload. */
			0,                  /* ID is not used. */
			SwTimerCallback);   /* The callback function. */
	/* Start timer. */
	xTimerStart(SwTimerHandle, 0);
	/* Start scheduling. */
	vTaskStartScheduler();

	while(1) {
		__asm volatile ("nop");
	}
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

static void SwTimerCallback(TimerHandle_t xTimer)
{
//	PRINTF("Tick.\r\n");
	DAC_SetBufferValue(DEMO_DAC_BASEADDR, 0U, sinDAC[DACcounter++]);

	if(DACcounter > 49)
		DACcounter = 0;
}
