#include <predef.h>
#include <stdio.h>
#include <ctype.h>
#include <startnet.h>
#include <autoupdate.h>
#include <smarttrap.h>

#include <pin_irq.h>

#include "Definitions.h"

#include "src/comm/Parallel/ParallelPort.hpp"

HiResTimer* throttle = HiResTimer::getHiResTimer(3);

extern "C" {
void UserMain(void * pd);
}

const char * AppName="2017-Rocksat-X-Flightcode";

uint16_t parallelPortData = 0;
void driveParallelPort()
{
	ParallelPort::writeBits(parallelPortData);
	parallelPortData = ~parallelPortData;
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
    SetPinIrq( 49, 1, &driveParallelPort);  // IRQ 3

    //iprintf("Application started\n");
    while (1) {
        OSTimeDly(10*TICKS_PER_SECOND);
    }
}
