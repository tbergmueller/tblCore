/*
 * m3s.h
 *
 *  Created on: Nov 12, 2012
 *      Author: tbergmueller
 */

#ifndef M3S_H_
	#define M3S_H_

	typedef struct _m3sFrame_t
	{
		unsigned char CtrlByte;
		unsigned char SlaveAddr;

		unsigned char UpperBound;
		unsigned char* Data;

		unsigned char CRC;
	} m3sFrame_t;

	unsigned char m3sCreateFrame(
			m3sFrame_t* rTarget,
			unsigned char vProtocol,
			unsigned char vSlaveAddress,
			unsigned char vMasterAddress,
			unsigned char vAckOrAckRequest,
			unsigned char vMasterSend,
			unsigned char* rData,
			unsigned char rDataLength
			);

	unsigned char* m3sStream(m3sFrame_t* rFrame, unsigned char status);
	unsigned char extractProtocol(unsigned char vControlByte);

	#define M3S_STREAM_START	1
	#define M3S_STREAM_CONTINUE	0

	#define M3S_CTRLBYTE_PROTOCOL_RESET_gc			0x10
	#define M3S_CTRLBYTE_PROTOCOL_BROADCAST_gc		0x20
	#define M3S_CTRLBYTE_PROTOCOL_DATA_gc			0x30
	#define M3S_CTRLBYTE_PROTOCOL_CMD_gc			0x40
	#define M3S_CTRLBYTE_PROTOCOL_ACK_gc			0x80
	#define M3S_CTRLBYTE_PROTOCOL_CMDBROADCAST_gc   0x90
	#define M3S_CTRLBYTE_PROTOCOL_FILETRANSFER_gc	0xA0


	typedef enum _m3s_protocol_t
	{
		Reset=0x10,
		DataBroadcast=0x20,
		Data=0x30,
		Command=0x40,
		Acknowledge=0x80,
		CommandBroadcast=0x90,
		FileTransfer=0xA0
	} m3sProtocol_t;


#endif /* M3S_H_ */
