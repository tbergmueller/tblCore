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
	printf("Found Receive Error with %d bytes\n", length);
}

int main(void)
{

	unsigned char slaveAddr = 4;

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

	data[1] = 0xff;

	DevComPayload_t nulldaten;
	nulldaten.Length = sizeof(data);
	nulldaten.Data = data;



	if(master->SendData(slaveAddr, &nulldaten, DC_ACK_REQUIRED, NULL))
	{
		printf("Data Sent\n");
	}
	else
	{
		printf("DATA SEND FAILURE\n");
	}

	DevComPayload_t payload;

	payload.Length = 1;
	payload.Data = (unsigned char*)"U";

	if(master->SendCommand(slaveAddr, &payload, DC_ACK_NO, NULL))
	{
		printf("Update Command Sent\n");
	}
	else
	{
		printf("UPDATE COMMAND FAILURE\n");
	}


	dcm_stop(master);

	printf("Operation of master stopped\n");

	return EXIT_SUCCESS;
}
