/*
 * @file    PES Project 6.c
 * @brief   The solution to the second Problem of PES Project 2
 *
 * This program outputs a sine wave from the DAC of 1V - 3V of a period of 5 seconds
 * and samples it from the ADC, and performs some basic DSP to display values such
 * as average, min, max, standard deviation
 * It uses DMA to transfer items from the ADC queue to the DSP buffer to perform these
 * calculations
 *
 * @date 8 Dec 2019
 * @author Rahul Ramaprasad, Prayag Desai
 * @version v1.0
 */

/* Standard includes. */
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

/* Freescale includes. */
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL25Z4.h"
#include "fsl_debug_console.h"
//#include <math.h>

/* USER HEADERS */
#include "dac_adc_drivers.h"
#include "loggerFunctions.h"
#include "ledControl.h"
#include "dma_driver.h"
#include "PES_PROJECT_6.h"


/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

/* Freescale includes. */
#include "fsl_device_registers.h"



/*******************************************************************************
 * Definitions
 ******************************************************************************/
//Mutex for LED control
SemaphoreHandle_t xMutex;


/* The software timer period. */
#define SW_TIMER_PERIOD_MS (100 / portTICK_PERIOD_MS)
#define ADC_TIMER_PERIOD_MS (100 / portTICK_PERIOD_MS)
#define DMA_TIMER_PERIOD_MS (100 / portTICK_PERIOD_MS)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
//Math operation function
static void mathOps(void *pvParameters);

//Generates the look up table
void generateLUT(void);

/* ADC input queue handle */
QueueHandle_t adcQueue = NULL;

//Items in the queue
#define MAXADCQITEMS 64
//SIze of the element in the queue
#define ADCQELEMENTSIZE (sizeof(float))

//LUT holding sine DAC outputs
uint16_t sinDAC[50] = {0};
//The LUT index that specifies the current element
//being sent to the DAC
uint8_t DACcounter = 0;
//buffer transferred to by DMA
uint32_t destDMA[64] = {0};
//Math Calculations task counttaskYIELD();
uint16_t mathCount = 0;
//Counts the number of times the LED has blinked
uint16_t ledCount = 0;
//Executes the DSP function
TaskHandle_t dmaMath;
//Handles the LED blink when DMA happens
TaskHandle_t dma_led_task_handle;
//Outputs the sine value to the DAC
TaskHandle_t dac_task_handle;
//Reads the sine value from the ADC
TaskHandle_t adc_task_handle;

//Task that samples the ADC
static void adc_task(void *pvParameters);
//Task that outputs sine to the DAC
static void dac_task(void *pvParameters);
//Updates the LED
void updateLed(void);
//Prints the math calculations
void printMath(void);

/*
 * @brief   Application entry point.
 */
int main(void)
{
	//mutex that keeps track of the LED
	xMutex = xSemaphoreCreateMutex();
	//
	/* Init board hardware. */
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitBootPeripherals();
	/* Init FSL debug console. */
	BOARD_InitDebugConsole();

	//Update the clock
	SystemCoreClockUpdate();

	//Init the LEDs
	Init_RGB_LEDs();
	ledOff();

	//Enable the ADC interrupts
	EnableIRQ(ADC0_IRQn);

	//log
	log_message(DEBUG, __func__, "Generating LUT for sine wave");
	log_message(RELEASE, __func__, "Generating LUT for sine wave");
	//Generate the sine LUT
	generateLUT();
	log_message(DEBUG, __func__, "Initializing the DAC");
	//Init the DAC
	DAC_Initialize();

	log_message(DEBUG, __func__, "Initializing the ADC");
	//Init the ADC
	ADC_Initialize();

	log_message(DEBUG, __func__, "Initializing the DMA");
	//Init the dMA
	DMA_Initialize();

	//Create the task that runs the ADC
	xTaskCreate(adc_task, "adc_task", configMINIMAL_STACK_SIZE + 128, NULL, tskIDLE_PRIORITY + 1, adc_task_handle);
	//Create the task
	xTaskCreate(dac_task, "dac_task", configMINIMAL_STACK_SIZE + 128, NULL, tskIDLE_PRIORITY + 1, dac_task_handle);


	log_message(DEBUG, __func__, "Updating signals from DAC every 100mS");
	log_message(RELEASE, __func__, "Updating signals from DAC every 100mS");
	log_message(DEBUG, __func__, "Reading signals from ADC every 100mS");
	log_message(RELEASE, __func__, "Reading signals from ADC every 100mS");

	//create the queue that holds the ADC values
	adcQueue = xQueueCreate(64, sizeof(uint32_t));
	//Check if the queue is NULL
	if(adcQueue == NULL){
		printf("Queue creation failed\n");
	} else {
		printf("Queue creation passed\n");
	}

	//Start the scheduler
	vTaskStartScheduler();
	for (;;)
		;
	return 0 ;
}

//Generate the sine LUT
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

