/*
 * m3s_basicFunctions.h
 *
 * Created: 13.12.2011 14:46:41
 *  Author: tbergmueller
 */


#ifndef M3S_BASICFUNCTIONS_H_
	#define M3S_BASICFUNCTIONS_H_

	// Teilrahmen, Rahmenlängen
	#define M3S_CRC_LENGTH						1		// 1 Byte CRC
	#define M3S_HEADER_LENGTH 					3		// ctrlbyte, address, upperBound
	#define M3S_ACK_FRAME_LENGTH				3
	#define M3S_OVERHEAD_LENGTH					M3S_CRC_LENGTH + M3S_HEADER_LENGTH


	#define M3S_BROADCAST_ADDRESS				0x00

	#define M3S_PARAM_UPPERBOUND_RECEIVE_ALL	0x00

	#define M3S_CRC_INITVAL						0

	// Controlbyte
	#define M3S_CTRLBYTE_PROTOCOL_gm				0xF0



	// Bit Positions
	#define M3S_CTRLBYTE_ACK_OR_MC_bp					1
	#define M3S_CTRLBYTE_ACK_OR_MC_bm					(1<<M3S_CTRLBYTE_ACK_OR_MC_bp)
	#define M3S_CTRLBYTE_SS_bp							0
	#define M3S_CTRLBYTE_PROTOCOL_FLAG_RESPONSE_bp 		4

	// Group positions
	#define M3S_CTRLBYTE_MASTERADDR_gp					2

	// Acknowledge
	#define M3S_ACK_YES_gc								0x02
	#define M3S_ACK_NO_gc								0x00
	#define M3S_MC_YES_gc								M3S_ACK_YES
	#define M3S_MC_NO_gc								M3S_ACK_NO


	// Positionen im Übertragungsrahmen
	#define M3S_BYTENUM_CTRLBYTE					1
	#define M3S_BYTENUM_SLAVEADDRESS				2
	#define M3S_BYTENUM_UPPERBOUND					3

	#define M3S_UPPERBOUND_IDX						2

	#define M3S_ERROR_FRAME_NOT_ACK_FRAME_gc		0x0A
	#define M3S_ERROR_CHECKSUM_ERROR_gc				0x0C
	#define M3S_ERROR_NOERROR_gc					0x00
	#define M3S_ERROR_WRONG_SLAVEADDR_gc			0x01


	#define M3S_RESETFRAME_BYTE1					0x10
	#define M3S_RESETFRAME_BYTE2					0x00
	#define M3S_RESETFRAME_BYTE3					0x00
	#define M3S_RESETFRAME_BYTE4					'R'
	#define M3S_RESETFRAME_BYTE5					0x5F		// this is CRC


	// Prototypen

	unsigned char m3s_validateFrame(unsigned char* pFrame, unsigned short pFrameLength);



#endif /* M3S_BASICFUNCTIONS_H_ */
