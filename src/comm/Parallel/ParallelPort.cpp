/*
 * ParallelPort.cpp
 *
 *  Created on: Mar 4, 2017
 *      Author: mattG
 */

#include "ParallelPort.hpp"
#include <ucos.h>
#include <cfinter.h>
void * ParallelPort::queueData[QUEUE_SIZE];
OS_Q ParallelPort::parallelQueue;

uint8_t ParallelPort::pinArray[16] =
	{9,10,12,14,15,19,21,23,25,28,30,31,33,35,36,37};

//19 data   RGPIO4
//21 clear  RGPIO3
//23 clock  RGPIO2
//25 data   RGPIO1
static volatile PWORD pRGPIO_BAR = (PWORD) RGPIO_BAR;

void ParallelPort::initParallel()
{
//	Pins[9].function(PIN_9_GPIO); //bit 1
//	Pins[10].function(PIN_10_GPIO);
//	Pins[12].function(PIN_12_GPIO);
//	Pins[14].function(PIN_14_GPIO);//bit 4
//	Pins[15].function(PIN_15_GPIO);
//	Pins[19].function(PIN_19_GPIO);
//	Pins[21].function(PIN_21_GPIO);//bit 7
//	Pins[23].function(PIN_23_GPIO);
//	Pins[25].function(PIN_25_GPIO);
//	Pins[28].function(PIN_28_GPIO);//bit 10
//	Pins[30].function(PIN_30_GPIO);
//	Pins[31].function(PIN_31_GPIO);
//	Pins[33].function(PIN_33_GPIO);//bit 13
//	Pins[35].function(PIN_35_GPIO);
//	Pins[36].function(PIN_36_GPIO);
//	Pins[37].function(PIN_37_GPIO);//bit 16
//	//Pins[38].function(PIN_38_GPIO);
//	Pins[49].function(PIN_49_IRQ3);

	Pins[19].function(PIN_19_GPIO);
	Pins[19] = 0;
	Pins[21].function(PIN_21_GPIO);
	Pins[21] = 0;
	Pins[23].function(PIN_23_GPIO);
	Pins[23] = 0;
	Pins[25].function(PIN_25_GPIO);
	Pins[25] = 0;

	// Enable processor access to the RGPIO module
	asm(" move.l #0x8C000035,%d0");  // All bits fixed except bit 0, set to 1 to enable
	asm(" movec %d0,#0x009");        // Use movec to write to control register

	pRGPIO_BAR[RGPIO_DIR] = pRGPIO_BAR[RGPIO_DIR] | SHIFT_LSB | SHIFT_CLEAR | SHIFT_CLOCK | SHIFT_MSB;    // Set RGPIO to be an output
	pRGPIO_BAR[RGPIO_ENB] = pRGPIO_BAR[RGPIO_ENB] | SHIFT_LSB | SHIFT_CLEAR | SHIFT_CLOCK | SHIFT_MSB;    // Enable RGPIO pin

	// Set DSPI0 & One-Wire Slew Rate Control Register (SRCR_DSPIOW) to their maximum freq
	sim1.gpio.srcr_dspiow = 0x33;
	pRGPIO_BAR[RGPIO_SET] = SHIFT_CLOCK | SHIFT_CLEAR;
	asm volatile("nop");
	pRGPIO_BAR[RGPIO_CLR] = ~SHIFT_CLOCK & ~SHIFT_LSB & ~SHIFT_MSB;

//	SetPinIrq( 49, 1, &ParallelPort::parallelPort_INT);  // IRQ 3

}

/**
 * the LSBit of both bytes is shifted out first. the lowest pin o
 */
