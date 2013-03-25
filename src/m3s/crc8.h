/*
* crc.h
*
* Created: 22.01.2012 14:17:29
* Author: tbergmueller
*/


#ifndef CRC8_H_
#define CRC8_H_

unsigned char crc8_frameBased(const unsigned char* rFrame, unsigned short vFrameLength, unsigned char vInitialValue);
unsigned char crc8(unsigned char vByte, unsigned char vCurrentCrc);


#endif /* CRC_H_ */
