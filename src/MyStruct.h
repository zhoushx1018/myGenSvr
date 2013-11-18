/*
 * MyStruct.h
 *
 *  Created on: 2013-2-28
 *      Author: Administrator
 */

#ifndef MYSTRUCT_H_
#define MYSTRUCT_H_

typedef struct __stSystemParam
{
    char log_dir[128];
    char log_prename[32];
    int log_size;
    int log_num;
    short log_priority;

    int socket_timeout;
    int socket_bufsize;
    int backlog;
    int ip_connect_limit;
    int thread_num;

    char ip[16];
    u_short port;
    int type;
}stSystemParam;

extern stSystemParam g_sysParam;

#endif /* MYSTRUCT_H_ */
