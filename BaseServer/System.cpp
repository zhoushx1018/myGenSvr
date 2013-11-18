#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <fcntl.h>



#include "System.h"
#include "Log.h"

bool g_bIsStop = false;

long long CONNECTION_ID (int ip, unsigned short port, int fd) 
{	
	long long retval, tmp; 
	long long pf = 0x0000ffff & port;
	pf = (pf << 16) | fd;
	tmp = ip;
	retval = (tmp << 32) | pf; 
	return retval; 
}



void sigterm_handler(int signo)
{
	g_bIsStop = true;
}


void init_fdlimit ()
{
	struct rlimit rlim;

	/* allow core dump */
	rlim.rlim_cur = RLIM_INFINITY;
	rlim.rlim_max = RLIM_INFINITY;
	setrlimit(RLIMIT_CORE, &rlim);
}


int open_tcp_port_blk (const char* ip, short port, int backlog)
{
	int listenfd;
	struct sockaddr_in servaddr;

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons (port);
	inet_pton(AF_INET, ip, &servaddr.sin_addr);

	if ((listenfd = socket (AF_INET, SOCK_STREAM, 0)) == -1) 
	{ 
		LOG( LOG_ERROR , "socket error:%s\n", strerror(errno));
		return -1;
	} 
	int reuse_addr = 1;
	setsockopt (listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof reuse_addr);

	if (bind (listenfd, (struct sockaddr *)(&servaddr), sizeof(struct sockaddr)) == -1) 
	{ 
	    LOG( LOG_ERROR ,"bind %s:%d error:%s\n", ip, port, strerror(errno) );
		return -1;
	}
	
	if (listen (listenfd, backlog) == -1) 
	{ 
	    LOG( LOG_ERROR ,"listen error:%s\n", strerror(errno));
		return -1;
	}

	LOG( LOG_INFO, "open tcp %s:%d [ok]\n", ip, port );
	return listenfd;
}

/**
 *@brief ����������fd����Ϊ������
 */
int open_tcp_port_nblk (const char* ip, short port, int backlog)
{
	int	listenfd;
	struct sockaddr_in servaddr;

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons (port);
	inet_pton(AF_INET, ip, &servaddr.sin_addr);

	if ((listenfd = socket (AF_INET, SOCK_STREAM, 0)) == -1) 
	{ 
		LOG( LOG_ERROR,"socket error:%s\n", strerror(errno));
		return -1;
	} 
	int reuse_addr = 1;
	setsockopt (listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof reuse_addr);

	if (bind (listenfd, (struct sockaddr *)(&servaddr), sizeof(struct sockaddr)) == -1) 
	{ 
	    LOG( LOG_ERROR, "bind %s:%d error:%s\n", ip, port, strerror(errno) );
		return -1;
	}
	
	if (listen (listenfd, backlog) == -1) 
	{ 
	    LOG( LOG_ERROR,"listen error:%s\n", strerror(errno));
		return -1;
	}

	//setnonblocking
	setnonblocking(listenfd);
	
	LOG( LOG_INFO,"open tcp %s:%d [ok]\n",ip, port );
	return listenfd;
}

/**
 *@brief accept socket����
 *@param input sockfd �����Ƿ�������fd,����������� accept��
 *@return
 *  >0 Ϊfdֵ
 *  <=0 ʧ��,������Ӧ�ж�errno,�����ΪEAGAIN, EMFILE,��Ӧ���µ��ñ�����
 */
int accept_tcp_nblk (int sockfd, struct sockaddr_in *peer)
{
	socklen_t peer_size;
	int newfd;

	for ( ; ; ) 
	{
		peer_size = sizeof(struct sockaddr_in); 
		if ((newfd = accept(sockfd, (struct sockaddr *)peer, &peer_size)) < 0)
		{
			//�������ﵽ����
			if( errno == EMFILE )
			{
				//��ʱ�����޷���־����Ϊ�Ѿ���������ļ�������Ϊд��־��Ҫ�µ�fd������ʱ�Ѿ�û�и����fd
				fprintf( stderr, "EMFILE !!!!!! too many fd!!! accept error, lsfd[%d], errno[%d], strerror[%s]\n", sockfd, errno, strerror(errno) );
				return -1;
			}

			if (errno == EINTR)
				continue;         /* back to for () */

			if(errno == EAGAIN )
				return -1;

			LOG (LOG_ERROR,  "accept failed,listenfd=[%d], errno[%d] errmsg[%s]", sockfd, errno, strerror(errno));
			return -1;
		}

		break;
	}

	//set socket buffer
	int bufsize = 655350;
	setsockopt (newfd, SOL_SOCKET, SO_RCVBUF, (char *) bufsize, sizeof (int));
	setsockopt (newfd, SOL_SOCKET, SO_SNDBUF, (char *) bufsize, sizeof (int));

	// ���� SO_REUSEADDR
	int reuse_addr = 1;
	setsockopt (newfd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof reuse_addr);

	// ����	SO_LINGER
	linger m_sLinger;
	m_sLinger.l_onoff = 1; // (��closesocket()����,���ǻ�������û������ϵ�ʱ��������)
	m_sLinger.l_linger = 0; // (��������ʱ��Ϊ0��)
	setsockopt(newfd, SOL_SOCKET, SO_LINGER,(const char*)&m_sLinger, sizeof(linger));
	
	//setnonblocking
	setnonblocking(newfd);

/*	LOG( LOG_DEBUG,  "accept connection,fd=[%d],ip=[%s],port=[%d]",
		sockfd, inet_ntoa(peer->sin_addr), ntohs(peer->sin_port));
*/
	return newfd;
}


