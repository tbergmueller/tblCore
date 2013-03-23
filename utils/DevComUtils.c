/*
 * DevComUtils.c
 *
 *  Created on: Mar 23, 2013
 *      Author: root
 */


#include "DevComUtils.h"
#include <stdio.h>

void dc_slaveInfoToString(const DevComSlaveInformation_t* rSlaveInfo, char* oBuffer)
{
	sprintf(oBuffer,"Addr. %d (MC %d), V%d.%d%c (TODO: implemented Features) DeviceID: %d, DataUpperBound: %d",
			rSlaveInfo->SlaveAddress,
			rSlaveInfo->MulticastAddress,
			rSlaveInfo->Version,
			rSlaveInfo->Subversion,
			rSlaveInfo->Revision,
			rSlaveInfo->DeviceID,
			rSlaveInfo->DataUpperBound);
}


void dc_errCodeToString(unsigned char errCode, char* oStrBuffer)
{
	switch(errCode)
	{
		case DEVCOM_ERRCODE_TIMEOUT:
						sprintf(oStrBuffer, "#%d: Timeout for last sent frame - Response and/or Acknowledge pending", errCode);
						break;

		case DEVCOM_ERRCODE_NAK_RECEIVED:
					sprintf(oStrBuffer, "#%d: Slave signalled 'not acknowledge' (NAK)", errCode);
					break;






		case DEVCOM_ERRCODE_NO_ERROR:
			sprintf(oStrBuffer, "#%d: No Error, everything is fine", errCode);
			break;

		case DEVCOM_ERRCODE_NOT_IMPLEMENTED:
			sprintf(oStrBuffer, "#%d: Called function is not implemented yet. Please file a Bug-Report", errCode);
			break;

		case DEVCOM_ERRCODE_NO_DETAILED_ERRORHANDLING:
			sprintf(oStrBuffer, "#%d: The called function has not implemented detailed Errorhandling yet. Please file a Bug-Report", errCode);
			break;
		default:
			sprintf(oStrBuffer, "#%d: Unknown DevCom-ErrorCode", errCode);
			break;
	}
}
