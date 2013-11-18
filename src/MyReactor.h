/*
 * MyReactor.h
 *
 *  Created on: 2013-2-1
 *      Author: Administrator
 */

#ifndef MYREACTOR_H_
#define MYREACTOR_H_

#include "LiteReactorBase.h"

class MyReactor: public LiteReactorBase
{
public:
    MyReactor()
    {
        _ptrReq = new char[MAXSOCKBUFF];
        assert(_ptrReq);

        _ptrAck = new char[MAXSOCKBUFF];
        assert(_ptrAck);
    }
    virtual ~MyReactor()
    {
        if(_ptrReq)
            delete[] _ptrReq;

        if(_ptrAck)
            delete[] _ptrAck;
    }

    virtual void HandleRead(int iFd);

private:
    int RecvMsg(SockConn* sockConn);
    int SendMsg(SockConn* sockConn, char *ptrAck, int iAckSize);
    char * _ptrReq;
    char * _ptrAck;
};


#endif /* MYREACTOR_H_ */
