/*
 * dcs_hardwareFunctions.h
 *
 * Created: 05.12.2012 14:26:32
 *  Author: SchR
 */ 


#ifndef DCS_HARDWAREFUNCTIONS_H_
	#define DCS_HARDWAREFUNCTIONS_H_


	// Send-Function, Regardless if Max or not Max
	void dcm_send(unsigned char* chArr, unsigned short anzBytes);

	// Schreibt ein Zeichen auf die Usart
	void dcm_usart_putc(unsigned char);

	//sets baud rate
	void dcm_setBaudrate();


#endif /* DCS_HARDWAREFUNCTIONS_H_ */
