/*
 * m3s_util.c
 *
 *  Created on: Nov 12, 2012
 *      Author: tbergmueller
 */



#include "m3s_util.h"
#include <stdio.h>
#include <string.h>


void m3sGetProtStr(const m3sFrame_t* frame, char* rTarget)
{
	switch(frame->CtrlByte & 0xf0)
	{
		case Reset: sprintf(rTarget, "RES"); break;
		case Data: sprintf(rTarget, "DUC"); break;
		case DataBroadcast:
				if(frame->SlaveAddr)
					sprintf(rTarget, "DMC");
				else
					sprintf(rTarget, "DBC");
				break;
		case Command: sprintf(rTarget, "CUC"); break;
		case CommandBroadcast:
						if(frame->SlaveAddr)
							sprintf(rTarget, "CMC");
						else
							sprintf(rTarget, "CBC");
						break;
		case Response:
				sprintf(rTarget, "RSP");
				break;
		case Acknowledge: sprintf(rTarget, "ACK"); break;
		case FileTransfer: sprintf(rTarget, "FTU"); break;
		default: sprintf(rTarget, "UKW"); break;

	}
}

char* m3sFrameToString(const m3sFrame_t* frame, char* rTarget)
{
	char protBuf[4];
	m3sGetProtStr(frame,protBuf);
	sprintf(rTarget, "M3S-Frame: %s S=%02x M=%d UB=%02x CRC=%02x Data=", protBuf, frame->SlaveAddr, 0, frame->UpperBound+1, frame->CRC);


	char* ptr = (char*)(rTarget + strlen(rTarget));
	unsigned char cnt = 0;

	for(cnt = 0; cnt <= frame->UpperBound; cnt++)
	{
		sprintf(ptr," %02x", frame->Data[cnt]);
		ptr += (3);
	}

	return(rTarget);
}
