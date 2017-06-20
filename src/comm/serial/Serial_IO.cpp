/*
 * Serial_IO.cpp
 *
 *  Created on: Mar 26, 2015
 *      Author: Matt Gramlich
 */

#include "Serial_IO.h"

#include "../spi/SDCard/FileSystemUtils.h"

int Serial_IO::serialFd[8];
int Serial_IO::setFDs[8];
OS_Q Serial_IO::SerialQueue;
void * Serial_IO::queueData[QUEUE_SIZE];

Serial_IO::serialMsg_t Serial_IO::msg[NUM_OF_SERIAL_IN_BUFFERS];
mail::mail_t Serial_IO::package[NUM_OF_SERIAL_IN_BUFFERS];

int16_t currentBufNum[USED_FDS]={-1,-1,-1,-1};

void Serial_IO::initBuffers()
{
	for(int i = 0; i < NUM_OF_SERIAL_IN_BUFFERS; i++)
	{
		msg[i].serialMsg.H1=MSG_HEADER;
		msg[i].serialMsg.type=SERIAL_PACKAGE_TYPE;
		msg[i].serialMsg.dataBegin=DATA_BEGIN_HEADER;
		msg[i].serialMsg.dataLength=0;
		package[i].data=msg[i].serialDataChar;
		package[i].inUse=FALSE;
		package[i].length=0;
		package[i].pktID=0;
		package[i].serialfd=0;
		package[i].type=SERIAL_PACKAGE_TYPE;
	}
}

inline int Serial_IO::selectNextBuffer()
{
	for(uint16_t selectedBuffer = NUM_OF_SERIAL_IN_BUFFERS; selectedBuffer >= 0; selectedBuffer--)
	{
		if(package[selectedBuffer].inUse == FALSE)
		{
			package[selectedBuffer].inUse = 3;
			return selectedBuffer;
		}
	}
	return -1;
}

int Serial_IO::SerialRxFlush(int fd)
{
	//static char buf[2048];
	if (dataavail(fd))
		return read(fd, NULL, 2048);
	return 0;
}

void Serial_IO::SerialReadTask(void * pd)
{
	DEBUG_PRINT_NET("Started Serial Read Task\r\n");
	DEBUG_PRINT_NET("Initializing Buffers\r\n");

	initBuffers();

	DEBUG_PRINT_NET("Buffers Initialized\r\n");

	fd_set fullReadSet;
	FD_SET(Serial_IO::serialFd[Serial_IO::setFDs[0]], &fullReadSet);
	FD_SET(Serial_IO::serialFd[Serial_IO::setFDs[1]], &fullReadSet);
	FD_SET(Serial_IO::serialFd[Serial_IO::setFDs[2]], &fullReadSet);
	FD_SET(Serial_IO::serialFd[Serial_IO::setFDs[3]], &fullReadSet);
	fd_set readSet;
	int result = 0;

	for(int i = 0 ; i < USED_FDS; i++)
	{
		if(currentBufNum[i] < 0)
		{
			currentBufNum[i]=selectNextBuffer();
			DEBUG_PRINT_NET("Interface Num %d :: Selected Buffer %d\r\n", i, currentBufNum[i]);
			if(currentBufNum[i]>=0)
			{
				msg[currentBufNum[i]].serialMsg.interfaceNum=setFDs[i];
				msg[currentBufNum[i]].serialMsg.systemTick=TimeTick;
			}
		}
	}

	while(true)
	{
		memcpy(&readSet, &fullReadSet, sizeof(fd_set));
		result = select(4, &readSet, 0, 0, TICKS_PER_SECOND * SELECT_READ_TIMEOUT);
		DEBUG_PRINT_NET("Select Return Value %d\r\n", result);
		for(int i = 0 ; i < USED_FDS; i++)
		{
			if(result &&
				(currentBufNum[i]>=0) &&
				FD_ISSET(Serial_IO::serialFd[Serial_IO::setFDs[i]], &readSet))
			{
				DEBUG_PRINT_NET("FD index %d is Set!\r\n", i);

				msg[currentBufNum[i]].serialMsg.dataLength +=
						read(Serial_IO::serialFd[Serial_IO::setFDs[i]],
								(char*) &msg[currentBufNum[i]].serialMsg.data[msg[currentBufNum[i]].serialMsg.dataLength],
								SERIAL_DATA_PER_MSG-msg[currentBufNum[i]].serialMsg.dataLength);

				DEBUG_PRINT_NET("Amount read %u\r\n",msg[currentBufNum[i]].serialMsg.dataLength);
				//Serial_IO::writePend(&Serial_IO::serialFd[Serial_IO::setFDs[i]],readData, amountRead);
			}

			if(((TimeTick - msg[currentBufNum[i]].serialMsg.systemTick) > BUFFER_WRITE_TIMEOUT) &&
					(SERIAL_DATA_PER_MSG-msg[currentBufNum[i]].serialMsg.dataLength < 150))
			{
				DEBUG_PRINT_NET("Interface Num %d sending mail\r\n",Serial_IO::setFDs[i]);

				memcpy(&msg[currentBufNum[i]].serialData[msg[currentBufNum[i]].serialMsg.dataLength],&footer_val,4);
				msg[currentBufNum[i]].serialMsg.dataLength+=4;
				package[currentBufNum[i]].length=SERIAL_DATA_T_LENGTH_WITHOUT_DATA+msg[currentBufNum[i]].serialMsg.dataLength;
				SD_IO::postToQueue(&package[currentBufNum[i]]);

				currentBufNum[i]=selectNextBuffer();

				DEBUG_PRINT_NET("Interface Num %d :: Selected Buffer %d\r\n", Serial_IO::setFDs[i], currentBufNum[i]);

				if(currentBufNum[i]>=0)
				{
					msg[currentBufNum[i]].serialMsg.interfaceNum=setFDs[i];
					msg[currentBufNum[i]].serialMsg.systemTick=TimeTick;
				}
			}

		}
	}
}

void Serial_IO::SerialWriteTask(void * pd)
{
	DEBUG_PRINT_NET("Started Serial Write Task\r\n");
	BYTE err;
	mail::mail_t* datamsg;

	while(true)
	{
		datamsg = (mail::mail_t*)OSQPend(&SerialQueue, 0, &err);
		if(err == OS_NO_ERR || err == OS_Q_FULL)
		{
			if(datamsg->serialfd)
			{
				Serial_IO::writePend(datamsg->serialfd, datamsg->data, datamsg->length);
			}
			else
			{
				Serial_IO::writePend(&Serial_IO::serialFd[UART2], datamsg->data, datamsg->length);
			}
		}
		datamsg->inUse = FALSE;
		err = 0;
	}
}

int Serial_IO::StartSerialWriteTask()
{
	int ret = 0;
	ret = OSQInit(&SerialQueue, queueData, QUEUE_SIZE);
	if(ret != OS_NO_ERR)
	{
		return ret;
	}
	OSSimpleTaskCreatewName(Serial_IO::SerialWriteTask, SERIAL_WRITE_TASK_PRIO, "Serial Write Task");
	return 0;
}

int Serial_IO::StartSerialReadTask()
{
	OSSimpleTaskCreatewName(Serial_IO::SerialReadTask, SERIAL_READ_TASK_PRIO, "Serial Read Task");
	return 0;
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