void ParallelPort::writeBits(uint16_t* data)
{

	/*
	 init:
	 clock low;
	 data0 low;
	 data1 low;
	 clear low;

	 shift data:

	 clear high;

	 clock high;
	 shift MSB;
	 clock low;

	 clock high;
	 shift MSB-1;
	 clock low;

	 clock high;
	 shift MSB-2;
	 clock low;

	 clock high;
	 shift MSB-3;
	 clock low;

	 clock high;
	 shift MSB-4;
	 clock low;

	 clock high;
	 shift MSB-15;
	 clock low;

	 clear low;
	 */

	uint16_t andVar = 0x8080;

	pRGPIO_BAR[RGPIO_CLR] = ~SHIFT_CLOCK & ~SHIFT_LSB & ~SHIFT_MSB & ~SHIFT_CLEAR;
	asm volatile("nop");asm volatile("nop");asm volatile("nop");
	pRGPIO_BAR[RGPIO_SET] = SHIFT_CLEAR;
	for(int i=0; i < 8; i++)
	{
		pRGPIO_BAR[RGPIO_SET] = (SHIFT_MSB * ((*data & andVar) > 0x00ff)) | (SHIFT_LSB * (((*data & andVar) & 0x00ff)!=0));
		asm volatile("nop");asm volatile("nop");asm volatile("nop");
		pRGPIO_BAR[RGPIO_SET] = SHIFT_CLOCK;
		asm volatile("nop");asm volatile("nop");asm volatile("nop");
		pRGPIO_BAR[RGPIO_CLR] = ~SHIFT_CLOCK & ~SHIFT_LSB & ~SHIFT_MSB;
		andVar=(andVar>>1);
	}
	asm volatile("nop");asm volatile("nop");asm volatile("nop");
	pRGPIO_BAR[RGPIO_SET] = SHIFT_CLOCK;
	asm volatile("nop");asm volatile("nop");asm volatile("nop");
	pRGPIO_BAR[RGPIO_CLR] = ~SHIFT_CLOCK & ~SHIFT_LSB & ~SHIFT_MSB;


//	//uint16_t andVar = 0x8080;
//
//	pRGPIO_BAR[RGPIO_CLR] = ~SHIFT_CLOCK & ~SHIFT_LSB & ~SHIFT_MSB & ~SHIFT_CLEAR;
//	asm volatile("nop");asm volatile("nop");asm volatile("nop");
//	pRGPIO_BAR[RGPIO_SET] = SHIFT_CLEAR;
//
//	pRGPIO_BAR[RGPIO_SET] = (((*data & 0x8000)!=0)*SHIFT_MSB)  | (((*data & 0x0080)!=0)*SHIFT_LSB);
//	asm volatile("nop");asm volatile("nop");asm volatile("nop");
//	pRGPIO_BAR[RGPIO_SET] = SHIFT_CLOCK;
//	asm volatile("nop");asm volatile("nop");asm volatile("nop");
//
//	pRGPIO_BAR[RGPIO_CLR] = ~SHIFT_CLOCK;
//	pRGPIO_BAR[RGPIO_SET] = (((*data & 0x4000)!=0)*SHIFT_MSB)  | (((*data & 0x0040)!=0)*SHIFT_LSB);
//	asm volatile("nop");asm volatile("nop");asm volatile("nop");
//	pRGPIO_BAR[RGPIO_SET] = SHIFT_CLOCK;
//	asm volatile("nop");asm volatile("nop");asm volatile("nop");
//
//	pRGPIO_BAR[RGPIO_CLR] = ~SHIFT_CLOCK;
//	pRGPIO_BAR[RGPIO_SET] = (((*data & 0x2000)!=0)*SHIFT_MSB)  | (((*data & 0x0020)!=0)*SHIFT_LSB);
//	asm volatile("nop");asm volatile("nop");asm volatile("nop");
//	pRGPIO_BAR[RGPIO_SET] = SHIFT_CLOCK;
//	asm volatile("nop");asm volatile("nop");asm volatile("nop");
//
//	pRGPIO_BAR[RGPIO_CLR] = ~SHIFT_CLOCK;
//	pRGPIO_BAR[RGPIO_SET] = (((*data & 0x1000)!=0)*SHIFT_MSB)  | (((*data & 0x0010)!=0)*SHIFT_LSB);
//	asm volatile("nop");asm volatile("nop");asm volatile("nop");
//	pRGPIO_BAR[RGPIO_SET] = SHIFT_CLOCK;
//	asm volatile("nop");asm volatile("nop");asm volatile("nop");
//
//	pRGPIO_BAR[RGPIO_CLR] = ~SHIFT_CLOCK;
//	pRGPIO_BAR[RGPIO_SET] = (((*data & 0x0800)!=0)*SHIFT_MSB)  | (((*data & 0x0008)!=0)*SHIFT_LSB);
//	asm volatile("nop");asm volatile("nop");asm volatile("nop");
//	pRGPIO_BAR[RGPIO_SET] = SHIFT_CLOCK;
//	asm volatile("nop");asm volatile("nop");asm volatile("nop");
//
//	pRGPIO_BAR[RGPIO_CLR] = ~SHIFT_CLOCK;
//	pRGPIO_BAR[RGPIO_SET] = (((*data & 0x0400)!=0)*SHIFT_MSB)  | (((*data & 0x0004)!=0)*SHIFT_LSB);
//	asm volatile("nop");asm volatile("nop");asm volatile("nop");
//	pRGPIO_BAR[RGPIO_SET] = SHIFT_CLOCK;
//	asm volatile("nop");asm volatile("nop");asm volatile("nop");
//
//	pRGPIO_BAR[RGPIO_CLR] = ~SHIFT_CLOCK;
//	pRGPIO_BAR[RGPIO_SET] = (((*data & 0x0200)!=0)*SHIFT_MSB)  | (((*data & 0x0002)!=0)*SHIFT_LSB);
//	asm volatile("nop");asm volatile("nop");asm volatile("nop");
//	pRGPIO_BAR[RGPIO_SET] = SHIFT_CLOCK;
//	asm volatile("nop");asm volatile("nop");asm volatile("nop");
//
//	pRGPIO_BAR[RGPIO_CLR] = ~SHIFT_CLOCK;
//	pRGPIO_BAR[RGPIO_SET] = (((*data & 0x0100)!=0)*SHIFT_MSB)  | (((*data & 0x0001)!=0)*SHIFT_LSB);
//	asm volatile("nop");asm volatile("nop");asm volatile("nop");
//	pRGPIO_BAR[RGPIO_SET] = SHIFT_CLOCK;
//	asm volatile("nop");asm volatile("nop");asm volatile("nop");
//
//	pRGPIO_BAR[RGPIO_CLR] = ~SHIFT_CLOCK & ~SHIFT_LSB & ~SHIFT_MSB & ~SHIFT_CLEAR;
//	//USER_ENTER_CRITICAL();
//	uint16_t andVar = 0x8080;
//	int i = 0;
//	for(; i < 8; i++)
//	{
//		pRGPIO_BAR[RGPIO_CLR] = ~SHIFT_CLOCK & ~SHIFT_LSB & ~SHIFT_MSB;
//		asm volatile("nop");asm volatile("nop");asm volatile("nop");
//		pRGPIO_BAR[RGPIO_SET] = SHIFT_CLOCK;
//		asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");asm volatile("nop");
//	}
//	pRGPIO_BAR[RGPIO_CLR]= ~SHIFT_CLEAR;
//	asm volatile("nop");asm volatile("nop");asm volatile("nop");
//	pRGPIO_BAR[RGPIO_SET] = SHIFT_CLOCK;
//	asm volatile("nop");asm volatile("nop");asm volatile("nop");
//	pRGPIO_BAR[RGPIO_CLR] = ~SHIFT_CLOCK & ~SHIFT_LSB & ~SHIFT_MSB;asm volatile("nop");asm volatile("nop");asm volatile("nop");
//	pRGPIO_BAR[RGPIO_SET] = SHIFT_CLOCK;
//	asm volatile("nop");asm volatile("nop");asm volatile("nop");
//	pRGPIO_BAR[RGPIO_CLR] = ~SHIFT_CLOCK & ~SHIFT_LSB & ~SHIFT_MSB;
//	asm volatile("nop");asm volatile("nop");asm volatile("nop");
//	pRGPIO_BAR[RGPIO_SET]=  SHIFT_CLEAR;
//	asm volatile("nop");asm volatile("nop");asm volatile("nop");
//	pRGPIO_BAR[RGPIO_SET] = SHIFT_CLOCK;
//	asm volatile("nop");asm volatile("nop");asm volatile("nop");
//	pRGPIO_BAR[RGPIO_CLR] = ~SHIFT_CLOCK & ~SHIFT_LSB & ~SHIFT_MSB;
//	asm volatile("nop");asm volatile("nop");asm volatile("nop");
//	for(i=0; i < 8; i++)
//	{
//		pRGPIO_BAR[RGPIO_CLR] = ~SHIFT_CLOCK & ~SHIFT_LSB & ~SHIFT_MSB;
//		pRGPIO_BAR[RGPIO_SET] = (SHIFT_MSB * ((*data & andVar) > 0x00ff)) | (SHIFT_LSB * (((*data & andVar) & 0x00ff)!=0));
//		asm volatile("nop");asm volatile("nop");asm volatile("nop");
//		pRGPIO_BAR[RGPIO_SET] = SHIFT_CLOCK;
//		asm volatile("nop");asm volatile("nop");asm volatile("nop");
//		andVar=(andVar>>1);
//	}
//	asm volatile("nop");asm volatile("nop");asm volatile("nop");
//	pRGPIO_BAR[RGPIO_SET] = SHIFT_CLOCK;
//	asm volatile("nop");asm volatile("nop");asm volatile("nop");
//	pRGPIO_BAR[RGPIO_CLR] = ~SHIFT_CLOCK & ~SHIFT_LSB & ~SHIFT_MSB;
//	asm volatile("nop");asm volatile("nop");asm volatile("nop");
////	pRGPIO_BAR[RGPIO_CLR]= ~SHIFT_CLEAR;
//	//USER_EXIT_CRITICAL();
}



