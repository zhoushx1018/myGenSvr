#ifndef SYSTEM_H
#define SYSTEM_H

#include "CommDef.h"
#include <fcntl.h>


#define CONNECTION_FD(x)	(int)(x&0xFFFF)
#define CONNECTION_IP(x)	(x >> 32)
#define CONNECTION_PORT(x)	(int)((x >> 16)&0xFFFF)


extern long long CONNECTION_ID (int ip, unsigned short port, int fd); 

extern void init_fdlimit ();

extern bool g_bIsStop;

int open_tcp_port_nblk (const char* ip, short port, int backlog);

int open_tcp_port_blk (const char* ip, short port, int backlog);

int accept_tcp_nblk (int listenfd, struct sockaddr_in *peer);

void setnonblocking(int sock);

void sigterm_handler(int signo);

int lock_reg( int fd, int cmd, int type, off_t offset, int whence, off_t len );

int run_single(char * appname);

#endif

