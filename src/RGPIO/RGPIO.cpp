/*
 * RGPIO.cpp
 *
 *  Created on: May 30, 2015
 *      Author: Matt Gramlich
 */
#include "RGPIO.h"

volatile PWORD RGPIO::pRGPIO_BAR = (PWORD) RGPIO_BAR;
BYTE RGPIO::val0 = FALSE;
BYTE RGPIO::val1 = FALSE;
BYTE RGPIO::val2 = FALSE;
BYTE RGPIO::val3 = FALSE;

void RGPIO::SetupRGPIO()
{
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

	pRGPIO_BAR[RGPIO_DIR] = RGPIO_1 | RGPIO_2 | RGPIO_3 | RGPIO_4;    // Set RGPIO to be an output
   	pRGPIO_BAR[RGPIO_ENB] = RGPIO_1 | RGPIO_2 | RGPIO_3 | RGPIO_4;    // Enable RGPIO pin

    // Set DSPI0 & One-Wire Slew Rate Control Register (SRCR_DSPIOW) to their maximum freq
   	sim1.gpio.srcr_dspiow = 0x33;

}

/*
The MIT License (MIT)

Copyright (c) 2015 Matthew Gramlich

Permission is hereby granted, free of charge, 
to any person obtaining a copy of this software 
and associated documentation files (the "Software"), 
to deal in the Software without restriction, 
including without limitation the rights to use, 
copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit 
persons to whom the Software is furnished to do so, 
subject to the following conditions:

The above copyright notice and this permission
notice shall be included in all copies or substantial
portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT 
WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE 
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, 
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

