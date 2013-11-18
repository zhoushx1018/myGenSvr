/*
 * LiteReactorBase.h
 *
 *  Created on: 2013-1-29
 *      Author: Administrator
 *
 * ʹ�� Reactorģʽ
 * ÿ��LiteReactorBase����ֻ�� һ�� event loop ��һ�� thread
 *
 * �ڲ�ʹ��epoll ��Ϊͳһ��io�¼�����ʱ�¼��Ĵ�������
 * �ⲿ�¼��������ӵ����������ͻ������ݵ�������ͨ���ܵ�ע�ᵽepoll���Ӷ���֤epoll�¼���ͳһ��
 *
 * ����
 *   �����ͻ������ݶ��У�
 *   ͨ���ܵ����¼��������ö��еĶ��¼�
 *
 * �¼���
 *   �ͻ��˶��¼�
 *   �ܵ����¼�:�ͻ��������ӵĵ���
 *   �ܵ����¼��������ͻ������ݵĵ���
 *   �ڲ�����( ��epoll��ʱ��
 *
 */

#ifndef LITEREACTOR_H_
#define LITEREACTOR_H_

#include <queue>
#include <tr1/unordered_map>

#include "Mutex.h"
#include "CommStruct.h"
#include "CommDef.h"



//�ͻ�������
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
    int _iSeq;               //�������к�,�½�����ʱ��ֵ, 1~N
    char _szIp[16];
    int _iPort;
    char *_ptrRecvBuff;
    UInt32 _uiRecvLen;          //�Ѿ���ȡ���ֽ���
    UInt64 _ulTimestamp;     //ʱ���
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
    int _connNotifySendFd;      //�ͻ����������¼� send fd
    int _connNotifyRecvFd;      //�ͻ����������¼� recv fd
    int _dataNotifySendFd;      //���͸��ͻ��������¼� send fd
    int _dataNotifyRecvFd;      //���͸��ͻ��������¼� recv fd

    int _ObjNO;                 //������ ,  1~N, �����ڶ����������
    int _connSeq;               //�ͻ����������к�,ÿ����һ������+1

    QUECONNFD _queConnFd;      //�ͻ���������fd�б�

    MutexLock _mutexConn; //�ͻ������������
    MutexLock _mutexData; //���͸��ͻ���������

    UMAPFDSOCKCONN _umapFdSockConn;// <�ͻ���fd,SockConn> unordered map/��tr1 ��hash map

};

typedef struct __stThreadArg
{
    int iThreadSeq;
    LiteReactorBase * service;
} stThreadArg;

#endif

