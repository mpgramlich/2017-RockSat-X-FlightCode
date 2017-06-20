/*
 * SD_IO.cpp
 *
 *  Created on: Mar 26, 2015
 *      Author: Matt Gramlich
 */

#include "SD_IO.h"
#include "../Parallel/ParallelPort.hpp"

#include "../spi/SDCard/FileSystemUtils.h"

OS_Q SD_IO::SDQueue;
void * SD_IO::queueData[QUEUE_SIZE];
volatile BYTE SD_IO::busy = FALSE;

void SD_IO::findAndMakeNextAvalDir()
{
	F_FIND finder;
	char tempFolderName[FN_MAXPATH];
	int highestFolderNum = -1;
	/* Find first file or subdirectory in specified directory. First call the
	      f_findfirst function, and if file was found get the next file with
	      f_findnext function. Files with the system attribute set will be ignored.
	      Note: If f_findfirst() is called with "*.*" and the current directory is
	      not the root directory, the first entry found will be "." - the current
	      directory.
	*/
	volatile int rc = f_findfirst( "*.*", &finder );
	if(rc==F_NO_ERROR)
	{
		do
		{
			if ((finder.attr & F_ATTR_DIR))
			{
				DEBUG_PRINT_NET( "Found Directory [%s]\r\n", finder.filename);

				if (finder.filename[0] != '.')
				{
					char* locOfUnderscore = strchr(finder.filename, '_');
					if(locOfUnderscore != 0)
					{
						int temp = strtol(&locOfUnderscore[1], 0, 10);
						if(temp > highestFolderNum)
						{
							highestFolderNum = temp;
							DEBUG_PRINT_NET("Highest Data Directory Suffix %d\r\n", highestFolderNum);
						}
					}
				}
				else
				{
					DEBUG_PRINT_NET("Changing to root Dir\r\n");
					f_chdir("/");
				}
			}
			else
			{
				DEBUG_PRINT_NET(
						"Found File [%s] : %d Bytes\r\n", finder.filename, finder.filesize);
			}
		} while (!f_findnext( &finder ));

		highestFolderNum++;
		sprintf(tempFolderName,"run_%d",highestFolderNum);
	}
	else
	{
		DEBUG_PRINT_NET("\r\nFolder scan failed, assuming new folder %d\r\n",rc);
		DisplayEffsErrorCode( rc );
		strcpy(tempFolderName, "run_1");
	}
	DEBUG_PRINT_NET("New Data Dir [%s]\r\n", tempFolderName);
	f_mkdir(tempFolderName);
	f_chdir(tempFolderName);


}

void SD_IO::SDWriteTask(void * pd)
{
	DEBUG_PRINT_NET("SD Write Task Started\r\n");
	f_enterFS();
	//InitExtFlash(F_MMC_DRIVE2);
	InitExtFlash(0);
	SD_IO::findAndMakeNextAvalDir();
	BYTE err;
	mail::mail_t* datamsg;
	int i = 0;

	while(true)
	{
		char name[12];
		datamsg = (mail::mail_t*)OSQPend(&SDQueue, 0, &err);
		if(err == OS_NO_ERR || err == OS_Q_FULL)
		{
			busy = TRUE;
			DEBUG_PRINT_NET("SD:: Received Package, writing data %d %2.6f\r\n",
											datamsg->pktID, timer->readTime());
//			if(datamsg->type == 1)
//			{
//				sprintf(name,"ADC%d.txt",datamsg->pktID);
//				DEBUG_PRINT_NET("Received ACC data");
//			}
//			else if(datamsg->type == 2)
//			{
//				sprintf(name,"DBG%d.txt",datamsg->pktID);
//				//DEBUG_PRINT_NET("Received Debug data");
//				datamsg->length = strlen(datamsg->data);
//			}
//			else
//			{
				sprintf(name,"Tele%d.txt",0);
//			}
			AppendFile(//reinterpret_cast<BYTE*>(NBADCTEST::letter[0].SDData),
					reinterpret_cast<BYTE*>(datamsg->data),
					name,
					datamsg->length
					);
			DEBUG_PRINT_NET("SD:: Finished writing data %d %2.6f\r\n",
											datamsg->pktID, timer->readTime());
//			if(datamsg->type==2)
//			{
//				datamsg->data[0] = 0;
//				datamsg->inUse = FALSE;
//			}
			busy = FALSE;
		}
//		datamsg->inUse = FALSE;
		ParallelPort::postToQueue(datamsg);
		err = 0;
		i++;
	}
	UnmountExtFlash();
	f_releaseFS();
}

int SD_IO::StartSDWriteTask()
{
	int ret = 0;
	ret = OSQInit(&SDQueue, queueData, QUEUE_SIZE);
	if(ret != OS_NO_ERR)
	{
		return ret;
	}
	OSSimpleTaskCreatewName(SD_IO::SDWriteTask, SD_WRITE_TASK_PRIO, "SD Write Task");
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
