#include <predef.h>
#include <stdio.h>
#include <ctype.h>
#include <startnet.h>
#include <autoupdate.h>
#include <smarttrap.h>

#include "Definitions.h"

#include "src/comm/Parallel/ParallelPort.hpp"
#include "src/comm/serial/Serial_IO.h"

#include <iosys.h>

HiResTimer* timer = HiResTimer::getHiResTimer(2);
HiResTimer* throttle = HiResTimer::getHiResTimer(3);

extern "C" {
void UserMain(void * pd);
}

const char * AppName="2017-Rocksat-X-Flightcode";

int amountWritten = 0;
uint16_t zeroData=0;
int amountRead = 0;
char readData[500];
bool writeData = 0;
uint8_t* parallelPortData = 0;
OS_SEM finishedWriting;
void parallelPort_INT()
{
	if(writeData && parallelPortData && (amountWritten < amountRead))
	{
		if(!(amountWritten) && (amountRead%2))
		{
			uint16_t paddedData = parallelPortData[0];
			ParallelPort::writeBits(&paddedData);
			amountWritten++;
			return;
		}
		ParallelPort::writeBits((uint16_t*)(&parallelPortData[amountWritten]));
		amountWritten+=2;
	}
	else
	{

		if(writeData && amountWritten >= amountRead)
		{
			OSSemPost(&finishedWriting);
		}
		amountWritten = 0;
		//ParallelPort::writeBits(&zeroData);
	}
}


void UserMain(void * pd) {
    InitializeStack();
    OSChangePrio(MAIN_TASK_PRIO);
    EnableAutoUpdate();

    #ifndef _DEBUG
    EnableSmartTraps();
    #endif

    OSSemInit(&finishedWriting, 0);

    iprintf("Testing0\r\n");

    Serial_IO::initSerial();
    ParallelPort::initParallel();
    SetPinIrq( 49, 1, parallelPort_INT);  // IRQ 3
    ParallelPort::writeBits(&zeroData);
    iprintf("Testing1\r\n");

    DEBUG_PRINT_NET("Waiting for IO... \r\n");

while(1)
{
    fd_set readSet;
    FD_SET(Serial_IO::serialFd[Serial_IO::setFDs[0]], &readSet);
    int result = select(1, &readSet, 0, 0, TICKS_PER_SECOND * 10);
    DEBUG_PRINT_NET("Select Return Value %d\r\n", result);

    if(!writeData && (result > 0))
    {
    	for(int i = 0 ; i < USED_FDS; i++)
    	{
    		if(FD_ISSET(Serial_IO::serialFd[Serial_IO::setFDs[i]], &readSet))
    		{
    			DEBUG_PRINT_NET("FD index %d is Set!\r\n", i);
    			amountRead += read(Serial_IO::serialFd[Serial_IO::setFDs[i]],&readData[amountRead], 500-amountRead);
    			DEBUG_PRINT_NET("Amount read %d\r\n",amountRead);
    			//Serial_IO::writePend(&Serial_IO::serialFd[Serial_IO::setFDs[i]],readData, amountRead);
    		}
    	}
    }
    if(amountRead>=50)
    {
    	parallelPortData = (uint8_t*)readData;
    	writeData = 1;
    	DEBUG_PRINT_NET("Waiting on Parallel Port Write... %2f\r\n", timer->readTime());
    	OSSemPend(&finishedWriting, 0);
    	DEBUG_PRINT_NET("Parallel Port Write finished... %2f\r\n", timer->readTime());
    	writeData=0;
    	amountRead=0;
    }
}

    //iprintf("Application started\n");
    while (1) {
        OSTimeDly(10*TICKS_PER_SECOND);
    }
}
