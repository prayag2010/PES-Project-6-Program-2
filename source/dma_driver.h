/*
 * dma_driver.h
 *
 *  Created on: 08-Dec-2019
 *      Author: prayag
 */

#ifndef DMA_DRIVER_H_
#define DMA_DRIVER_H_

#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_dma.h"
#include "fsl_dmamux.h"

#include "pin_mux.h"
#include "clock_config.h"

#define BUFF_LENGTH 64
#define DMA_CHANNEL 0
#define DMA_SOURCE 63

extern dma_handle_t g_DMA_Handle;
extern volatile bool g_Transfer_Done;

extern dma_transfer_config_t transferConfig;

void DMA_Callback(dma_handle_t *handle, void *param);
void DMA_Initialize(void);


#endif /* DMA_DRIVER_H_ */
