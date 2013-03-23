/*
 * dcm_basicFunctions.h
 *
 * Created: 05.12.2012 15:04:05
 *  Author: SchR
 */ 


#ifndef DCM_BASICFUNCTIONS_H_
	#define DCM_BASICFUNCTIONS_H_
	
	#include "../m3s/m3s.h"

	#define M3S_OVERHEAD_LENGTH M3S_CRC_LENGTH + M3S_HEADER_LENGTH

	uint8_t dcm_sendFrame(m3sFrame_t* pFrame, uint8_t vExpectsResponse, m3sFrame_t** oAnswerFrame);

	uint8_t dcm_sendResetSequence(unsigned char* oError);
	void dcm_processReceived(unsigned char pData);
	uint8_t dcm_ping(unsigned char slaveAddr, unsigned short* oRTT, unsigned char* oErrorCode);
	uint8_t dcm_readSlaveInfo(unsigned char vSlaveAddr, DevComSlaveInformation_t* oSlaveInfo, unsigned char* oErrorCode);

	uint8_t dcm_receiveFrame(m3sFrame_t* receivedFrame);

#endif /* DCM_BASICFUNCTIONS_H_ */
