/*
 * devComMaster.h
 *
 * Created: 05.12.2012 14:37:14
 *  Author: SchR
 */ 


#ifndef DEVCOMMASTER_H_
	#define DEVCOMMASTER_H_

	#include <inttypes.h>
	#include "m3s/m3s.h"

	//TODO: review
	typedef uint8_t (*DevComMasterFrameTransmitter_t)(m3sFrame_t* pFrame); //Sends a m3s frame, master to slave(s)
	typedef unsigned char (*DevComMasterPinger_t)(unsigned char vSlaveAddr); //Sends a m3s frame, master to slave(s)

	typedef void (*DevComMasterResetSequenceTransmitter_t)(void);

	typedef uint8_t (*DevComMasterFrameReceiver_t)(m3sFrame_t* receivedFrame);

	typedef void (*DevComMasterRxCCallback_t)(unsigned char pReceivedByte);
	typedef void (*DevComMasterTxCCallback_t)();

	typedef struct devComMasterStruct
	{
		unsigned char  Address; //Master Address
		// RW-Output (if enabled)
		volatile uint8_t* RWPort;					// Port, where RW-Wire is connected (e.g. for Buscoupler,...)
		uint8_t RW_bp;					// Bit Position, where RW-Wire is connected (Pn0...Pn7), e.g. PC3
		unsigned char RWPolarity;				// 0... when writing, RW-Wire is logic 0, 1 when reading
															// 1... when writing, RW-Wire is logic 1, 0 when reading
		//volatile unsigned char*		Data;					// Pointer to valid Data
		short ReadTimeout_ms;
		DevComMasterFrameTransmitter_t SendFrame;
		DevComMasterResetSequenceTransmitter_t SendResetSequence;
		DevComMasterFrameReceiver_t ReceiveFrameHandler;
		DevComMasterPinger_t Ping;
	} DevComMaster_t;

	DevComMaster_t* dcm_create(unsigned char MasterAddress);
	
	void dcm_start(DevComMaster_t* rMaster, uint32_t vFcpu, uint32_t vBaudrate);

#endif /* DEVCOMMASTER_H_ */
