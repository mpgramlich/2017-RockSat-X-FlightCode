#include <predef.h>
#include <stdio.h>
#include <ctype.h>
#include <startnet.h>
#include <autoupdate.h>
#include <smarttrap.h>

#include "Definitions.h"

#include "src/comm/Parallel/ParallelPort.hpp"

HiResTimer* timer = HiResTimer::getHiResTimer(2);
HiResTimer* throttle = HiResTimer::getHiResTimer(3);

extern "C" {
void UserMain(void * pd);
}

const char * AppName="2017-Rocksat-X-Flightcode";

uint16_t parallelPortData = 0x1;
void parallelPort_INT()
{
	parallelPortData=(parallelPortData==0x8000)?1:parallelPortData<<1;

	ParallelPort::writeBits(parallelPortData);
	//parallelPortData = ~parallelPortData;
}


void UserMain(void * pd) {
    InitializeStack();
    OSChangePrio(MAIN_TASK_PRIO);
    EnableAutoUpdate();

    #ifndef _DEBUG
    EnableSmartTraps();
    #endif

    ParallelPort::initParallel();
    ParallelPort::writeBits(0);


    //iprintf("Application started\n");
    while (1) {
        OSTimeDly(10*TICKS_PER_SECOND);
    }
}
