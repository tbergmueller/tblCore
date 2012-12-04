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

#include "m3s.h"
#include "m3s_util.h"
int main(void) {

	m3sFrame_t testFrame;
	unsigned char testData[]={(unsigned char)'R'};

	m3sCreateFrame(&testFrame, Reset, 0, 0, 0, 0, testData, sizeof(testData));

	//handler.CreateFrame(1, M3SProtocol.BroadCast, 1, data, true, false);

	unsigned char testdata2[]={1,2,3,4};
	m3sFrame_t testFrame2;

	m3sCreateFrame(&testFrame2,
			DataBroadcast,
			1,
			1,
			0,
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


	return EXIT_SUCCESS;
}
