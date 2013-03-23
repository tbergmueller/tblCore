/*
 * DevComCore.h
 *
 *  Created on: Mar 23, 2013
 *      Author: root
 */


	#include <inttypes.h>


#ifndef DEVCOMCORE_H_
	#define DEVCOMCORE_H_

	typedef struct dc_payload_t
	{
		unsigned char* Data;
		unsigned char Length;
	} DevComPayload_t;

	typedef struct dc_slaveInformation_t
	{
		unsigned char Version;
		unsigned char Subversion;
		unsigned char Revision;
		unsigned char Implementation;
		unsigned char SlaveAddress;
		unsigned char MulticastAddress;
		unsigned short DeviceID;
		unsigned char DataUpperBound;
	} DevComSlaveInformation_t;


	typedef struct devComMasterStruct
	{
		unsigned char Address; //Master Address
		char* UART;
		// RW-Output (if enabled)
		volatile uint8_t* RWPort;					// Port, where RW-Wire is connected (e.g. for Buscoupler,...)
		uint8_t RW_bp;					// Bit Position, where RW-Wire is connected (Pn0...Pn7), e.g. PC3
		unsigned char RWPolarity;				// 0... when writing, RW-Wire is logic 0, 1 when reading
															// 1... when writing, RW-Wire is logic 1, 0 when reading
		//volatile unsigned char*		Data;					// Pointer to valid Data
		short ReadTimeout_ms;

		uint8_t (*Ping)(unsigned char vSlaveAddr, unsigned short* oRTT, unsigned char* oErrorCode);
		uint8_t (*SendResetSequence)(unsigned char* oErrorCode);
		uint8_t (*ReadSlaveInformation)(unsigned char vSlaveAddr, DevComSlaveInformation_t* oSlaveInformation, unsigned char* oErrorCode);


	} DevComMaster_t;


	DevComMaster_t* dcm_create(unsigned char MasterAddress);

	void dcm_start(DevComMaster_t* rMaster, uint32_t vFcpu, uint32_t vBaudrate);


	#define DEVCOM_ERRCODE_NOT_IMPLEMENTED					255
	#define DEVCOM_ERRCODE_NO_DETAILED_ERRORHANDLING		254


#endif /* DEVCOMCORE_H_ */
