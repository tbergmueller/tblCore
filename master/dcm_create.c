/*
 * dcm_create.c
 *
 * Created: 05.12.2012 14:51:15
 *  Author: SchR
 */ 

#include "../devComMaster.h"

//TODO: look for different solution
DevComMaster_t hiddenGlobalDevComMaster;

DevComMaster_t* dcm_create(unsigned char MasterAddress)
{
	unsigned char cnt;
	for(cnt = 0; cnt < sizeof(hiddenGlobalDevComMaster); cnt++)
	{
		*(unsigned char*)(&hiddenGlobalDevComMaster+cnt) = 0x00;
	}
	
	//master address consists of 2 bits
	hiddenGlobalDevComMaster.Address = (0x03 & MasterAddress);
	hiddenGlobalDevComMaster.RWPolarity = 1;
	hiddenGlobalDevComMaster.ReadTimeout_ms = 500;
	
	return(&hiddenGlobalDevComMaster);
}
