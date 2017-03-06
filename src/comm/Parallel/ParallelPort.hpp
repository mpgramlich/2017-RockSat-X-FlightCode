/*
 * ParallelPort.hpp
 *
 *  Created on: Mar 4, 2017
 *      Author: mattG
 */

#ifndef PARALLELPORT_HPP_
#define PARALLELPORT_HPP_

#include "../../../Definitions.h"
#include <pins.h>
#include <pinconstant.h>
#include <pin_irq.h>


#define SHIFT_LSB	 RGPIO_4 //pin 19
#define SHIFT_CLEAR  RGPIO_3 //pin 21
#define SHIFT_CLOCK  RGPIO_2 //pin 23
#define SHIFT_MSB	 RGPIO_1 //pin 25

namespace ParallelPort
{

	void initParallel();
	void writeBits(uint16_t* data);
	void parallelPort_INT();

	extern uint8_t pinArray[16];

}

#endif /* PARALLELPORT_HPP_ */