struct __attribute__((packed)) noDataMsg_t
{
	uint32_t H1;
	uint16_t  type;
	tick_t	 systemTick;
	uint32_t F1;
};

noDataMsg_t noDataMsg;
mail::mail_t noDataPackage;

volatile mail::mail_t* datamsg=0;
static uint16_t parallelZero=0;

OS_SEM parallelPortFinished;
volatile bool portBusy = false;
volatile bool dataAvail = false;
volatile uint32_t parallelAmountWritten=0;
volatile uint32_t ParallelPort::intCount=0;

//INTERRUPT(parallelPort_INT, 0x2000)
void parallelPort_INT()
{
	ParallelPort::intCount++;
	//outbyte(40);outbyte(40);outbyte(40);
//	if(dataAvail)
//	{
//		outbyte(49);
//	}
	//outbyte(0x0A);
	//outbyte(0x0A);
	if(dataAvail && datamsg && (parallelAmountWritten < datamsg->length))
	{
		//outbyte(41);outbyte(41);outbyte(41);outbyte(0x0A);
		portBusy=true;
		if(datamsg->length - parallelAmountWritten == 1)
		{
			uint16_t paddedData = ((uint16_t)datamsg->data[parallelAmountWritten])<<8;
			ParallelPort::writeBits(&paddedData);
			parallelAmountWritten++;
		}
		else
		{
			ParallelPort::writeBits((uint16_t*)(&datamsg->data[parallelAmountWritten]));
			parallelAmountWritten+=2;
		}
	}
	else
	{
		//outbyte(42);outbyte(42);outbyte(42);outbyte(0x0A);
		ParallelPort::writeBits(&parallelZero);
		if(!noDataPackage.inUse)
		{
			noDataPackage.inUse=TRUE;
			noDataMsg.systemTick=TimeTick;
			//outbyte(44);outbyte(44);outbyte(44);outbyte(0x0A);
			ParallelPort::postToQueue(&noDataPackage);
			//outbyte(42);outbyte(42);outbyte(42);outbyte(0x0A);
		}
	}
	if(dataAvail && portBusy && datamsg && parallelAmountWritten >= datamsg->length)
	{
		//outbyte(46);outbyte(46);outbyte(46);outbyte(0x0A);
		OSSemPost(&parallelPortFinished);
		parallelAmountWritten = 0;
		portBusy = false;
		dataAvail = false;
	}
}

