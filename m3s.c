/*
 * m3s.c
 *
 *  Created on: Nov 12, 2012
 *      Author: tbergmueller
 */

#include <stdlib.h>
#include "m3s.h"
#include "m3sV2_basicFunctions.h"
#include "crc8.h"

unsigned char calcCRC(const m3sFrame_t* pFrame)
{
	unsigned char curCrc = M3S_CRC_INITVAL; // startwert
	unsigned short cnt;

	curCrc = crc8(pFrame->CtrlByte, curCrc);
	curCrc = crc8(pFrame->SlaveAddr, curCrc);

	// If acknowledge, there is no more information
	if(extractProtocol(pFrame->CtrlByte) == M3S_CTRLBYTE_PROTOCOL_ACK_gc)
	{
		return(curCrc);
	}

	curCrc = crc8(pFrame->UpperBound, curCrc);

	for(cnt=0; cnt <= pFrame->UpperBound; cnt++)
	{
		curCrc = crc8(pFrame->Data[cnt], curCrc);
	}

	return(curCrc);
}


unsigned char m3sCreateFrame(
			m3sFrame_t* rTarget,
			unsigned char vProtocol,
			unsigned char vSlaveAddress,
			unsigned char vMasterAddress,
			unsigned char vAckOrAckRequest,
			unsigned char vMasterSend,
			unsigned char* rData,
			unsigned char vDataLength
			)
{

	rTarget->Data = rData;
	rTarget->UpperBound = vDataLength-1;
	rTarget->CtrlByte = vProtocol | ((vMasterAddress & 0x03) << M3S_CTRLBYTE_MASTERADDR_gp);
	rTarget->SlaveAddr = vSlaveAddress;

	if(vAckOrAckRequest)
	{
		rTarget->CtrlByte |= (1<<M3S_CTRLBYTE_ACK_OR_MC_bp);
	}

	if(!vMasterSend)
	{
		rTarget->CtrlByte |= (1<<M3S_CTRLBYTE_SS_bp);
	}

	rTarget->CRC = calcCRC(rTarget);


	return(0);
}

unsigned char* m3sStream(m3sFrame_t* rFrame, unsigned char reset)
{
		static unsigned short idxCnt;

		if(reset)
		{
			idxCnt = 0;
		}

		idxCnt++;

		if(idxCnt > rFrame->UpperBound+M3S_OVERHEAD_LENGTH+1)
		{
			idxCnt = 0;
			return(NULL);
		}

		if(idxCnt == M3S_OVERHEAD_LENGTH+rFrame->UpperBound+1)
		{
			return(&rFrame->CRC);
		}

		switch(idxCnt)
		{
			case M3S_BYTENUM_CTRLBYTE: return(&rFrame->CtrlByte); break;
			case M3S_BYTENUM_SLAVEADDRESS: return(&rFrame->SlaveAddr); break;
			case M3S_BYTENUM_UPPERBOUND: return(&rFrame->UpperBound); break;
			default:
					return(&rFrame->Data[idxCnt-M3S_HEADER_LENGTH-1]);
					break;
		}

		// ERROR!!
		return(NULL);
}

unsigned char extractProtocol(unsigned char vControlByte)
{
	return(vControlByte & M3S_CTRLBYTE_PROTOCOL_gm);
}


unsigned char checkFrameByStream(const unsigned char* pFrameToCheck)
{

	if(extractProtocol(pFrameToCheck[0]) == M3S_CTRLBYTE_PROTOCOL_ACK_gc)
	{
		// Because of the order of the struct you may parse the acknowledge frame into a m3sFrame_t
		// this is a bit tricky though but saves a lot of memory ;)
		return(calcCRC((m3sFrame_t*)pFrameToCheck) == pFrameToCheck[2]);

	}
	else
	{
		// identify upperBound
		unsigned char upperBound = pFrameToCheck[M3S_BYTENUM_UPPERBOUND-1];

		// if crc is null INCLUDING the CRC, everything is fine..
		return(crc8_frameBased(pFrameToCheck, upperBound+1+M3S_OVERHEAD_LENGTH, M3S_CRC_INITVAL) == 0);
	}
}

unsigned char checkFrame(const m3sFrame_t* frameToCheck)
{
	return(frameToCheck->CRC == calcCRC(frameToCheck)); // if crc matches, everything is fine
}

// memory for data is not allocated, but the pointer goes to rToParse
unsigned char parseToFrame(m3sFrame_t* rParsedFrame, unsigned char* rToParse)
{
	unsigned char err = checkFrameByStream(rToParse);

	if(err != 0)
	{
		return(err);
	}

	rParsedFrame->CtrlByte = rToParse[0];
	rParsedFrame->SlaveAddr = rToParse[1];

	if(extractProtocol(rParsedFrame->CtrlByte) != M3S_CTRLBYTE_PROTOCOL_ACK_gc)
	{
		rParsedFrame->UpperBound = 0;
		rParsedFrame->Data = NULL;
		rParsedFrame->CRC = rToParse[2];
	}
	else
	{
		rParsedFrame->UpperBound = rToParse[2];
		rParsedFrame->Data = &rToParse[3];
		rParsedFrame->CRC = rToParse[M3S_HEADER_LENGTH+rParsedFrame->UpperBound+1];
	}

	return(0);

}