//Task that samples the ADC values
static void adc_task(void *pvParameters)
{
	//Calculating 100 mS delay
	const TickType_t xDelay = 100 / portTICK_PERIOD_MS;
	//Last time the delay was processed
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();

	while(1)
	{
		log_message(DEBUG, __func__, "Reading ADC value");
		taskYIELD();
		//ADC conversion not done
		g_Adc16ConversionDoneFlag = false;
		//set the ADC channel
		ADC16_SetChannelConfig(DEMO_ADC16_BASEADDR, DEMO_ADC16_CHANNEL_GROUP, &g_adc16ChannelConfigStruct);
		//Wait till conversion is over
		while (!g_Adc16ConversionDoneFlag)
		{
			taskYIELD();
		}

		//get the ADC value
		uint32_t ui = g_Adc16ConversionValue;

		//Update the LED if it's free
		if(!uxSemaphoreGetCount(xMutex)){
			updateLed();
		}

		//Send item to the queue
		if(xQueueSend(adcQueue, &ui, 0) == errQUEUE_FULL){
			log_message(DEBUG, __func__, "ADC queue full, using DMA to transfer to DSP queue");
			log_message(RELEASE, __func__, "ADC queue full, using DMA to transfer to DSP queue");
			//Block the LED
			xSemaphoreTake(xMutex, 10);

			//Holds the pointer to the queue
			uint32_t* tt;
			tt = adcQueue;

			//Transfer contents of the queue to the DSP buffer
			DMA_PrepareTransfer(&transferConfig, &tt[20], sizeof(tt[20]), destDMA, sizeof(destDMA[0]), 64 * sizeof(uint32_t),
					kDMA_MemoryToMemory);
			//Prepare and start the transfer
			DMA_SubmitTransfer(&g_DMA_Handle, &transferConfig, kDMA_EnableInterrupt);
			DMA_StartTransfer(&g_DMA_Handle);
			//		taskYIELD();

			//Wait till transfer is done
			while(g_Transfer_Done != true);
			//In case the queue is not empty, empty it
			uint32_t temp;
			while(xQueueReceive(adcQueue, &temp, 0) != errQUEUE_EMPTY);

			//If it is the 5th cycle, calculate the values and terminate the program
			if(mathCount == 4){
				++mathCount;
				//Print math results
				printMath();
				//Kill all tasks
				printf("Terminating Program\n");
				vTaskSuspend(dac_task_handle);
				vTaskSuspend(adc_task_handle);
				vTaskSuspend(dmaMath);
				//Terminate
				while(1);
			}
			//Create the math calc task
			log_message(DEBUG, __func__, "Resuming the DSP task");
			if(xTaskCreate(mathOps, "Math OPS", configMINIMAL_STACK_SIZE + 256, NULL,tskIDLE_PRIORITY + 1, dmaMath ) == pdFALSE)
				printf("TASK CREATION FAILED\n");

			taskYIELD();
		}
		//Wait for 100mS
		vTaskDelayUntil( &xLastWakeTime, xDelay );
	}
}

//Outputs the sine wave to the DAC
static void dac_task(void *pvParameters)
{
	//Delay for 100mS
	const TickType_t xDelay = 100 / portTICK_PERIOD_MS;
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();

	while(1){
		log_message(DEBUG, __func__, "Updating DAC Value");
		taskYIELD();
		if(xSemaphoreTake(xMutex, 10) == pdTRUE){
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
			//    taskYIELD();
			xSemaphoreGive(xMutex);
			taskYIELD();
		}
		vTaskDelayUntil( &xLastWakeTime, xDelay );
	}
}

//Update the LED
void updateLed(void)
{
	//Increment the LED counter
	ledCount++;
	//Toggle the LED
	LED_BLUE_TOGGLE();
	//release the LED if count limit is reached
	if(ledCount == 5)
	{
		ledCount = 0;
		xSemaphoreGive(xMutex);
		ledOff();
		taskYIELD();
	}
}

//Perform the DSP calculations
static void mathOps(void *pvParameters)
{
	while(1){
		//		printf("MATH\n");
		if(g_Transfer_Done == true)
		{
			//To check if loop ran 5 times
			mathCount++;
			if(mathCount == 5)
			{
				//Terminate the program
				printf("Terminating Tasks\n");
				vTaskSuspend(dac_task_handle);
				vTaskSuspend(adc_task_handle);
				vTaskSuspend(dmaMath);
				while(1);
			}
			else
			{
				//Print the claculations
				printMath();
				//Suspend the program
				vTaskSuspend(dmaMath);
			}
		}
	}
}

//Print the DSP calculations
void printMath(void)
{
	log_message(DEBUG, __func__, "Printing DSP results:");
	log_message(RELEASE, __func__, "Printing DSP results:");
	//Counter
	int i;
	//Maximum value
	uint32_t max = destDMA[0], min = destDMA[0];
	//Average value
	float average = destDMA[0];
	//Standard deviation
	float sd = 0;
	//	uint32_t max1 = sinDAC[0], min1 = sinDAC[0];
	for ( i = 1 ; i < 64 ; i++)
	{
		//Get maximum
		if(destDMA[i] > max)
			max = destDMA[i];
		//Get minimum
		if(destDMA[i] < min)
			min = destDMA[i];
		//Get average
		average += destDMA[i];

		//		if(sinDAC[i] > max1)
		//			max1 = sinDAC[i];
		//		if(sinDAC[i] < min1)
		//			min1 = sinDAC[i];

	}
	//average
	average /= 64;
	//calculate standard deviation
	for (i = 0; i < 64; i++)
		sd += pow(destDMA[i] - average, 2);
	sd /= 64;
	sd = sqrt(sd);
	//	printf("Percentage difference = %d\n", (max - max1));
	//Print the values
	printf("Count = %d\n", mathCount);
	printf("Maximum = %d\n",max);
	printf("Minimum = %d\n",min);
	printf("Average = %f\n",average);
	printf("Standard Deviation = %f\n",sd);
}


