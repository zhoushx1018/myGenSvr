/*
 * MyArchv.h
 *  ���л���
 *  Created on: 2013-2-21
 *      Author: Administrator
 */

#ifndef MYARCHV_H_
#define MYARCHV_H_

#include "Serializer.h"
#include "BaseArchive.h"
#include "List.h"

class Archv3005Req: public BaseArchive
{

public:
    //��Ա��������ֵ
    //	�� string���͡���Listģ�����͵ĳ�Ա���������鶼���ϳ�ֵ
    Archv3005Req() :
        opType(0)
    {
    }

public:
    //��Ա����
    string userNumber;
    Byte opType;

    //��Ա�������л�������Serializer�ཫ���ֶ�˳��������л�
    BEGIN_SERIAL_MAP()
        SERIAL_ENTRY(userNumber)
        SERIAL_ENTRY(opType)

    END_SERIAL_MAP()
};


class Archv3005Ack: public BaseArchive
{

public:
    //��Ա��������ֵ
    //  �� string���͡���Listģ�����͵ĳ�Ա���������鶼���ϳ�ֵ
    Archv3005Ack() :
        errorCode(0)
    {
    }

public:
    //��Ա����
    UInt16 errorCode;
    string userNumber;
    string userAlias;

    //��Ա�������л�������Serializer�ཫ���ֶ�˳��������л�
    BEGIN_SERIAL_MAP()
        SERIAL_ENTRY(errorCode)
        SERIAL_ENTRY(userNumber)
        SERIAL_ENTRY(userAlias)
    END_SERIAL_MAP()
};



#endif /* MYARCHV_H_ */
