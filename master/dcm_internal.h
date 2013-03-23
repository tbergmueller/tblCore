/*
 * dcm_internal.h
 *
 * Created: 05.12.2012 15:08:14
 *  Author: SchR
 */ 


#ifndef DCM_INTERNAL_H_
	#define DCM_INTERNAL_H_

	#include "../DevCom.h"
	#include "../m3s/m3sV2_basicFunctions.h"

	DevComMaster_t* currentMaster;



	#define DCM_STATUS_DEFAULT				0
	#define DCM_STATUS_RECRESET_bm			0x01 // LSB... check reset, es wurde ein ResetCtrlByte empfangen, w�hrend dem normalen Betrieb. pr�fe weiteren Rahmen
	#define DCM_STATUS_DORESET_bm			0x02 // Wenn dieses Bit gesetzt wird, dann werden alle Variablen (static im processData()) zur�ckgesetzt
	#define DCM_STATUS_BYPASS_bm			0x04 // Wenn irgendwelche Daten nicht f�r mich bestimmt sind
	#define DCM_STATUS_BYPASS_ACK_bm		0x10 // Diese DAten sind im speziellen ein Acknowledgeframe (nur 5 Byte lang)
	#define DCM_STATUS_RECEIVE_CHECKSUM_bm	0x20 // Beginne nun mit dem Empfang der Pr�fsumme (letzten 3 Byte)
	
	#define TRUE 1
	#define FALSE 0

	#define DCS_BUFFERSIZE		(256 + M3S_OVERHEAD_LENGTH)
	
	// Buffer und gültige Daten togglen immer hin und her..
	unsigned char dcs_space1[DCS_BUFFERSIZE];
	unsigned char dcs_space2[DCS_BUFFERSIZE];

	uint32_t dc_fcpu;
	uint32_t dc_baudrate;
	
	volatile unsigned char changeBaudrateFlag;

#endif /* DCM_INTERNAL_H_ */
