#include <predef.h>
#include <stdio.h>
#include <ctype.h>
#include <startnet.h>
#include <autoupdate.h>
#include <smarttrap.h>
#include <string.h>

#include <NetworkDebug.h>

#include "Definitions.h"

#include "src/comm/Parallel/ParallelPort.hpp"
#include "src/comm/serial/Serial_IO.h"
#include "src/comm/serial/SD_IO.h"

#include <iosys.h>

HiResTimer* timer = HiResTimer::getHiResTimer(2);
HiResTimer* throttle = HiResTimer::getHiResTimer(3);

extern "C" {
void UserMain(void * pd);
}

const char * AppName="2017-Rocksat-X-Flightcode";

void UserMain(void * pd) {
    InitializeStack();
    OSChangePrio(MAIN_TASK_PRIO);
    EnableAutoUpdate();

    #ifndef _DEBUG
    EnableSmartTraps();
    #endif
	#ifdef _DEBUG
	//InitializeNetworkGDB();
	InitializeNetworkGDB_and_Wait();
	#endif

    //iprintf("Testing0\r\n");

    Serial_IO::initSerial();
    ParallelPort::initParallel();

    ParallelPort::StartParallelQueueTask();
    SD_IO::StartSDWriteTask();
    Serial_IO::StartSerialReadTask();


    //iprintf("Testing1\r\n");

    DEBUG_PRINT_NET("Waiting for IO... \r\n");


//	while(1)
//	{
//		if(amountRead>=50)
//		{
//			parallelPortData = (uint8_t*)readData;
//			writeData = 1;
//			DEBUG_PRINT_NET("Waiting on Parallel Port Write... %2f\r\n", timer->readTime());
//			OSSemPend(&finishedWriting, 0);
//			DEBUG_PRINT_NET("Parallel Port Write finished... %2f\r\n", timer->readTime());
//			writeData=0;
//			amountRead=0;
//		}
//	}

    //iprintf("Application started\n");
    while (1) {
        OSTimeDly(10*TICKS_PER_SECOND);
        DEBUG_PRINT_NET("Testing... %u\r\n",ParallelPort::intCount);
    }
}
