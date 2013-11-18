// 通讯包结构、通讯包头、个别通讯包体

#ifndef PKGHEAD_H
#define	PKGHEAD_H

#include "CommDef.h"
#include "CommStruct.h"
#pragma pack(1)



struct PkgHead
{
    UInt32 uiSrc;               //系统来源
    UInt32 uiExtSrc;            //扩展系统来源
    UInt32 uiVersion;           //版本号
    UInt32 uiMsgType;           //消息类型
    UInt32 uiSeq;               //消息的序列号
    UInt32 uiPkgBodyLen;        //包体长度

    void packet(char *sPkgOutput, int iNetByteOrder = 0);
    void unpacket(char *sPkgInput, int NetByteOrder = 0);

};

#pragma pack()

#endif

