/*
 * PES_PROJECT_6.h
 *
 *  Created on: 08-Dec-2019
 *      Author: prayag
 */

#ifndef PES_PROJECT_6_H_
#define PES_PROJECT_6_H_


//The value of PI (not there in math.h)
#define M_PI 3.14159265358979323846

//LUT holding sine DAC outputs
extern uint16_t sinDAC[50];
//The LUT index that specifies the current element
//being sent to the DAC
extern uint8_t DACcounter;
//buffer transferred to by DMA
extern uint32_t destDMA[64];


#endif /* PES_PROJECT_6_H_ */
