/*
 * Service.cpp
 *  业务处理
 *  Created on: 2013-2-21
 *      Author: Administrator
 */

#include "Service.h"
#include "LiteReactorBase.h"
#include "MyDef.h"
#include "DebugData.h"
#include "DataBuffer.h"
#include "Serializer.h"
#include "MyArchv.h"


namespace service
{

/**
 *@breif 业务处理入口
 *@param ptrReq 请求包
 *@param iReqSize 请求包长度
 *@param ptrAck 应答包
 *@param iAckSize 应答包长度
 *
 */
void DealService(char * ptrReq, int iReqSize, char * ptrAck, int &iAckSize)
{

    int iRet = 0;
    LOG( LOG_DEBUG, "req hex string[%s]", DEBUG_HEXSTRING(ptrReq, iReqSize).c_str());

    //解析请求报文包头
    PkgHead pkgHeadReq;
    memset(&pkgHeadReq, 0, sizeof(pkgHeadReq));
    pkgHeadReq.unpacket(ptrReq,1);

    //业务处理入口
    int iAckBodySize = 0;
    switch(pkgHeadReq.uiMsgType)
    {
        case MSGTYPE_GETUSERALIAS:  //3005
            func3005(pkgHeadReq, ptrReq + sizeof(PkgHead), iReqSize - sizeof(PkgHead), ptrAck + sizeof(PkgHead), iAckBodySize, MAXSOCKBUFF - sizeof(PkgHead));
            break;

        default:
            LOG( LOG_ERROR, "msgType error!msgType[%d]", pkgHeadReq.uiMsgType );
            makeMiniAck(ERROR_MSGTYPE, ptrAck+sizeof(PkgHead), iAckBodySize);
            break;
    }

    //应答包头赋值
    PkgHead pkgHeadAck;
    memcpy(&pkgHeadAck,&pkgHeadReq, sizeof(pkgHeadAck));
    pkgHeadAck.uiPkgBodyLen = iAckBodySize;           //包体长度
    pkgHeadAck.packet(ptrAck,1);                  //组装应答包

    //整个包的长度
    iAckSize = sizeof(PkgHead) + iAckBodySize;

    //应答包打印
    LOG( LOG_DEBUG, "req hex string[%s]", DEBUG_HEXSTRING(ptrAck, iAckSize).c_str());
    return;
}

/**
 *@brief
 *@param pkgHeadReq 请求包包头
 *@param ptrPkgBodyReq 请求包包体
 *@param iPkgBodyReqLen 请求包包体长度
 *@param ptrPkgBodyAck 应答包包体
 *@param input/output iPkgBodyAckLen 应答包包体长度
 *@param iMaxPkgBodyAckLen 最大应答包包体长度
 */
void func3005(PkgHead &pkgHeadReq, char *ptrPkgBodyReq, int iPkgBodyReqLen, char *ptrPkgBodyAck, int&iPkgBodyAckLen, int iMaxPkgBodyAckLen)
{

    DataBuffer dataBufferReq(ptrPkgBodyReq, iPkgBodyReqLen, iPkgBodyReqLen);
    Serializer s(&dataBufferReq, true);
    Archv3005Req archv3005Req;
    int iRetCode = 0;
    iPkgBodyAckLen = 0;

    Archv3005Ack archv3005Ack;

    //反序列化
    s >> archv3005Req;
    if(s.GetErrorCode())
    {
        iRetCode = ERROR_PARAM;
        LOG( LOG_ERROR, "GetErrorCode error!msgType[%d]", pkgHeadReq.uiMsgType );
        goto EndOfProcess;
    }

    LOG( LOG_DEBUG, "archv3005Req, userNumber[%s],opType[%d]", archv3005Req.userNumber.c_str(), archv3005Req.opType);


    //应答包体赋值
    archv3005Ack.errorCode = 0;
    archv3005Ack.userNumber = "13800138000";
    archv3005Ack.userAlias = "userAlias_111";

EndOfProcess: //使用goto 对释放资源,组应答包 做集中处理

    //错误应答
    if(iRetCode)
    {
        makeMiniAck(iRetCode, ptrPkgBodyAck, iPkgBodyAckLen);
        return;
    }

    //正确应答
    DataBuffer dataBufferAck(ptrPkgBodyAck, iMaxPkgBodyAckLen ,0);
    s.SetDataBuffer(&dataBufferAck);
    s << archv3005Ack;
    if(s.GetErrorCode())
    {
        LOG( LOG_ERROR, "GetErrorCode error!msgType[%d]", pkgHeadReq.uiMsgType);
    }
    iPkgBodyAckLen  = dataBufferAck.GetDataSize();  //应答包体长度
}

/**
 *@breif 简单应答包组装
 *  应答包中仅仅包含一个字段  retCode
 *@breif ptrPkgBodyAck 应答包包体
 *@breif iPkgBodyAckLen 应答包包体长度
 */
void makeMiniAck(int iErrorCode, char *ptrPkgBodyAck, int&iPkgBodyAckLen)
{
    UInt16 uiErrorcode = iErrorCode;
    memcpy(ptrPkgBodyAck, &uiErrorcode, sizeof(uiErrorcode));
    iPkgBodyAckLen = sizeof(uiErrorcode);
}

}

