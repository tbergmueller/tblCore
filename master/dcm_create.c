/*
 * dcm_create.c
 *
 * Created: 05.12.2012 14:51:15
 *  Author: SchR
 */ 

#include "../DevCom.h"

//TODO: look for different solution
DevComMaster_t hiddenGlobalDevComMaster;

DevComMaster_t* dcm_create(unsigned char MasterAddress)
{
	unsigned char cnt;

	unsigned char* ptr = (unsigned char*)&hiddenGlobalDevComMaster;

	for(cnt = 0; cnt < sizeof(DevComMaster_t); cnt++)
	{
		*ptr = 0;
		ptr++;
	}
	
	//master address consists of 2 bits
	hiddenGlobalDevComMaster.Address = (0x03 & MasterAddress);
	hiddenGlobalDevComMaster.RWPolarity = 1;
	hiddenGlobalDevComMaster.ReadTimeout_ms = 500;
	
	return(&hiddenGlobalDevComMaster);
}