void ParallelPort::ParallelQueueTask(void * pd)
{
	DEBUG_PRINT_NET("Started Parallel Write Task\r\n");
	BYTE err=0;

	while(true)
	{
		datamsg = (mail::mail_t*)OSQPend(&parallelQueue, 0, &err);
		if(portBusy)
		{
			//DEBUG_PRINT_NET("Waiting for Parallel Finish 1 dataavail=%d\r\n",dataAvail);
			OSSemPend(&parallelPortFinished, 0);
			portBusy=false;
		}
		if(err == OS_NO_ERR || err == OS_Q_FULL)
		{
			dataAvail=true;
			//DEBUG_PRINT_NET("Waiting for Parallel Finish 2 dataavail=%d\r\n",dataAvail);
			OSSemPend(&parallelPortFinished, 0);
			datamsg->inUse = FALSE;
		}
		err = 0;
	}
}

int ParallelPort::StartParallelQueueTask()
{
	int ret = 0;

	noDataMsg.H1=MSG_HEADER;
	noDataMsg.type=EMPTY_SERIAL_TYPE;
	noDataMsg.systemTick=TimeTick;
	noDataMsg.F1=MSG_FOOTER;

	noDataPackage.data=(char*) &noDataMsg;
	noDataPackage.inUse=FALSE;
	noDataPackage.length=sizeof(noDataMsg_t);
	noDataPackage.pktID=1;
	noDataPackage.type=0;
	noDataPackage.serialfd=0;

	//RGPIO::SetupRGPIO();

	OSSemInit(&parallelPortFinished,0);
	ret = OSQInit(&parallelQueue, queueData, QUEUE_SIZE);
	if(ret != OS_NO_ERR)
	{
		return ret;
	}
	OSSimpleTaskCreatewName(ParallelPort::ParallelQueueTask, PARALLEL_QUEUE_TASK_PRIO, "Parallel Task");

	OSTimeDly(20);

	Pins[50].function(PIN_50_IRQ2);
	SetPinIrq( 50, -1, parallelPort_INT);  // IRQ 2

	return 0;
}
