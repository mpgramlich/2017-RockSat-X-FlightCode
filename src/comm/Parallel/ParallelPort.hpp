/*
 * ParallelPort.hpp
 *
 *  Created on: Mar 4, 2017
 *      Author: mattG
 */

#ifndef PARALLELPORT_HPP_
#define PARALLELPORT_HPP_

#include "../../../Definitions.h"
#include "../msgs/Mailbox-msg.h"
#include "../../RGPIO/RGPIO.h"
#include <pins.h>
#include <pinconstant.h>
#include <pin_irq.h>


#define QUEUE_SIZE 255

#define SHIFT_LSB	 RGPIO_3 //pin 19
#define SHIFT_CLEAR  RGPIO_1 //pin 21
#define SHIFT_CLOCK  RGPIO_2 //pin 23
#define SHIFT_MSB	 RGPIO_4 //pin 25

namespace ParallelPort
{
	extern void * queueData[QUEUE_SIZE];
	extern OS_Q parallelQueue;
	extern uint8_t pinArray[16];
	extern volatile uint32_t intCount;

	inline BYTE postToQueue(mail::mail_t * msg)
	{
		return OSQPost( &parallelQueue, (void*)msg);
	}

	void ParallelQueueTask(void * pd);
	int  StartParallelQueueTask();
	void initParallel();
	void writeBits(uint16_t* data);

}

#endif /* PARALLELPORT_HPP_ */
