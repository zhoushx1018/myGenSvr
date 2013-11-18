/*
 * MyArchv.h
 *  序列化类
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
    //成员变量赋初值
    //	非 string类型、非List模板类型的成员变量，建议都赋上初值
    Archv3005Req() :
        opType(0)
    {
    }

public:
    //成员变量
    string userNumber;
    Byte opType;

    //成员变量序列化操作，Serializer类将按字段顺序进行序列化
    BEGIN_SERIAL_MAP()
        SERIAL_ENTRY(userNumber)
        SERIAL_ENTRY(opType)

    END_SERIAL_MAP()
};


class Archv3005Ack: public BaseArchive
{

public:
    //成员变量赋初值
    //  非 string类型、非List模板类型的成员变量，建议都赋上初值
    Archv3005Ack() :
        errorCode(0)
    {
    }

public:
    //成员变量
    UInt16 errorCode;
    string userNumber;
    string userAlias;

    //成员变量序列化操作，Serializer类将按字段顺序进行序列化
    BEGIN_SERIAL_MAP()
        SERIAL_ENTRY(errorCode)
        SERIAL_ENTRY(userNumber)
        SERIAL_ENTRY(userAlias)
    END_SERIAL_MAP()
};



#endif /* MYARCHV_H_ */
