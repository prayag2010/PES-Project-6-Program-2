/*
 * PES_PROJECT_6.h
 *
 *  Created on: 08-Dec-2019
 *      Author: prayag
 */

#ifndef PES_PROJECT_6_H_
#define PES_PROJECT_6_H_

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"


//The value of PI (not there in math.h)
#define M_PI 3.14159265358979323846

//LUT holding sine DAC outputs
extern uint16_t sinDAC[50];
//The LUT index that specifies the current element
//being sent to the DAC
extern uint8_t DACcounter;
//buffer transferred to by DMA
extern uint32_t destDMA[64];
//Queue that holds the ADC values
extern QueueHandle_t adcQueue;


#endif /* PES_PROJECT_6_H_ */