void setnonblocking(int sock)
{
	int opts;
	opts=fcntl(sock,F_GETFL);
	if(opts<0)
	{
		perror("fcntl(sock,GETFL)");
		LOG (LOG_ERROR,  "fcntl error" );
		exit(1);
	}
	
	opts = opts|O_NONBLOCK;
	if(fcntl(sock,F_SETFL,opts)<0)
	{
		perror("fcntl(sock,SETFL,opts)");
		LOG (LOG_ERROR,  "fcntl error" );
		exit(1);
	}
}





/*
 �������ø�ʽ:
 int fcntl( int fd, int cmd, struct * flock );
 ����:
 F_SETLK     ��ȡ���ͷ�flcok�ṹָ��������������ɹ���������EACCES��EAGAIN
 F_SETLKW    ��ȡ���ͷ�flock�ṹָ��������������ɹ�����ý��̱�����
 F_GETLK     ��ȡflock�ṹָ�������Ƿ���ڣ��Լ���ؽ���pid�������͵���Ϣ
 struct flock{   //�ļ��������ṹ
 short l_type;   //F_RDLCK, F_WRLCK, F_UNLCK �ֱ��Ƕ�����д��������
 short l_whence; //SEEK_SET, SEEK_CUR, SEEK_END �ļ�ƫ����ʼ����λ�ã��ļ�ͷ����ǰ���ļ�β
 off_t l_start;  //relative starting offset in bytes,ƫ���ֽ���
 off_t l_len;    //#bytes; 0 means until end-of-file, ���ĳ��ȣ�0Ϊ�����ļ�
 pid_t l_pid;    //pid returned by F_GETLK, ��ʹ��F_GETLK����ʱ������������̵�pid
 }; */
int lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len)
{
    struct flock lock;
    lock.l_type = type;
    lock.l_start = offset;
    lock.l_whence = whence;
    lock.l_len = len;
    return (fcntl(fd, cmd, &lock)); /* -1 upon error */
}

#define write_lock( fd, offset, whence, len ) \
    lock_reg( fd, F_SETLK, F_WRLCK, offset, whence, len )

/**
 *@brief ʹ���ļ���,ʹͬһʱ����ֻ������һ������ʵ��
 *@param char * appname    ���г��������,һ����argv[0]�õ�
 *@return  0 ִ�м����ɹ�         ��0 ִ�м���ʧ��
 */
int run_single(char * appname)
{
    int pidfd;
    char szFileName[1024];
    char szTmp[1024];

    //pid �ļ���
    memset(szFileName, 0, sizeof(szFileName));
    sprintf(szFileName, "./%s.pid", basename(appname));
    pidfd = open(szFileName, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if(pidfd < 0)
    {
        LOG(LOG_ERROR, "open error !can't create file lock file[%s]", szFileName);
        return -1;
    }

    if(write_lock(pidfd, 0, SEEK_SET, 0) < 0)
    {

        if(errno == EACCES || errno == EAGAIN)
        {
            LOG(LOG_ERROR, "can't run more than one process! lock file[%s], appname[%s] ", szFileName, appname);
        }
        else
        {
            LOG(LOG_ERROR, "write_lock error,can't lock the file! lock file[%s] ", szFileName);
        }

        //����ر�  pidfd ,�����ļ���ʧЧ,�޷���ֻ֤����һ������ʵ��
        return -1;
    }

    //��pid д���ļ�
    ftruncate(pidfd, 0); /*clear the file */
    snprintf(szTmp, sizeof(szTmp), "%ld\n", (long) (getpid()));
    write(pidfd, szTmp, strlen(szTmp));

    //����ر�  pidfd ,�����ļ���ʧЧ,�޷���ֻ֤����һ������ʵ��
    return 0;
}
