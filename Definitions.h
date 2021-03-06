 /*
 * Definitions.h
 *
 *  Created on: May 29, 2015
 *      Author: Matt Gramlich
 *      			Greg Lusk
 */

#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

#include "predef.h"
#include <stdio.h>
#include <constants.h>
#include <ctype.h>
#include <startnet.h>
#include <autoupdate.h>
#include <smarttrap.h>
#include <taskmon.h>
#include <system.h>
#include <init.h>
#include <utils.h>
#include <stdlib.h>
#include <pins.h>
#include <pitr_sem.h>
#include <sim5441x.h>
#include <HiResTimer.h>

extern HiResTimer* throttle;
extern HiResTimer* timer;

//#define REDIRECT_NET_DEBUG_TO_STDOUT
//#include <SysLog.h> //comment to remove ethernet debugging statements

//RGPIO
	// Addresses of Rapid GPIO registers
	#define RGPIO_BAR 0x8C000000           // Base Address Register

	// Offsets to BAR when accessing registers as a 16-bit word array
	#define RGPIO_DIR (0)   // Data Dir Register
	#define RGPIO_DAT (1)   // Data Register
	#define RGPIO_ENB (2)   // Enable Register
	#define RGPIO_CLR (3)   // Clear
	#define RGPIO_SET (5)   // Set
	#define RGPIO_TOG (7)   // Toggle

	#define RGPIO_0 0x0001     // Pin 37  N11
	#define RGPIO_1 0x0002     // Pin 32  G13
	#define RGPIO_2 0x0004     // Pin 33  H14
	#define RGPIO_3 0x0008     // Pin 34  H13
	#define RGPIO_4 0x0010     // Pin 36  H15
	#define RGPIO_5 0x0020     // Pin 29  E13
	#define RGPIO_6 0x0040     // Pin 23  B11
	#define RGPIO_7 0x0080     // Pin 24  C10
	#define RGPIO_8 0x0100     // Pin 25  D10

//Serial Definitions
	#define SERIAL_1_BAUDRATE 57600 //marshall
	#define SERIAL_2_BAUDRATE 57600 //state
	#define SERIAL_8_BAUDRATE 9600   //pep
	#define SERIAL_9_BAUDRATE 57600

//Network Debugging
#ifdef REDIRECT_NET_DEBUG_TO_STDOUT
#define DEBUG_PRINT_NET(args...) iprintf(args)
#else
	#ifdef SYSLOG_H
	#define SYSLOGIP "192.168.11.99"
	#define DEBUG_PRINT_NET(args...) SysLog(args)
	#else
	#define DEBUG_PRINT_NET(args...)
	#endif
#endif

//Task Priorities

	#define PARALLEL_QUEUE_TASK_PRIO 48
	#define SD_WRITE_TASK_PRIO 49
	#define SERIAL_READ_TASK_PRIO 50
	#define SERIAL_WRITE_TASK_PRIO 51
	#define MAIN_TASK_PRIO 52


//Message Specific Definitions
	#define MSG_HEADER 				0xFF0A0BCC
	#define MSG_FOOTER 				0x1234DEAD
static __attribute__((unused)) uint32_t footer_val=MSG_FOOTER;
	#define PAYLOAD_ACT_MSG_HEADER	0xEE0F0ABB
	#define DATA_BEGIN_HEADER		0x4D47474C //'MGGL'
	#define DATA_END_FOOTER			0xFA
	#define IDLE 					0x00

#define NUM_OF_SERIAL_IN_BUFFERS 10000
#define SERIAL_DATA_PER_MSG 100
#define BUFFER_SIZE_DELTA 10
#define FOOTER_LENGTH 4
#define SERIAL_PACKAGE_TYPE 42
#define EMPTY_SERIAL_TYPE 52

/*
The MIT License (MIT)

Copyright (c) 2015 Matthew Gramlich
				   Greg Lusk

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

#endif /* DEFINITIONS_H_ */
