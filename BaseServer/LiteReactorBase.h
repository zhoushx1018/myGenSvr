/*
 * LiteReactorBase.h
 *
 *  Created on: 2013-1-29
 *      Author: Administrator
 *
 * 使用 Reactor模式
 * 每个LiteReactorBase对象只有 一个 event loop ，一个 thread
 *
 * 内部使用epoll 作为统一的io事件、定时事件的触发器；
 * 外部事件（新连接到来、发往客户端数据到来），通过管道注册到epoll，从而保证epoll事件的统一；
 *
 * 队列
 *   发往客户端数据队列；
 *   通过管道读事件，触发该队列的读事件
 *
 * 事件：
 *   客户端读事件
 *   管道读事件:客户端新连接的到来
 *   管道读事件：发往客户端数据的到来
 *   内部唤醒( 即epoll超时）
 *
 */

#ifndef LITEREACTOR_H_
#define LITEREACTOR_H_

#include <queue>
#include <tr1/unordered_map>

#include "Mutex.h"
#include "CommStruct.h"
#include "CommDef.h"



//客户端连接
class SockConn
{
    //noncopyable
private:
    SockConn(const SockConn&);
    const SockConn& operator=(const SockConn&);

public:
    SockConn();
    ~SockConn();
    int Init(int fd, int seq);

public:
    int _iSockFd;
    int _iSeq;               //连接序列号,新建连接时候赋值, 1~N
    char _szIp[16];
    int _iPort;
    char *_ptrRecvBuff;
    UInt32 _uiRecvLen;          //已经读取的字节数
    UInt64 _ulTimestamp;     //时间戳
};


class LiteReactorBase
{
public:
    typedef std::queue<int> QUECONNFD;
    typedef std::tr1::unordered_map<int,SockConn*> UMAPFDSOCKCONN;

public:
    LiteReactorBase();
    virtual ~LiteReactorBase();
    int Init(int iObjNO);
    static void * thread_main(void *arg);
    void OnNewConn(int fd);

protected:
    void svc();
    void HandleNewConn();
    virtual void HandleRead(int iFd) = 0;

protected:
    int _epFd;                  //epoll fd
    int _connNotifySendFd;      //客户端新连接事件 send fd
    int _connNotifyRecvFd;      //客户端新连接事件 recv fd
    int _dataNotifySendFd;      //发送给客户端数据事件 send fd
    int _dataNotifyRecvFd;      //发送给客户端数据事件 recv fd

    int _ObjNO;                 //对象编号 ,  1~N, 方便在对象池中区分
    int _connSeq;               //客户端连接序列号,每新来一个连接+1

    QUECONNFD _queConnFd;      //客户端新连接fd列表

    MutexLock _mutexConn; //客户端连接相关锁
    MutexLock _mutexData; //发送给客户端数据锁

    UMAPFDSOCKCONN _umapFdSockConn;// <客户端fd,SockConn> unordered map/即tr1 的hash map

};

typedef struct __stThreadArg
{
    int iThreadSeq;
    LiteReactorBase * service;
} stThreadArg;

#endif

