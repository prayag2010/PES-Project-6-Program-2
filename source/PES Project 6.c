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

double sinLUT[50] = {0};
#define M_PI 3.14159265358979323846


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

	PRINTF("Hello World\n");

	for(int i = 0; i < 50; i++){
		sinLUT[i] = sin((double)M_PI * (i / (double)49));
	}

	for(int i = 0; i < 50; i++){
		PRINTF("%0.2lf\n", sinLUT[i]);
	}

	while(1) {
		__asm volatile ("nop");
	}
	return 0 ;
}
