/*
 * LiteReactorBase.cpp
 *
 *  Created on: 2013-1-29
 *      Author: Administrator
 */
#include <sys/epoll.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#include "LiteReactorBase.h"
#include "CommDef.h"
#include "Log.h"
#include "System.h"
#include "Mutex.h"
#include "Log.h"
#include <gperftools/profiler.h>

using namespace std;


SockConn::SockConn()
{
    _iSockFd = 0;
    _iSeq = 0;
    memset(_szIp, 0, sizeof(_szIp));
    _iPort = 0;
    _ptrRecvBuff = NULL;
    _uiRecvLen = 0;
}

SockConn::~SockConn()
{
    if(NULL != _ptrRecvBuff)
    {
        delete[] _ptrRecvBuff;
        _ptrRecvBuff = NULL;
    }
}

/**
 *@brief ��ʼ�� ,��ȡip �˿ڵȵ�
 *@return 0�ɹ�      ��0 ʧ��
 */
int SockConn::Init(int fd, int seq)
{
    struct sockaddr_in CltAddr;
    socklen_t iCltLen = sizeof(CltAddr);

    _iSockFd = fd;
    _iSeq = seq;

    if(getpeername(_iSockFd, (sockaddr *) &CltAddr, &iCltLen) != 0)
    {
        return -1;
    }

    strncpy(_szIp, inet_ntoa(CltAddr.sin_addr), sizeof(_szIp) - 1);
    _iPort = ntohs(CltAddr.sin_port);

    //���ջ���
    _ptrRecvBuff = new char[MAXSOCKBUFF];
    _uiRecvLen = 0;
    if(NULL == _ptrRecvBuff)
    {
        return -2;
    }

    _ulTimestamp = time(NULL);

    return 0;
}


LiteReactorBase::LiteReactorBase()
{
    _epFd = 0;
    _connNotifySendFd = 0;
    _connNotifyRecvFd = 0;
    _dataNotifySendFd = 0;
    _dataNotifyRecvFd = 0;

    _connSeq = 0;
    _ObjNO = 0;
}

LiteReactorBase::~LiteReactorBase()
{
    g_bIsStop = true;

    _epFd = 0;
    _connNotifySendFd = 0;
    _connNotifyRecvFd = 0;
    _dataNotifySendFd = 0;
    _dataNotifyRecvFd = 0;

    UMAPFDSOCKCONN::iterator it;
    for(it = _umapFdSockConn.begin(); it != _umapFdSockConn.end(); it++)
    {
        delete it->second;
    }
}

int LiteReactorBase::Init(int iObjNO)
{
    int iRet = 0;
    _ObjNO = iObjNO;

    //epoll��ʼ��
    _epFd = epoll_create(MAXEPOLLFDNUM);
    if(_epFd < 0)
    {
        LOG(LOG_ERROR, "epoll_create error ! strerr[%s]", strerror(errno));
        return -1;
    }

    //��ʼ���ܵ�,���� notify fd
    int fds[2];
    iRet = pipe(fds);
    if(iRet)
    {
        LOG(LOG_ERROR, "pipe error ! strerr[%s]", strerror(errno));
        return -1;
    }
    _connNotifyRecvFd = fds[0];
    _connNotifySendFd = fds[1];

    iRet = pipe(fds);
    if(iRet)
    {
        LOG(LOG_ERROR, "pipe error ! strerr[%s]", strerror(errno));
        return -1;
    }
    _dataNotifyRecvFd = fds[0];
    _dataNotifySendFd = fds[1];

    //ע��epoll �¼�
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = _connNotifyRecvFd;
    epoll_ctl(_epFd, EPOLL_CTL_ADD, _connNotifyRecvFd, &ev);

    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = _dataNotifyRecvFd;
    epoll_ctl(_epFd, EPOLL_CTL_ADD, _dataNotifyRecvFd, &ev);

    //Ϊ���ݶ������,ʹ�ò����ṹ��
    //Ϊȷ���������˳���,���߳��Ծ��ܰ�ȫ���� �����ṹ, �����ṹ�Ӷ������ڴ�����ǴӺ���ջ
    pthread_t tid;
    stThreadArg * ptrArg = new stThreadArg;
    ptrArg->service = this;
    ptrArg->iThreadSeq = 0;

    iRet = pthread_create(&tid, NULL, LiteReactorBase::thread_main, ptrArg);
    if(iRet)
    {
        LOG(LOG_ERROR, "pthread_create error ! iRet[%d],strerr[%s]", iRet, strerror(errno));
        return -1;
    }

    return 0;
}

