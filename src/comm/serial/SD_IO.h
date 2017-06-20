/*
 * Serial_IO.h
 *
 *  Created on: Mar 26, 2015
 *      Author: Matt Gramlich
 */
 
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

#ifndef SD_IO_H_
#define SD_IO_H_

#include "../../../Definitions.h"

#include <iosys.h>
#include <serial.h>
#include <pins.h>
#include <stdarg.h>

#include <effs_fat/mmc_mcf.h>
#include "../msgs/Mailbox-msg.h"
#include "Serial_IO.h"

#ifdef DEBUG_SERIAL_IO__
#include <stdio.h>
#endif

#define QUEUE_SIZE 255

namespace SD_IO
{
	extern void * queueData[QUEUE_SIZE];
	extern OS_Q SDQueue;
	volatile extern BYTE busy;

	void SDWriteTask(void * pd);
	int StartSDWriteTask();

	void findAndMakeNextAvalDir();

	inline BYTE postToQueue(mail::mail_t * msg)
	{
		return OSQPost( &SDQueue, (void*)msg);
	}
}

#endif /* SD_IO_H_ */
