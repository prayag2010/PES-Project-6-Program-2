/*
 * @file dma_driver.c
 * @brief Header file for using the DMA
 *
 * This header file has prototypes for functions that
 * uses the DMA to transfer contents from ADC buffer
 * to DSP buffer
 *
 * @authors Rahul Ramaprasad, Prayag Milan Desai
 * @date December 8 2019
 * @verison 1.0
 */

#ifndef DMA_DRIVER_H_
#define DMA_DRIVER_H_

#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_dma.h"
#include "fsl_dmamux.h"

#include "pin_mux.h"
#include "clock_config.h"

//Length of the buffer to be copies to
#define BUFF_LENGTH 64
//DMA channel
#define DMA_CHANNEL 0
//Source of the DMA
#define DMA_SOURCE 63

//DMA handler
extern dma_handle_t g_DMA_Handle;
//Is the transfer done
extern volatile bool g_Transfer_Done;

//Config of DMA
extern dma_transfer_config_t transferConfig;

/*
 * @brief Callback function of DMA
 *
 * This function is executed when DMA transfer is done
 * Sets a flag
 *
 * @param The handler, The function parameters as a void pointer
 * @return void
 */
void DMA_Callback(dma_handle_t *handle, void *param);

/*
 * @brief Initializes the DMA
 *
 * This function configures the DMA to transfer 64 bytes from
 * the ADC to the DSP buffer
 *
 * @return void
 */
void DMA_Initialize(void);


#endif /* DMA_DRIVER_H_ */
