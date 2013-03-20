/*
 * dcm_basicFunctions.c
 *
 * Created: 06.12.2012 11:56:29
 *  Author: SchR
 */ 

#include <stddef.h>
#include "dcm_internal.h"
#include "dcm_hardwareFunctions.h"
#include "dcm_basicFunctions.h"
#include "../m3s/crc8.h"
#include "../m3s/m3sV2_basicFunctions.h"

#include "../m3s/m3s.h"

#include <stdio.h>
#include <time.h>


static m3sFrame_t* curAnswerFrame;

static unsigned char waitingForAck = 0;
static unsigned char waitingForAnswer = 0;

// Stellt einen Pointer auf verwendbaren Speicher zur Verf�gung (w�hlt aus den 2 Spaces aus)
unsigned char* dcm_getAvailableWorkspace()
{
	return(dcs_space1);
}

uint8_t waitForTimeout()
{
	unsigned char timeout = 0;

	clock_t start = clock();
	while((waitingForAck || waitingForAnswer) && !timeout)
	{
		if((((double)clock() - start ) / CLOCKS_PER_SEC)*1000 > currentMaster->ReadTimeout_ms)
		{
			timeout = 1;
		}
	}

	return timeout;
}

uint8_t dcm_sendFrame(m3sFrame_t* pFrame)
{
	unsigned char* Frame = dcm_getAvailableWorkspace();
	unsigned short i;
	

	//set data send direction (master send)
	Frame[0] = pFrame->CtrlByte;
	Frame[1] = pFrame->SlaveAddr;
	Frame[2] = pFrame->UpperBound;										// Datenframe zusammenstellen...
	
	for(i = 0; i < (pFrame->UpperBound+1); i++)
	{
		Frame[i + M3S_HEADER_LENGTH] = pFrame->Data[i];								// Daten kopieren und Verpacken...
	}
	
	
	Frame[pFrame->UpperBound + 1 + M3S_HEADER_LENGTH] = crc8_frameBased(Frame, pFrame->UpperBound + 1 + M3S_HEADER_LENGTH, M3S_CRC_INITVAL);
	

	waitingForAnswer = 0;
	waitingForAck = 0;

	if(pFrame->CtrlByte & (M3S_CTRLBYTE_ACK_OR_MC_bm))
	{
		waitingForAck = 1;
	}

	dcm_send(Frame, (pFrame->UpperBound + 1) + M3S_HEADER_LENGTH + M3S_CRC_LENGTH);

	return waitForTimeout();

}










// 0.. success, other timeout
unsigned char dcm_sendFrameReadAnswer(m3sFrame_t* pFrame, m3sFrame_t* oAnswerFrame)
{
	curAnswerFrame = oAnswerFrame;

	waitingForAnswer = 1;
	waitingForAck = 1;

	dcm_sendFrame(pFrame);



	unsigned char timeout = waitForTimeout();

	if(!timeout)
	{
		oAnswerFrame = curAnswerFrame;
	}
	else
	{
		oAnswerFrame = 0;
	}

	return(timeout);

}

uint8_t dcm_receiveFrame(m3sFrame_t* recFrame)
{
	// TODO: See if this is a response to the frame I sent out...
	waitingForAck = 0;
	waitingForAnswer =0;
	printf("frame received\n");
}

void dcm_sendResetSequence()
{
	unsigned char* resetSequence = dcm_getAvailableWorkspace();
	
	resetSequence[0] = 0x10;
	resetSequence[1] = 0x00;
	resetSequence[2] = 0x00;
	resetSequence[3] = 0x52;
	resetSequence[4] = crc8_frameBased(resetSequence, 4, M3S_CRC_INITVAL);
	unsigned char frameCounter;

	for(frameCounter = 0x00; frameCounter < M3S_RESET_SEQUENCE_LENGTH; frameCounter++)
	{
		dcm_send(resetSequence, (M3S_HEADER_LENGTH + M3S_CRC_LENGTH + 1));
	}
}

uint8_t dcm_ping(unsigned char slaveAddr)
{
	unsigned char pingCmd[]={(unsigned char)'P'};
		m3sFrame_t ping;

		m3sCreateFrame(&ping,
				Command,
				slaveAddr,
				currentMaster->Address,
				1,
				1,
				pingCmd,
				sizeof(pingCmd));

		//currentMaster->SendFrame(&ping);
		return dcm_sendFrame(&ping);
}


static m3sFrame_t lastRecFrame;


void dcm_processReceived(unsigned char pData)
{
	static unsigned short byteCnt = 0; // 0 beim ersten Programmaufruf
	static unsigned char crc = M3S_CRC_INITVAL;
	static unsigned short upperBound = 0;			// h�chster NUTZDATEN Arrayindex, Anzahl der Nutzdaten
	static unsigned char* workspace = NULL;
	
	unsigned char tmpProtocol;
	
	byteCnt++;	// increase for next Byte, note beneath this line, Byte Numbering starts with 1, like good old VB-Times ;)
	
	if(byteCnt == 1 || workspace == NULL)
	{
		workspace = dcm_getAvailableWorkspace();
	}
	
	workspace[byteCnt-1] = pData;		// save @ receive Buffer
	crc = crc8(pData, crc);				// permanently calculate CRC, fastest implementation (?)
	
	//TODO: implement acknoledge
	if(byteCnt==3)						// Das dritte Byte ist entweder der CRC des Acknowledge oder das upperBound
	{
		upperBound = pData;
	}
	
	tmpProtocol = workspace[M3S_BYTENUM_CTRLBYTE-1] & M3S_CTRLBYTE_PROTOCOL_gm;


	if((byteCnt == ((upperBound+1) + M3S_OVERHEAD_LENGTH)) || ((tmpProtocol == M3S_CTRLBYTE_PROTOCOL_ACK_gc) && (byteCnt == M3S_ACK_FRAME_LENGTH))) // Wenn erwartetes Frame empfangen wurde
	{		
		
		if(crc==0) // no error...
		{
			// parse into frame
			m3s_parseToFrame(&lastRecFrame,workspace);
			currentMaster->ReceiveFrameHandler(&lastRecFrame);
			workspace = NULL;
		}
		
		crc = M3S_CRC_INITVAL;
		byteCnt = 0;
		upperBound = 0;
		workspace = NULL;
	}
}
