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
static signed short lastCRC = -1;
static DevComMaster_t* currentMaster=NULL;

static unsigned char dcs_space1[DCS_BUFFERSIZE];
	//static unsigned char dcs_space2[DCS_BUFFERSIZE];



void setCurrentMaster(DevComMaster_t* master)
{
	currentMaster = master;
}

// Stellt einen Pointer auf verwendbaren Speicher zur Verf�gung (w�hlt aus den 2 Spaces aus)
unsigned char* dcm_getAvailableWorkspace()
{
	return(dcs_space1);
}

DevComMaster_t* getCurrentMaster()
{
	return currentMaster;
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


/**
 * Sends a Frame and wait's for acknowledge (if set in Frame header) and/or answer (if passed accordingly)
 * @param pFrame	Frame to send
 * @param vExpectsResponse 1 .. wait for answer, 0.. Fire and Forget
 * @param oAnswerFrame Pointer to Received frame. Chills somewhere around in workspaces
 * @return 1 on Success, 0 on Failure
 */
uint8_t dcm_sendFrame(m3sFrame_t* pFrame, uint8_t vExpectsResponse, m3sFrame_t** oAnswerFrame, unsigned char* oErrorCode)
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

	if(vExpectsResponse)
	{
		waitingForAnswer = 1;
	}


	dcm_send(Frame, (pFrame->UpperBound + 1) + M3S_HEADER_LENGTH + M3S_CRC_LENGTH);

	lastCRC = -1; // reset
	uint8_t timeout = waitForTimeout();


	// ################ done, evaluate
	if(timeout)
	{
		if(lastCRC < 0) // nothing happened, never got so far as to check a CRC
		{
			SET_ERR_CODE(oErrorCode, DEVCOM_ERRCODE_TIMEOUT); // ordinary timeout
		}
		else
		{
			if(lastCRC != 0)
			{
				SET_ERR_CODE(oErrorCode, DEVCOM_ERRCODE_CRC); // timeout because CRC mismatch
			}
		}
		return 0; // error
	}



	// If response was required
	if((pFrame->CtrlByte & (M3S_CTRLBYTE_ACK_OR_MC_bm)) || vExpectsResponse)
	{
		// Check Slave-Adress of replied frame
		if(pFrame->SlaveAddr != curAnswerFrame->SlaveAddr)
		{
			SET_ERR_CODE(oErrorCode,DEVCOM_ERRCODE_SLAVEADDR_MISMATCH);
			return 0;
		}

		// Check if master address is set to my address
		if((pFrame->CtrlByte & (3<<M3S_CTRLBYTE_MASTERADDR_gp)) != (curAnswerFrame->CtrlByte & (3<<M3S_CTRLBYTE_MASTERADDR_gp)))
		{
			SET_ERR_CODE(oErrorCode,DEVCOM_ERRCODE_MASTERADDR_MISMATCH);
			return 0;
		}


		// Check if protocol matches..
		unsigned char recProt = curAnswerFrame->CtrlByte & M3S_CTRLBYTE_PROTOCOL_gm;

		if(recProt != M3S_CTRLBYTE_PROTOCOL_CMD_ANSWER_gc && recProt != M3S_CTRLBYTE_PROTOCOL_ACK_gc)
		{
			SET_ERR_CODE(oErrorCode,DEVCOM_ERRCODE_PROTOCOL_MISMATCH);
			return 0;
		}

		if(vExpectsResponse)
		{
			*oAnswerFrame = curAnswerFrame;
			if(recProt != M3S_CTRLBYTE_PROTOCOL_CMD_ANSWER_gc)
			{
				SET_ERR_CODE(oErrorCode,DEVCOM_ERRCODE_PROTOCOL_MISMATCH);
				return 0;
			}
		}
		else
		{
			if(pFrame->CtrlByte & (M3S_CTRLBYTE_ACK_OR_MC_bm)) // i wanted some ack
			{
				// here should only be ack-protocol ;)
				if(!(curAnswerFrame->CtrlByte & (M3S_CTRLBYTE_ACK_OR_MC_bm)))
				{
					SET_ERR_CODE(oErrorCode, DEVCOM_ERRCODE_NAK_RECEIVED);
					return 0; // error
				}
			}
		}
	}


	return !timeout;
}


uint8_t dcm_receiveFrame(m3sFrame_t* recFrame)
{
	// TODO: See if this is a response to the frame I sent out...
	waitingForAck = 0;
	waitingForAnswer =0;
	curAnswerFrame = recFrame;

	//printf("frame received\n");
	return 0;
}

/**
 * @brief Reads information from a remote slave with specified address.
 * @param vSlaveAddr SlaveAddress, from whom the information should be retrieved
 * @param oSlaveInfo Output of received slave information. Does NOT allocate memory.
 * @param oErrorCode Output of ErrorCode for a more detailed error analysis. Pass NULL if unwanted.
 * @return 1 on Success, 0 on Failure. Examine oErrorCode!
 */
