/*
 * @file dma_driver.h
 * @brief Source file for using the DMA
 *
 * This source file has functions that
 * uses the DMA to transfer contents from ADC buffer
 * to DSP buffer
 *
 * @authors Rahul Ramaprasad, Prayag Milan Desai
 * @date December 8 2019
 * @verison 1.0
 */


#include "dma_driver.h"
#include "PES_PROJECT_6.h"
#include "loggerFunctions.h"

dma_handle_t g_DMA_Handle;
volatile bool g_Transfer_Done = false;
dma_transfer_config_t transferConfig;

/* User callback function for DMA transfer. */
void DMA_Callback(dma_handle_t *handle, void *param)
{
	g_Transfer_Done = true;
	xQueueReset(adcQueue);
	log_message(DEBUG, __func__, "DMA Transfer completed");
	log_message(RELEASE, __func__, "DMA Transfer completed");
}

//Initialize the DMA
void DMA_Initialize(void)
{

	/* Configure DMAMUX */
	DMAMUX_Init(DMAMUX0);
	DMAMUX_SetSource(DMAMUX0, DMA_CHANNEL, DMA_SOURCE);
	DMAMUX_EnableChannel(DMAMUX0, DMA_CHANNEL);
	/* Configure DMA one shot transfer */
	DMA_Init(DMA0);
	DMA_CreateHandle(&g_DMA_Handle, DMA0, DMA_CHANNEL);
	DMA_SetCallback(&g_DMA_Handle, DMA_Callback, NULL);
}
