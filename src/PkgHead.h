// ͨѶ���ṹ��ͨѶ��ͷ������ͨѶ����

#ifndef PKGHEAD_H
#define	PKGHEAD_H

#include "CommDef.h"
#include "CommStruct.h"
#pragma pack(1)



struct PkgHead
{
    UInt32 uiSrc;               //ϵͳ��Դ
    UInt32 uiExtSrc;            //��չϵͳ��Դ
    UInt32 uiVersion;           //�汾��
    UInt32 uiMsgType;           //��Ϣ����
    UInt32 uiSeq;               //��Ϣ�����к�
    UInt32 uiPkgBodyLen;        //���峤��

    void packet(char *sPkgOutput, int iNetByteOrder = 0);
    void unpacket(char *sPkgInput, int NetByteOrder = 0);

};

#pragma pack()

#endif

