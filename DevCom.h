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
		uint8_t (*RequestSlaveInformation)(unsigned char vSlaveAddr, DevComSlaveInformation_t* oSlaveInformation, unsigned char* oErrorCode);

		// Raw functions
		uint8_t (*Request)(unsigned char vSlaveAddr, const DevComPayload_t* pCommand, DevComPayload_t* oPayload, unsigned char* oErrorCode );
		uint8_t (*SendCommand)(unsigned char vSlaveAddr, const DevComPayload_t* pCommand, unsigned char vAcknowledgeRequired, unsigned char* oErrorCode);
		uint8_t (*SendData)(unsigned char vSlaveAddr, const DevComPayload_t* pData, unsigned char vAcknowledgeRequired, unsigned char *oErrorCode);

		// Broad-/Multicasts
		uint8_t (*SendCommandBroadcast)(unsigned char vMulticastAddress, const DevComPayload_t* pCommand, unsigned char* oErrorCode);
		uint8_t (*SendDataBroadcast)(unsigned char vMulticastAddress, const DevComPayload_t* pData, unsigned char* oErrorCode);

		void (*ReportOnReceiveError)(const unsigned char* ReceiveBuffer, unsigned char length);

	} DevComMaster_t;


	DevComMaster_t* dcm_create(unsigned char MasterAddress);

	void dcm_start(DevComMaster_t* rMaster, uint32_t vFcpu, uint32_t vBaudrate);
	void dcm_stop(DevComMaster_t* rMaster);


	#define DC_BROADCAST									0

	#define DC_ACK_REQUIRED									1
	#define DC_ACK_NO										0



	#define DEVCOM_ERRCODE_NOT_IMPLEMENTED					255
	#define DEVCOM_ERRCODE_NO_DETAILED_ERRORHANDLING		254

	#define DEVCOM_ERRCODE_PROTOCOL_MISMATCH				14
	#define DEVCOM_ERRCODE_CRC								13
	#define DEVCOM_ERRCODE_MASTERADDR_MISMATCH				12
	#define DEVCOM_ERRCODE_SLAVEADDR_MISMATCH				11
	#define DEVCOM_ERRCODE_NAK_RECEIVED						10
	#define DEVCOM_ERRCODE_TIMEOUT							9

	#define DEVCOM_ERRCODE_NO_ERROR							0


#endif /* DEVCOMCORE_H_ */
