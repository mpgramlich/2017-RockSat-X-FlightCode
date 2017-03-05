#include <predef.h>
#include <stdio.h>
#include <ctype.h>
#include <startnet.h>
#include <autoupdate.h>
#include <smarttrap.h>


extern "C" {
void UserMain(void * pd);
}

const char * AppName="2017-Rocksat-X-Flightcode";

void UserMain(void * pd) {
    InitializeStack();
    OSChangePrio(MAIN_PRIO);
    EnableAutoUpdate();

    #ifndef _DEBUG
    EnableSmartTraps();
    #endif


    iprintf("Application started\n");
    while (1) {
        OSTimeDly(TICKS_PER_SECOND);
    }
}
