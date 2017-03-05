/*
 * ParallelPort.cpp
 *
 *  Created on: Mar 4, 2017
 *      Author: mattG
 */

#include "ParallelPort.hpp"

uint8_t ParallelPort::pinArray[16] =
	{9,10,12,14,15,19,21,23,25,28,30,31,33,35,36,37};

void ParallelPort::initParallel()
{
	Pins[9].function(PIN_9_GPIO); //bit 1
	Pins[10].function(PIN_10_GPIO);
	Pins[12].function(PIN_12_GPIO);
	Pins[14].function(PIN_14_GPIO);//bit 4
	Pins[15].function(PIN_15_GPIO);
	Pins[19].function(PIN_19_GPIO);
	Pins[21].function(PIN_21_GPIO);//bit 7
	Pins[23].function(PIN_23_GPIO);
	Pins[25].function(PIN_25_GPIO);
	Pins[28].function(PIN_28_GPIO);//bit 10
	Pins[30].function(PIN_30_GPIO);
	Pins[31].function(PIN_31_GPIO);
	Pins[33].function(PIN_33_GPIO);//bit 13
	Pins[35].function(PIN_35_GPIO);
	Pins[36].function(PIN_36_GPIO);
	Pins[37].function(PIN_37_GPIO);//bit 16
	//Pins[38].function(PIN_38_GPIO);
	Pins[49].function(PIN_49_IRQ3);

}

void ParallelPort::writeBits(uint16_t data)
{
	uint16_t andVar = 0x0001;
	for(int i = 0; i < 16; i++)
	{
		Pins[pinArray[i]].set((data & andVar) != 0);
		andVar = andVar << 1;
	}
}
