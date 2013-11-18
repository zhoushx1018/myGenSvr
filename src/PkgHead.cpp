#include "PkgHead.h"
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>

#include "DebugData.h"


/*
 *@brief 序列化
 *@param input/output sPkgOutput 序列化目标指针
 *@param int iF
 */
void PkgHead::packet(char *sPkgOutput, int iNetByteOrder)
{
    int iTmpLen = 0;
    UInt32 iTmpData = 0;

    if(iNetByteOrder) //转换字节序
    {
        iTmpData = htonl(uiSrc);
        memcpy(sPkgOutput + iTmpLen, &iTmpData, sizeof(iTmpData));
        iTmpLen += sizeof(iTmpData);

        iTmpData = htonl(uiExtSrc);
        memcpy(sPkgOutput + iTmpLen, &iTmpData, sizeof(iTmpData));
        iTmpLen += sizeof(iTmpData);

        iTmpData = htonl(uiVersion);
        memcpy(sPkgOutput + iTmpLen, &iTmpData, sizeof(iTmpData));
        iTmpLen += sizeof(iTmpData);

        iTmpData = htonl(uiMsgType);
        memcpy(sPkgOutput + iTmpLen, &iTmpData, sizeof(iTmpData));
        iTmpLen += sizeof(iTmpData);

        iTmpData = htonl(uiSeq);
        memcpy(sPkgOutput + iTmpLen, &iTmpData, sizeof(iTmpData));
        iTmpLen += sizeof(iTmpData);

        iTmpData = htonl(uiPkgBodyLen);
        memcpy(sPkgOutput + iTmpLen, &iTmpData, sizeof(iTmpData));
        iTmpLen += sizeof(iTmpData);
    }
    else //不转换字节序
    {

        memcpy(sPkgOutput + iTmpLen, &uiSrc, sizeof(uiSrc));
        iTmpLen += sizeof(uiSrc);

        memcpy(sPkgOutput + iTmpLen, &uiExtSrc, sizeof(uiExtSrc));
        iTmpLen += sizeof(uiExtSrc);

        memcpy(sPkgOutput + iTmpLen, &uiVersion, sizeof(uiVersion));
        iTmpLen += sizeof(uiVersion);

        memcpy(sPkgOutput + iTmpLen, &uiMsgType, sizeof(uiMsgType));
        iTmpLen += sizeof(uiMsgType);

        memcpy(sPkgOutput + iTmpLen, &uiSeq, sizeof(uiSeq));
        iTmpLen += sizeof(uiSeq);

        memcpy(sPkgOutput + iTmpLen, &uiPkgBodyLen, sizeof(uiPkgBodyLen));
        iTmpLen += sizeof(uiPkgBodyLen);
    }

}

/*
 *@brief 反序列化
 *@param input sPkgInput 序列化目标指针
 */
void PkgHead::unpacket(char *sPkgInput, int iNetByteOrder)
{
    int iTmpLen = 0;

    memcpy(&uiSrc, sPkgInput + iTmpLen, sizeof(uiSrc));
    iTmpLen += sizeof(uiSrc);

    memcpy(&uiExtSrc, sPkgInput + iTmpLen, sizeof(uiExtSrc));
    iTmpLen += sizeof(uiExtSrc);

    memcpy(&uiVersion, sPkgInput + iTmpLen, sizeof(uiVersion));
    iTmpLen += sizeof(uiVersion);

    memcpy(&uiMsgType, sPkgInput + iTmpLen, sizeof(uiMsgType));
    iTmpLen += sizeof(uiMsgType);

    memcpy(&uiSeq, sPkgInput + iTmpLen, sizeof(uiSeq));
    iTmpLen += sizeof(uiSeq);

    memcpy(&uiPkgBodyLen, sPkgInput + iTmpLen, sizeof(uiPkgBodyLen));
    iTmpLen += sizeof(uiPkgBodyLen);

    //转换字节序
    if(iNetByteOrder)
    {
        uiSrc = ntohl(uiSrc);
        uiExtSrc = ntohl(uiExtSrc);
        uiVersion = ntohl(uiVersion);
        uiMsgType = ntohl(uiMsgType);
        uiSeq = ntohl(uiSeq);
        uiPkgBodyLen = ntohl(uiPkgBodyLen);
    }

}

