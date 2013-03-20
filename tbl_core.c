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

#include "devComMaster.h"

int main(void) {

	m3sFrame_t testFrame;
	unsigned char testData[]={(unsigned char)'R'};

	m3sCreateFrame(&testFrame, Reset, 0, 0, 0, 1, testData, sizeof(testData));

	//handler.CreateFrame(1, M3SProtocol.BroadCast, 1, data, true, false);

	unsigned char testdata2[]={(unsigned char)'P'};
	m3sFrame_t testFrame2;

	m3sCreateFrame(&testFrame2,
			Command,
			1,
			1,
			1,
			1,
			testdata2,
			sizeof(testdata2));




	char test[200];

	printf("%s\n", m3sFrameToString(&testFrame, test));


	unsigned char* curByte = m3sStream(&testFrame2, M3S_STREAM_START);

	while(curByte != NULL)
	{
		printf("%02x ", *curByte);
		curByte = m3sStream(&testFrame2, M3S_STREAM_CONTINUE);
	}
	printf("\n");


	DevComMaster_t* master = dcm_create(1);

	dcm_start(master, 0, 38400);

	master->SendResetSequence();

	if(master->Ping(2) == 0)
	{
		printf("pinged\n");
	}
	else
	{
		printf("failed to ping\n");
	}


	while(1);

	return EXIT_SUCCESS;
}