void * LiteReactorBase::thread_main(void *arg)
{
    stThreadArg threadArg;
    stThreadArg * ptrThreadArg = static_cast<stThreadArg*>(arg);

    memcpy(&threadArg, ptrThreadArg, sizeof(threadArg));
    delete ptrThreadArg;
    ptrThreadArg = NULL;

    threadArg.service->svc();
}

void LiteReactorBase::svc()
{
    pthread_detach(pthread_self());

    struct epoll_event events[MAXEVENTS];

    ProfilerStart("myprof.txt");

    while(0 == g_bIsStop)
    {
        int iTimeout = 600 * 1000;     //��ʱʱ�� ����λ����
        int nfds = epoll_wait(_epFd, events, MAXEVENTS, iTimeout);

        if(nfds < 0)     //������
        {
            LOG(LOG_WARN, "epoll_wait error!!! nfds[%d],errno[%d] \n", nfds, errno);
            continue;
        }

        if(0 == nfds)     //��ʱ����
        {
            LOG(LOG_DEBUG, "epoll_wait timeout!\n");
            //todo HandleTimeout();
            //todo ��ʱ��� _umapFdSockConn �Ƿ��г�ʱ����
            continue;
        }

        for(int n = 0; n < nfds; n++)
        {
            if(events[n].data.fd == _connNotifyRecvFd)     //�ͻ���������
            {
                HandleNewConn();
            }
            else if(events[n].data.fd == _dataNotifyRecvFd)  //���͸��ͻ����¼�
            {
                //HandleSendClient();
                LOG(LOG_DEBUG, "_dataNotifyRecvFd !fd[%d]", _dataNotifyRecvFd);
            }
            else    //�ɶ��¼�
            {
                HandleRead(events[n].data.fd);
            }

        }
    }

    ProfilerStop();
}

void LiteReactorBase::HandleNewConn()
{
    MutexLockGuard guardObj(_mutexConn);
    epoll_event ev;
    int iRet = 0;
    SockConn * ptrSockConn = NULL;

    int iConnFd = 0;
    while(!_queConnFd.empty())
    {
        iConnFd = _queConnFd.front();
        _queConnFd.pop();

        LOG(LOG_DEBUG, "_queConnFd front fd!iConnFd[%d]", iConnFd);

        //fd �Ϸ�У��
        if(iConnFd <= 0)
        {
            LOG(LOG_ERROR, "_queConnFd front fd error!iConnFd[%d]", iConnFd);
            continue;
        }

        //���ӿռ������
        ptrSockConn = new SockConn;
        if(NULL == ptrSockConn)
        {
            close(iConnFd);
            LOG(LOG_ERROR, "new error! close fd! iConnFd[%d]", iConnFd);
            continue;
        }

        //��ʼ������ֵ
        iRet = ptrSockConn->Init(iConnFd, ++_connSeq);
        if(iRet)
        {
            close(iConnFd);
            delete ptrSockConn;
            LOG(LOG_ERROR, "ptrSockConn->Init error!close fd!iRet[%d] iConnFd[%d]", iRet, iConnFd);
            continue;
        }

        //ע�� epoll �¼�
        ev.events = EPOLLIN | EPOLLET;
        ev.data.fd = iConnFd;
        epoll_ctl(_epFd, EPOLL_CTL_ADD, iConnFd, &ev);

        //��������
        _umapFdSockConn.insert(make_pair(iConnFd, ptrSockConn));

        LOG(LOG_INFO, "get new connect ok!fd[%d],ip[%s],port[%d]", ptrSockConn->_iSockFd, ptrSockConn->_szIp, ptrSockConn->_iPort);
    }
}

void LiteReactorBase::OnNewConn(int fd)
{
    if(fd <= 0)
    {
        LOG(LOG_ERROR, "OnNewConn fd error!fd[%d]", fd);
        return;
    }

    MutexLockGuard guardObj(_mutexConn);

    _queConnFd.push(fd);

    if(write(_connNotifySendFd, "", 1) != 1)
    {
        LOG(LOG_ERROR, "OnNewConn fd error!fd[%d]", fd);
        return;
    }
}

