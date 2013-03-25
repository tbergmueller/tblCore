/*
 ============================================================================
 Name        : tbl_core.c
 Author      : Thomas Bergmueller
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "m3s/m3s.h"
#include "m3s/m3s_util.h"


#include "DevCom.h"
#include "utils/DevComUtils.h"


void recErrDetected(const unsigned char* rReceiveBuffer, unsigned char length)
{
	printf("Found Receive Error with %d bytes with Addr in Frame %x\n", length, rReceiveBuffer[1]);
}

int main(void)
{

	unsigned char slaveAddr = 1;

	DevComMaster_t* master = dcm_create(1);

	master->UART = "/dev/ttyUSB0";
	master->ReportOnReceiveError = recErrDetected;


	dcm_start(master, 0, 500000);

	master->SendResetSequence(NULL);


	if(master->Ping(slaveAddr, NULL, NULL))
	{
		printf("pinged\n");
	}
	else
	{
		printf("failed to ping\n");
	}


	DevComSlaveInformation_t slaveInfo;
	unsigned char err;

	if(master->RequestSlaveInformation(slaveAddr, &slaveInfo, &err))
	{
		char buf[300];
		dc_slaveInfoToString(&slaveInfo,buf);
		printf("SlaveInfo: %s\n", buf);
	}
	else
	{
		printf("failed to read slave info\n");
	}

	DevComPayload_t unsupportedCmd;

	unsupportedCmd.Length = 1;
	unsupportedCmd.Data = (unsigned char*)"x";


	if(master->SendCommand(slaveAddr, &unsupportedCmd, 1, &err))
	{
		printf("Update Command Sent\n");
	}
	else
	{
		char test[300];
		dc_errCodeToString(err, test);
		printf("%s\n", test);
	}



	unsigned char data[32];

	int cnt;

	for(cnt=0; cnt<sizeof(data); cnt++)
	{
		data[cnt] = 0;
	}

	//data[1] = 0xff;

	DevComPayload_t nulldaten;
				nulldaten.Length = sizeof(data);
				nulldaten.Data = data;

	int i;
	for(i=1; i<=22; i++)
	{

		if(i != 0)
			data[i-2] = 0x00;

		if(i == 11 || i == 22)
		{
			nulldaten.Data[0] = i/10;
			nulldaten.Length = 16;
		}
		else
		{
			data[i-1] = 0xff;
			nulldaten.Length = 32;
		}


		if(master->SendData(i, &nulldaten, DC_ACK_REQUIRED, NULL))
		{
			printf("Data Sent to %d\n",i);
		}
		else
		{
			printf("DATA SEND FAILURE to %d\n", i);
		}




	}

	DevComPayload_t payload;

	payload.Length = 1;
	payload.Data = (unsigned char*)"U";

	if(master->SendCommandBroadcast(DC_BROADCAST, &payload, NULL))
	{
		printf("Update Command Sent\n");
	}
	else
	{
		printf("UPDATE COMMAND FAILURE\n");
	}

	//int dimmCnt = 0;
	/*while(1)
	{
		unsigned char dimmDat[] = {'D', (dimmCnt%256)};
		payload.Length = 2;
		payload.Data = dimmDat;


		if(master->SendCommandBroadcast(DC_BROADCAST, &payload, NULL))
		{
			printf("Dimm Command Sent\n");
		}
		else
		{
			printf("Dimm COMMAND FAILURE\n");
		}

		dimmCnt++;

		usleep(10000);
	}*/

	dcm_stop(master);

	printf("Operation of master stopped\n");

	return EXIT_SUCCESS;
}
