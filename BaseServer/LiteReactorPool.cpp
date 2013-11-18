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
 *@brief 初始化
 *@param input iSize 对象池大小
 *@param input cb 生成对象回调函数
 *@return 0 成功         非0 失败
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
 *@brief 根据fd哈希值，获取对应的 对象
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
