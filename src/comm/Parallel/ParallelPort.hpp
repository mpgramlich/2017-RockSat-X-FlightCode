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

namespace ParallelPort
{

	void initParallel();
	void writeBits(uint16_t data);

	extern uint8_t pinArray[16];

}

#endif /* PARALLELPORT_HPP_ */
