/*
 * Service.cpp
 *  ҵ����
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
 *@breif ҵ�������
 *@param ptrReq �����
 *@param iReqSize ���������
 *@param ptrAck Ӧ���
 *@param iAckSize Ӧ�������
 *
 */
void DealService(char * ptrReq, int iReqSize, char * ptrAck, int &iAckSize)
{

    int iRet = 0;
    LOG( LOG_DEBUG, "req hex string[%s]", DEBUG_HEXSTRING(ptrReq, iReqSize).c_str());

    //���������İ�ͷ
    PkgHead pkgHeadReq;
    memset(&pkgHeadReq, 0, sizeof(pkgHeadReq));
    pkgHeadReq.unpacket(ptrReq,1);

    //ҵ�������
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

    //Ӧ���ͷ��ֵ
    PkgHead pkgHeadAck;
    memcpy(&pkgHeadAck,&pkgHeadReq, sizeof(pkgHeadAck));
    pkgHeadAck.uiPkgBodyLen = iAckBodySize;           //���峤��
    pkgHeadAck.packet(ptrAck,1);                  //��װӦ���

    //�������ĳ���
    iAckSize = sizeof(PkgHead) + iAckBodySize;

    //Ӧ�����ӡ
    LOG( LOG_DEBUG, "req hex string[%s]", DEBUG_HEXSTRING(ptrAck, iAckSize).c_str());
    return;
}

/**
 *@brief
 *@param pkgHeadReq �������ͷ
 *@param ptrPkgBodyReq ���������
 *@param iPkgBodyReqLen ��������峤��
 *@param ptrPkgBodyAck Ӧ�������
 *@param input/output iPkgBodyAckLen Ӧ������峤��
 *@param iMaxPkgBodyAckLen ���Ӧ������峤��
 */
void func3005(PkgHead &pkgHeadReq, char *ptrPkgBodyReq, int iPkgBodyReqLen, char *ptrPkgBodyAck, int&iPkgBodyAckLen, int iMaxPkgBodyAckLen)
{

    DataBuffer dataBufferReq(ptrPkgBodyReq, iPkgBodyReqLen, iPkgBodyReqLen);
    Serializer s(&dataBufferReq, true);
    Archv3005Req archv3005Req;
    int iRetCode = 0;
    iPkgBodyAckLen = 0;

    Archv3005Ack archv3005Ack;

    //�����л�
    s >> archv3005Req;
    if(s.GetErrorCode())
    {
        iRetCode = ERROR_PARAM;
        LOG( LOG_ERROR, "GetErrorCode error!msgType[%d]", pkgHeadReq.uiMsgType );
        goto EndOfProcess;
    }

    LOG( LOG_DEBUG, "archv3005Req, userNumber[%s],opType[%d]", archv3005Req.userNumber.c_str(), archv3005Req.opType);


    //Ӧ����帳ֵ
    archv3005Ack.errorCode = 0;
    archv3005Ack.userNumber = "13800138000";
    archv3005Ack.userAlias = "userAlias_111";

EndOfProcess: //ʹ��goto ���ͷ���Դ,��Ӧ��� �����д���

    //����Ӧ��
    if(iRetCode)
    {
        makeMiniAck(iRetCode, ptrPkgBodyAck, iPkgBodyAckLen);
        return;
    }

    //��ȷӦ��
    DataBuffer dataBufferAck(ptrPkgBodyAck, iMaxPkgBodyAckLen ,0);
    s.SetDataBuffer(&dataBufferAck);
    s << archv3005Ack;
    if(s.GetErrorCode())
    {
        LOG( LOG_ERROR, "GetErrorCode error!msgType[%d]", pkgHeadReq.uiMsgType);
    }
    iPkgBodyAckLen  = dataBufferAck.GetDataSize();  //Ӧ����峤��
}

/**
 *@breif ��Ӧ�����װ
 *  Ӧ����н�������һ���ֶ�  retCode
 *@breif ptrPkgBodyAck Ӧ�������
 *@breif iPkgBodyAckLen Ӧ������峤��
 */
void makeMiniAck(int iErrorCode, char *ptrPkgBodyAck, int&iPkgBodyAckLen)
{
    UInt16 uiErrorcode = iErrorCode;
    memcpy(ptrPkgBodyAck, &uiErrorcode, sizeof(uiErrorcode));
    iPkgBodyAckLen = sizeof(uiErrorcode);
}

}

