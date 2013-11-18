/*
 * LiteReactorPool.h
 *
 *  Created on: 2013-1-31
 *      Author: Administrator
 */

#ifndef LITEREACTORPOOL_H_
#define LITEREACTORPOOL_H_

#include "LiteReactorBase.h"

#define LITEREACTORPOOL_MAX_SIZE  150

typedef LiteReactorBase* (*CREATE_CALLBACK) (); //生成对象回调函数

class LiteReactorPool
{
    typedef std::vector<LiteReactorBase*> VECLITEREACTOR;

public:
    LiteReactorPool();
    ~LiteReactorPool();
    static LiteReactorPool*instance();
    int Init(int iPoolSize, CREATE_CALLBACK cb);
    void release();
    LiteReactorBase * HashGet(int fd);

private:
    int _iPoolSize;
    VECLITEREACTOR _vecLiteReactor;

};

#define LITEREACTORPOOL_INSTANCE LiteReactorPool::instance()

#endif
