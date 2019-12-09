/*
 * dma_driver.c
 *
 *  Created on: 08-Dec-2019
 *      Author: prayag
 */

#include "dma_driver.h"
#include "PES_PROJECT_6.h"

dma_handle_t g_DMA_Handle;
volatile bool g_Transfer_Done = false;
dma_transfer_config_t transferConfig;

/* User callback function for DMA transfer. */
void DMA_Callback(dma_handle_t *handle, void *param)
{
	DMA_Initialize();
	g_Transfer_Done = true;
	xQueueReset(adcQueue);
	printf("DMA DONE\n");
	for(int i = 0; i < 64; i++){
		printf("%d\n", destDMA[i]);
	}
}


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
	//	DMA_PrepareTransfer(&transferConfig, srcAddr, sizeof(srcAddr[0]), destAddr, sizeof(destAddr[0]), sizeof(srcAddr),
	//			kDMA_MemoryToMemory);
}
