/*
 * LiteReactorPool.cpp
 *
 *  Created on: 2013-1-31
 *      Author: Administrator
 */

#include "LiteReactorPool.h"

using namespace std;

LiteReactorPool::LiteReactorPool()
{
    _iPoolSize = 0;
}

LiteReactorPool::~LiteReactorPool()
{
    release();
}

void LiteReactorPool::release()
{
    VECLITEREACTOR::iterator it;
    for(it = _vecLiteReactor.begin(); it != _vecLiteReactor.end(); it++)
    {
        if(*it)
        {
            delete (*it);
        }
    }
}

/**
 *@brief ��ʼ��
 *@param input iSize ����ش�С
 *@param input cb ���ɶ���ص�����
 *@return 0 �ɹ�         ��0 ʧ��
 */
int LiteReactorPool::Init(int iSize, CREATE_CALLBACK cb)
{
    int iRet = 0;
    if(iSize <= 0 || iSize > LITEREACTORPOOL_MAX_SIZE)
    {
        LOG(LOG_ERROR, "iSize error!iSize[%d]", iSize);
        return -1;
    }

    _iPoolSize = iSize;

    for(int i = 0; i < _iPoolSize; i++)
    {
        LiteReactorBase* ptrObj = cb();
        iRet = ptrObj->Init(i+1);
        if(iRet)
        {
            LOG(LOG_ERROR, "Init error!");
            return -1;
        }

        _vecLiteReactor.push_back(ptrObj);
    }

    return 0;
}

/**
 *@brief ����fd��ϣֵ����ȡ��Ӧ�� ����
 */
LiteReactorBase * LiteReactorPool::HashGet(int fd)
{
    assert(fd>0);

    return _vecLiteReactor.at(fd % _iPoolSize);
}

LiteReactorPool* LiteReactorPool::instance()
{
    static LiteReactorPool inst;
    return &inst;
}
