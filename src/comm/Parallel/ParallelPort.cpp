/*
 * ParallelPort.cpp
 *
 *  Created on: Mar 4, 2017
 *      Author: mattG
 */

#include "ParallelPort.hpp"

uint8_t ParallelPort::pinArray[16] =
	{9,10,12,14,15,19,21,23,25,28,30,31,33,35,36,37};

//19 data   RGPIO4
//21 clear  RGPIO3
//23 clock  RGPIO2
//25 data   RGPIO1
static volatile PWORD pRGPIO_BAR = (PWORD) RGPIO_BAR;

void ParallelPort::initParallel()
{
//	Pins[9].function(PIN_9_GPIO); //bit 1
//	Pins[10].function(PIN_10_GPIO);
//	Pins[12].function(PIN_12_GPIO);
//	Pins[14].function(PIN_14_GPIO);//bit 4
//	Pins[15].function(PIN_15_GPIO);
//	Pins[19].function(PIN_19_GPIO);
//	Pins[21].function(PIN_21_GPIO);//bit 7
//	Pins[23].function(PIN_23_GPIO);
//	Pins[25].function(PIN_25_GPIO);
//	Pins[28].function(PIN_28_GPIO);//bit 10
//	Pins[30].function(PIN_30_GPIO);
//	Pins[31].function(PIN_31_GPIO);
//	Pins[33].function(PIN_33_GPIO);//bit 13
//	Pins[35].function(PIN_35_GPIO);
//	Pins[36].function(PIN_36_GPIO);
//	Pins[37].function(PIN_37_GPIO);//bit 16
//	//Pins[38].function(PIN_38_GPIO);
//	Pins[49].function(PIN_49_IRQ3);

	Pins[19].function(PIN_19_GPIO);
	Pins[19] = 0;
	Pins[21].function(PIN_21_GPIO);
	Pins[21] = 0;
	Pins[23].function(PIN_23_GPIO);
	Pins[23] = 0;
	Pins[25].function(PIN_25_GPIO);
	Pins[25] = 0;

	// Enable processor access to the RGPIO module
	asm(" move.l #0x8C000035,%d0");  // All bits fixed except bit 0, set to 1 to enable
	asm(" movec %d0,#0x009");        // Use movec to write to control register

	pRGPIO_BAR[RGPIO_DIR] = pRGPIO_BAR[RGPIO_DIR] | SHIFT_LSB | SHIFT_CLEAR | SHIFT_CLOCK | SHIFT_MSB;    // Set RGPIO to be an output
	pRGPIO_BAR[RGPIO_ENB] = pRGPIO_BAR[RGPIO_ENB] | SHIFT_LSB | SHIFT_CLEAR | SHIFT_CLOCK | SHIFT_MSB;    // Enable RGPIO pin

	// Set DSPI0 & One-Wire Slew Rate Control Register (SRCR_DSPIOW) to their maximum freq
	sim1.gpio.srcr_dspiow = 0x33;
	pRGPIO_BAR[RGPIO_SET] = SHIFT_CLOCK | SHIFT_CLEAR;

//	SetPinIrq( 49, 1, &ParallelPort::parallelPort_INT);  // IRQ 3

}

/**
 * the LSBit of both bytes is shifted out first. the lowest pin o
 */
void ParallelPort::writeBits(uint16_t* data)
{
	USER_ENTER_CRITICAL();
	uint32_t andVar = 0x8080;
	int i = 0;
	pRGPIO_BAR[RGPIO_CLR]= ~SHIFT_CLEAR;
	asm volatile("nop");asm volatile("nop");asm volatile("nop");
	pRGPIO_BAR[RGPIO_SET]=  SHIFT_CLEAR;
	for(; i < 8; i++)
	{
		pRGPIO_BAR[RGPIO_CLR] = ~SHIFT_CLOCK & ~SHIFT_LSB & ~SHIFT_MSB;

		pRGPIO_BAR[RGPIO_SET] = (SHIFT_MSB * ((*data & andVar) > 0x00ff)) | (SHIFT_LSB * (((*data & andVar) & 0x00ff)!=0));

		andVar=(andVar>>1);
		asm volatile("nop");asm volatile("nop");asm volatile("nop");

		pRGPIO_BAR[RGPIO_SET] = SHIFT_CLOCK;
	}
	pRGPIO_BAR[RGPIO_CLR]= ~SHIFT_CLEAR;
	USER_EXIT_CRITICAL();
}

void ParallelPort::parallelPort_INT()
{

}
