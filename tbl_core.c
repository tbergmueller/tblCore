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

#include "m3s/m3s.h"
#include "m3s/m3s_util.h"

#include "DevCom.h"

int main(void)
{

	DevComMaster_t* master = dcm_create(1);

	master->UART = "/dev/ttyUSB0";

	dcm_start(master, 0, 500000);

	master->SendResetSequence(NULL);


	if(master->Ping(4, NULL, NULL))
	{
		printf("pinged\n");
	}
	else
	{
		printf("failed to ping\n");
	}


	DevComSlaveInformation_t slaveInfo;
	unsigned char err;

	if(master->ReadSlaveInformation(4, &slaveInfo, &err))
	{
		printf("Slave Info received\n");
	}
	else
	{
		printf("failed to read slave info\n");
	}



	return EXIT_SUCCESS;
}
