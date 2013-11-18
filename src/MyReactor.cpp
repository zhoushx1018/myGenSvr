/*
 * MyReactor.cpp
 *
 *  Created on: 2013-2-1
 *      Author: Administrator
 */

#include <errno.h>

#include "MyReactor.h"
#include "PkgHead.h"
#include "DebugData.h"
#include "Service.h"


/**
 *@brief ��ȡ sock���ݵ�Ӧ�ò�
 *@return 0�ɹ�       ��0ʧ��  ���� -1��������ʧ��   -2�Զ˹ر�����
 */
int MyReactor::RecvMsg(SockConn* sockConn)
{
    int recv_bytes = recv(sockConn->_iSockFd, sockConn->_ptrRecvBuff + sockConn->_uiRecvLen, MAXSOCKBUFF - sockConn->_uiRecvLen, 0);
    if(recv_bytes == -1)
    {
        if(errno == EWOULDBLOCK || errno == EAGAIN)
            return 0;
        else
        {
            return -1;
        }
    }
    else if(recv_bytes == 0)
    {
//        LOG( LOG_DEBUG, "recv 0 Byte!!peer close Conn!! connIP[%s],connPort[%d], fd=[%d], errmsg[%s], _recvLen[%d]", _ip, _port, sockfd, strerror(errno), _recvLen);
        return -2;
    }

    sockConn->_uiRecvLen += recv_bytes;
    sockConn->_ulTimestamp = time(NULL);

    return 0;
}

/**
 *@brief ���� sock����
 *@return 0�ɹ�       ��0ʧ��
 */
int MyReactor::SendMsg(SockConn* sockConn, char *ptrAck, int iAckSize)
{
    int tmp = 0;
    size_t total = iAckSize;
    const char *p = ptrAck;
    int iTrySendNum = 0;

    LOG( LOG_DEBUG, "req hex string[%s]", DEBUG_HEXSTRING(ptrAck, iAckSize).c_str());

    while(total)
    {
        tmp = send(sockConn->_iSockFd, p, total, 0);
        if(tmp < 0)
        {
            // ��socket�Ƿ�����ʱ,�緵��EAGAIN,��ʾд�����������
            // ���� EINTR ��ʾ�жϣ��ɼ���д
            // ����������ʱ��������
            if(errno == EAGAIN || errno == EINTR)
            {
                iTrySendNum++;
                usleep(1000);

                //û�г�������Է��ʹ���,���������
                if(iTrySendNum <= MAX_TRYSENDNUM)
                    continue;
            }

            iTrySendNum = 0;
            LOG( LOG_ERROR, "socket send error, errno[%d],errormsg[%s] ", errno, strerror(errno));
            return -1;
        }

        if(tmp == total)
        {
            iTrySendNum = 0;

            LOG( LOG_DEBUG, "SenMsg ok!");
            return 0;
        }

        total -= tmp;
        p += tmp;
    }

    return 0;

}

/**
 *@brief ��ȡ�ͻ��� fd,���������,ҵ���� ��������Ӧ��
 *  ҵ��������
 */
void MyReactor::HandleRead(int iSockFd)
{
    LOG(LOG_DEBUG, "HandleRead ! Reactor No.[%d], iSockFd[%d]", _ObjNO, iSockFd);
    int iRet = 0;
    int iAckSize = 0;
    int iReqSize = 0;

    //���Ҷ�Ӧ�� sockConn��Ϣ
    UMAPFDSOCKCONN::iterator it;
    it = _umapFdSockConn.find(iSockFd);
    if(_umapFdSockConn.end()==it)
    {
        LOG(LOG_ERROR, "fd not found!fd[%d]", iSockFd);
        close(iSockFd);
        return;
    }
    SockConn* sockConn = it->second;

    //ʱ���
    sockConn->_ulTimestamp = time(NULL);

    //��ȡsock���ݵ�Ӧ�ò�
    iRet = RecvMsg(sockConn);
    if(-2==iRet)
    {
        LOG(LOG_INFO, "peer close Conn!");
        close(sockConn->_iSockFd);
        _umapFdSockConn.erase(it);
        return;
    }
    else if(iRet)
    {
        LOG(LOG_ERROR, "RecvMsg error!");
        close(sockConn->_iSockFd);
        _umapFdSockConn.erase(it);
        return;
    }

    //�����������Ƿ��а�ͷ
    if(sockConn->_uiRecvLen < (int) sizeof(PkgHead))
        return;

    //��ͷ����
    PkgHead pkgHead;
    memset(&pkgHead, 0, sizeof(pkgHead));
    pkgHead.unpacket(sockConn->_ptrRecvBuff, 1);

    //��ͷ�ֶ�У��
    if(pkgHead.uiPkgBodyLen < 0 || (pkgHead.uiPkgBodyLen + sizeof(pkgHead)) > (MAXSOCKBUFF < (0xffff - 1) ? MAXSOCKBUFF : (0xffff - 1)))
    {
        LOG( LOG_ERROR, "usPkgLen error");
        close(sockConn->_iSockFd);
        _umapFdSockConn.erase(it);
        return;
    }

    //���������а�ͷ�����Ƿ�����
    if(pkgHead.uiPkgBodyLen + sizeof(pkgHead) > sockConn->_uiRecvLen )
        return;

    //���ݿ���
    memcpy(_ptrReq, sockConn->_ptrRecvBuff, pkgHead.uiPkgBodyLen + sizeof(pkgHead) );
    iReqSize = pkgHead.uiPkgBodyLen + sizeof(pkgHead);


    //����ȥ�����ݰ�
    sockConn->_uiRecvLen -= pkgHead.uiPkgBodyLen + sizeof(pkgHead);
    memmove(sockConn->_ptrRecvBuff, sockConn->_ptrRecvBuff+pkgHead.uiPkgBodyLen + sizeof(pkgHead), sockConn->_uiRecvLen );

    //ҵ����
    memset(_ptrAck, 0, MAXSOCKBUFF);
    service::DealService(_ptrReq, iReqSize, _ptrAck, iAckSize );

    //����Ӧ��
    SendMsg(sockConn, _ptrAck, iAckSize);

}


