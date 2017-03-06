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

#ifndef SERIAL_IO_H_
#define SERIAL_IO_H_

#include "../../../Definitions.h"

#include <iosys.h>
#include <serial.h>
#include <pins.h>
#include <stdarg.h>
#include "../msgs/Mailbox-msg.h"

#ifdef DEBUG_SERIAL_IO__
#include <stdio.h>
#endif

#define QUEUE_SIZE 255
#define UART0 0
#define UART1 1
#define UART2 2
#define UART4 3
#define UART5 4
#define UART6 5
#define UART8 6
#define UART9 7

namespace Serial_IO
{
	extern void * queueData[QUEUE_SIZE];
	extern OS_Q SerialQueue;
	extern int serialFd[9];

	int SerialRxFlush(int fd);
	void SerialWriteTask(void * pd);
	int StartSerialWriteTask();

	inline BYTE postToQueue(void * msg)
	{
		return OSQPost( &SerialQueue, msg);
	}

	inline void initSerial()
	{
#ifdef DEBUG_SERIAL_IO__
		iprintf("Initializing Serial\n");
#endif
		SerialClose(0);
		SerialClose(1);
		SerialClose(2);
		SerialClose(4);
		SerialClose(5);
		SerialClose(6);
		SerialClose(8);
		SerialClose(9);

		Pins[26].function(PIN_26_UART0_TXD); //TX
		Pins[24].function(PIN_24_UART0_RXD); //RX
		serialFd[UART0] = OpenSerial(0, 115200, 1, 8, eParityNone);

		Pins[34].function(PIN_34_UART1_TXD); //TX
		Pins[32].function(PIN_32_UART1_RXD); //RX
		serialFd[UART1] = OpenSerial(1, SERIAL_1_BAUDRATE, 1, 8, eParityNone);

		Pins[16].function(PIN_16_UART2_TXD); //TX
		Pins[13].function(PIN_13_UART2_RXD); //RX
		serialFd[UART2] = OpenSerial(2, SERIAL_2_BAUDRATE, 1, 8, eParityNone);

		Pins[27].function(PIN_27_UART8_TXD); //TX
		Pins[29].function(PIN_29_UART8_RXD); //RX
		serialFd[UART8] = OpenSerial(8, SERIAL_8_BAUDRATE, 1, 8, eParityNone);

		Pins[22].function(PIN_22_UART9_TXD); //TX
		Pins[20].function(PIN_20_UART9_RXD); //RX
		serialFd[UART9] = OpenSerial(9, SERIAL_9_BAUDRATE, 1, 8, eParityNone);


#ifdef DEBUG_SERIAL_IO__
		iprintf("Leaving Serial Init\n");
#endif
	}

	inline int writePend(int* fileDesc, char* data, const int length)
	{
		//writeall returns length if succesful
		//or negative if error,
		//TODO do something on error?
		//printf("\nSerial 2 FD: %d\n", *fileDesc);
		return writeall(*fileDesc, data, length);
	}

	inline void debugPrintUart(int *fileDesc, const char *format, ...)
	{
#ifdef UART_DEBUG
		int rv;
		char printBuffer[255];
		va_list arg_ptr;
		va_start(arg_ptr, format);
		rv = vsnprintf(printBuffer, 255, format, arg_ptr);
		va_end(arg_ptr);
		if (rv)
		{
			writeall(*fileDesc, printBuffer, rv);

		}
#endif
	}

}

#endif /* SERIAL_IO_H_ */
