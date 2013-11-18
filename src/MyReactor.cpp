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
 *@brief 读取 sock数据到应用层
 *@return 0成功       非0失败  其中 -1接收数据失败   -2对端关闭连接
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
 *@brief 发送 sock数据
 *@return 0成功       非0失败
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
            // 当socket是非阻塞时,如返回EAGAIN,表示写缓冲队列已满
            // 返回 EINTR 表示中断，可继续写
            // 在这里做延时后再重试
            if(errno == EAGAIN || errno == EINTR)
            {
                iTrySendNum++;
                usleep(1000);

                //没有超过最大尝试发送次数,则继续发送
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
 *@brief 读取客户端 fd,请求包解析,业务处理及 处理结果的应答
 *  业务处理的入口
 */
void MyReactor::HandleRead(int iSockFd)
{
    LOG(LOG_DEBUG, "HandleRead ! Reactor No.[%d], iSockFd[%d]", _ObjNO, iSockFd);
    int iRet = 0;
    int iAckSize = 0;
    int iReqSize = 0;

    //查找对应的 sockConn信息
    UMAPFDSOCKCONN::iterator it;
    it = _umapFdSockConn.find(iSockFd);
    if(_umapFdSockConn.end()==it)
    {
        LOG(LOG_ERROR, "fd not found!fd[%d]", iSockFd);
        close(iSockFd);
        return;
    }
    SockConn* sockConn = it->second;

    //时间戳
    sockConn->_ulTimestamp = time(NULL);

    //读取sock数据到应用层
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

    //缓存数据中是否有包头
    if(sockConn->_uiRecvLen < (int) sizeof(PkgHead))
        return;

    //包头解析
    PkgHead pkgHead;
    memset(&pkgHead, 0, sizeof(pkgHead));
    pkgHead.unpacket(sockConn->_ptrRecvBuff, 1);

    //包头字段校验
    if(pkgHead.uiPkgBodyLen < 0 || (pkgHead.uiPkgBodyLen + sizeof(pkgHead)) > (MAXSOCKBUFF < (0xffff - 1) ? MAXSOCKBUFF : (0xffff - 1)))
    {
        LOG( LOG_ERROR, "usPkgLen error");
        close(sockConn->_iSockFd);
        _umapFdSockConn.erase(it);
        return;
    }

    //缓存数据中包头包体是否完整
    if(pkgHead.uiPkgBodyLen + sizeof(pkgHead) > sockConn->_uiRecvLen )
        return;

    //数据拷贝
    memcpy(_ptrReq, sockConn->_ptrRecvBuff, pkgHead.uiPkgBodyLen + sizeof(pkgHead) );
    iReqSize = pkgHead.uiPkgBodyLen + sizeof(pkgHead);


    //缓存去除数据包
    sockConn->_uiRecvLen -= pkgHead.uiPkgBodyLen + sizeof(pkgHead);
    memmove(sockConn->_ptrRecvBuff, sockConn->_ptrRecvBuff+pkgHead.uiPkgBodyLen + sizeof(pkgHead), sockConn->_uiRecvLen );

    //业务处理
    memset(_ptrAck, 0, MAXSOCKBUFF);
    service::DealService(_ptrReq, iReqSize, _ptrAck, iAckSize );

    //发送应答
    SendMsg(sockConn, _ptrAck, iAckSize);

}