uint8_t dcm_readSlaveInfo(unsigned char vSlaveAddr, DevComSlaveInformation_t* oSlaveInfo, unsigned char* oErrorCode)
{
	unsigned char infoRequCmd = 'i';

	DevComPayload_t p;
	DevComPayload_t out;

	p.Length = 1;
	p.Data = &infoRequCmd;

	unsigned char success = currentMaster->Request(vSlaveAddr, &p, &out, oErrorCode);

	if(success)
	{
		unsigned char i;
		unsigned char* ptr = (unsigned char*)oSlaveInfo;

		for(i=0; i<sizeof(DevComSlaveInformation_t); i++)
		{
			*ptr = out.Data[i];
			ptr++;
		}

		// Since some are big and some little endian, manually parse DevID
		oSlaveInfo->DeviceID = (((unsigned short)(out.Data[6])) << 8) | (unsigned short)(out.Data[7]);
	}

	return success;

}


uint8_t dcm_dataBC(unsigned char vMulticastAddress, const DevComPayload_t* pData, unsigned char* oErrorCode)
{
	m3sFrame_t data;

	m3sCreateFrame(&data,
				DataBroadcast,
				vMulticastAddress,
				currentMaster->Address,
				0,
				1,
				pData->Data,
				pData->Length);

	return dcm_sendFrame(&data,0,NULL, oErrorCode);
}

uint8_t dcm_commandBC(unsigned char vMulticastAddress, const DevComPayload_t* pCommand, unsigned char* oErrorCode)
{
	m3sFrame_t cmd;

	m3sCreateFrame(&cmd,
				CommandBroadcast,
				vMulticastAddress,
				currentMaster->Address,
				0,
				1,
				pCommand->Data,
				pCommand->Length);

	return dcm_sendFrame(&cmd,0,NULL, oErrorCode);
}


uint8_t dcm_command(unsigned char vSlaveAddr, const DevComPayload_t* pCommand, unsigned char vAcknowledgeRequired, unsigned char* oErrorCode)
{
	m3sFrame_t cmd;

		m3sCreateFrame(&cmd,
					Command,
					vSlaveAddr,
					currentMaster->Address,
					vAcknowledgeRequired,
					1,
					pCommand->Data,
					pCommand->Length);

		unsigned char success = dcm_sendFrame(&cmd,0,NULL, oErrorCode);

		return success;

}


uint8_t dcm_data(unsigned char vSlaveAddr, const DevComPayload_t* pData, unsigned char vAcknowledgeRequired, unsigned char* oErrorCode)
{
	m3sFrame_t dat;

	m3sCreateFrame(&dat,
				Data,
				vSlaveAddr,
				currentMaster->Address,
				vAcknowledgeRequired,
				1,
				pData->Data,
				pData->Length);

	return dcm_sendFrame(&dat,0,NULL, oErrorCode);

}



uint8_t dcm_request(unsigned char vSlaveAddr, const DevComPayload_t* rCommand, DevComPayload_t* oPayload, unsigned char* oErrorCode )
{
	m3sFrame_t request;

	m3sCreateFrame(&request,
				Command,
				vSlaveAddr,
				currentMaster->Address,
				1,
				1,
				rCommand->Data,
				rCommand->Length);

	m3sFrame_t* receivedFrame = NULL;

	unsigned char success = dcm_sendFrame(&request,1,&receivedFrame, oErrorCode);

	if(success)
	{
		oPayload->Length = receivedFrame->UpperBound+1;
		oPayload->Data = receivedFrame->Data;
	}
	else
	{
		oPayload->Length = 0;
		oPayload->Data = NULL;
	}

	return success;
}



uint8_t dcm_sendResetSequence(unsigned char* oError)
{
	if(oError)
	{
		*oError = DEVCOM_ERRCODE_NO_DETAILED_ERRORHANDLING;
	}

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

	return 1; // no error handling, always sucessful...
}


uint8_t dcm_ping(unsigned char slaveAddr, unsigned short* oRTT, unsigned char* oErrorCode)
{
	if(oErrorCode)
	{
		*oErrorCode = DEVCOM_ERRCODE_NO_DETAILED_ERRORHANDLING;
	}

	if(oRTT)
	{
		oRTT = NULL;
	}

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
		return dcm_sendFrame(&ping,0,NULL, oErrorCode);
}


static m3sFrame_t lastRecFrame;


void dcm_processReceived(unsigned char pData)
{
	static unsigned short byteCnt = 0; // 0 beim ersten Programmaufruf
	static unsigned char crc = M3S_CRC_INITVAL;
	static unsigned short upperBound = 0;			// höchster NUTZDATEN Arrayindex, Anzahl der Nutzdaten
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
		lastCRC = crc;
		if(crc==0) // no error...
		{
			// parse into frame
			m3s_parseToFrame(&lastRecFrame,workspace);
			dcm_receiveFrame(&lastRecFrame);
			workspace = NULL;
		}
		else
		{
			// CRC MISMATCH!!!
			if(currentMaster->ReportOnReceiveError != NULL)
			{
				currentMaster->ReportOnReceiveError(workspace, byteCnt);
			}

		}
		
		crc = M3S_CRC_INITVAL;
		byteCnt = 0;
		upperBound = 0;
		workspace = NULL;
	}
}
