/*
 * DevComUtils.h
 *
 *  Created on: Mar 23, 2013
 *      Author: root
 */

#ifndef DEVCOMUTILS_H_
#define DEVCOMUTILS_H_

#include "../DevCom.h"
	void dc_slaveInfoToString(const DevComSlaveInformation_t* rSlaveInfo, char* oBuffer);

	void dc_errCodeToString(unsigned char errCode, char* oStrBuffer);
#endif /* DEVCOMUTILS_H_ */
