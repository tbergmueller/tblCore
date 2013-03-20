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

	uint8_t dcm_sendFrame(m3sFrame_t* pFrame); // Sendet ein Frame vom Slave an den Master (dh. keine Ack-Behandlung)
	void dcm_sendResetSequence(); //sends a reset sequence
	void dcm_processReceived(unsigned char pData);
	unsigned char dcm_ping(unsigned char slaveAddr);

	uint8_t dcm_receiveFrame(m3sFrame_t* receivedFrame);

#endif /* DCM_BASICFUNCTIONS_H_ */
