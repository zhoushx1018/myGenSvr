/*
 * Service.h
 *  业务处理
 *  Created on: 2013-2-21
 *      Author: Administrator
 */

#ifndef SERVICE_H_
#define SERVICE_H_
#include "PkgHead.h"


namespace service
{

void DealService(char * ptrReq, int iReqSize, char * ptrAck, int &iAckSize);
void func3005(PkgHead &pkgHeadReq, char *ptrPkgBodyReq, int iPkgBodyReqLen, char *ptrPkgBodyAck, int&iPkgBodyAckLen, int iMaxPkgBodyAckLen);
void makeMiniAck(int iErrorCode, char *ptrPkgBodyAck, int&iPkgBodyAckLen);

}

#endif /* SERVICE_H_ */
