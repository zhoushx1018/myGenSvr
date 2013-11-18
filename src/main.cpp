#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/epoll.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "LiteReactorPool.h"
#include "MyReactor.h"
#include "System.h"
#include "Log.h"
#include "MyStruct.h"
#include "IniFile.h"
#include <gperftools/profiler.h>

Log *g_pLog;
int g_iEpollFd;   //主线程 epoll fd
int g_iListenFd;  //主线程 监听 fd
IniFile g_iniFile;
stSystemParam g_sysParam;

/**
 *@brief 监听
 *  非阻塞
 *@return 0成功      非0  失败
 */
int Listen( int &lsfd, char * ip, u_short port )
{
    //外网监听，非阻塞
    lsfd = open_tcp_port_nblk( ip, port, g_sysParam.backlog);
    if( lsfd < 0 )
    {
        LOG(LOG_ERROR, " open_tcp_port_nblk error, ip[%s], port[%d], backlog[%d]", ip, port, g_sysParam.backlog );
        return -1;
    }

    //epoll 事件注册
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = lsfd;
    epoll_ctl( g_iEpollFd, EPOLL_CTL_ADD, lsfd, &ev );

    return 0;
}

/**
 *@brief accept连接
 *
 */

void HandleAccept()
{
    struct sockaddr_in peer;

    //内网连接
    while(1)
    {
        int connFd = accept_tcp_nblk(g_iListenFd, &peer);
        if(connFd <= 0)
        {
            //没有后续的tcp连接, 或者达到系统最大文件数
            if(EAGAIN == errno || EMFILE == errno)
                break;

            LOG( LOG_ERROR, " accept_tcp_nblk error");
            continue;
        }

        //todo:检查 IP 连接数
        //todo:检查 IP 白名单

        //将新连接转交
        LITEREACTORPOOL_INSTANCE->HashGet(connFd)->OnNewConn(connFd);
    }

    return;

}

/**
 *@brief 主线程的loop
 */
int MainLoop()
{
    //epoll初始化
    g_iEpollFd = epoll_create(MAXEPOLLFDNUM);
    if(g_iEpollFd < 0)
    {
        LOG(LOG_ERROR, "epoll_create error ! strerr[%s]", strerror(errno));
        return -1;
    }

    //外网监听初始化
    if(Listen(g_iListenFd, g_sysParam.ip, g_sysParam.port))
    {
        LOG( LOG_ERROR, " Listen error, g_sysParam.ip[%s], g_sysParam.port[%d]", g_sysParam.ip, g_sysParam.port);
        return -1;
    }

    struct epoll_event events[MAXEVENTS];
    while(0 == g_bIsStop)
    {
        int nfds = epoll_wait(g_iEpollFd, events, MAXEVENTS, -1);
        if(nfds <= 0)     //错误处理
        {
            LOG(LOG_ERROR, "epoll_wait error!!! nfds[%d],errno[%d] \n", nfds, errno);
            continue;
        }

        for(int n = 0; n < nfds; n++)
        {
            if(events[n].data.fd == g_iListenFd)     //客户端新连接
            {
                LOG(LOG_DEBUG, "accept fd event! fd[%d]\n", events[n].data.fd);

                HandleAccept();
                continue;
            }
            else    //错误
            {
                LOG(LOG_ERROR, "unknow fd! fd[%d]\n", events[n].data.fd);
                continue;
            }

        }
    }

    return 0;
}

LiteReactorBase* newMyReactor()
{
    return new MyReactor();
}

#define INIFILE_READ(SEGMENT, KEY, VALUE) \
    do{\
        if(g_iniFile.read(SEGMENT,KEY,VALUE))\
        {\
            fprintf(stderr, "g_iniFile.read error,segment[%s], key[%s] \n", SEGMENT,KEY ); \
            return -1; \
        }\
    }while(0) \


int ReadConfig(const char* config_file)
{
    if(!g_iniFile.open(config_file))
    {
        printf("open config file:%s failed\n", config_file);
        return -1;
    }

    memset(&g_sysParam, 0x0, sizeof(g_sysParam));

    string strTmp;
    INIFILE_READ("logger", "LOG_DIR_PATH", strTmp);
    strcpy(g_sysParam.log_dir, strTmp.c_str());

    INIFILE_READ("logger", "LOG_MAX_SIZE", strTmp);
    g_sysParam.log_size = atoi(strTmp.c_str());

    INIFILE_READ("logger", "LOG_MAX_NUM", strTmp);
    g_sysParam.log_num = atoi(strTmp.c_str());

    INIFILE_READ("logger", "LOG_PRIORITY", strTmp);
    g_sysParam.log_priority = atoi(strTmp.c_str());

    INIFILE_READ("misc", "SOCKET_TIMEOUT", strTmp);
    g_sysParam.socket_timeout = atoi(strTmp.c_str());

    INIFILE_READ("misc", "CONNECT_LIMIT", strTmp);
    g_sysParam.ip_connect_limit = atoi(strTmp.c_str());

    INIFILE_READ("misc", "SOCKET_BUFSIZE", strTmp);
    g_sysParam.socket_bufsize = atoi(strTmp.c_str());

    INIFILE_READ("misc", "THREAD_NUM", strTmp);
    g_sysParam.thread_num = atoi(strTmp.c_str());

    INIFILE_READ("misc", "ACCEPT_BACKLOG", strTmp);
    g_sysParam.backlog = atoi(strTmp.c_str());

    INIFILE_READ("PORT_OUT", "IP", strTmp);
    strcpy(g_sysParam.ip, strTmp.c_str());

    INIFILE_READ("PORT_OUT", "PORT", strTmp);
    g_sysParam.port = atoi(strTmp.c_str());

    return 0;
}

void PrintConfig()
{
    printf("LOG_DIR:%s\n", g_sysParam.log_dir);
    printf("LOG_MAX_SIZE:%d\n", g_sysParam.log_size);
    printf("LOG_MAX_NUM:%d\n", g_sysParam.log_num);
    printf("LOG_PRIORITY:%d\n", g_sysParam.log_priority);
    printf("LOG_PRENAME:%s\n\n", g_sysParam.log_prename);

    printf("SOCKET_TIMEOUT:%d\n", g_sysParam.socket_timeout);
    printf("SOCKET_BUFSIZE:%d\n", g_sysParam.socket_bufsize);
    printf("SOCKET_backlog:%d\n\n", g_sysParam.backlog);
}

int main(int argc,char* argv[])
{
    int iRet = 0;



	if (argc < 2)
	{
		fprintf( stderr,"usage: %s config_file", argv[0]);
		return -1;
	}

	//读取配置文件
	char szIiniFile[256] = {0};
	strncpy (szIiniFile ,argv[1], sizeof (szIiniFile));

	if(access (szIiniFile, F_OK) != 0)
	{
		fprintf( stderr,"get config file:[%s] error", szIiniFile);
		return -1;
	}
	
	//读取配置参数
    if(ReadConfig(szIiniFile) != 0)
    {
        fprintf( stderr,"initalize config file:[%s] error\n", szIiniFile);
        exit(1);
    }

	//日志初始化
	g_pLog = new Log (g_sysParam.log_dir, g_sysParam.log_priority, g_sysParam.log_size, g_sysParam.log_num, g_sysParam.log_prename);
	
	//单实例进程
    iRet = run_single(argv[0]);
    if(iRet)
    {
        LOG(LOG_ERROR, "run_single error\n");
        exit(1);
    }

	//环境设置
	init_fdlimit ();

	//信号处理
    struct sigaction sa;
    sigset_t sset;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sigterm_handler;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);

    signal(SIGPIPE, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);

    sigemptyset(&sset);
    sigaddset(&sset, SIGSEGV);
    sigaddset(&sset, SIGBUS);
    sigaddset(&sset, SIGABRT);
    sigaddset(&sset, SIGILL);
    sigaddset(&sset, SIGCHLD);
    sigaddset(&sset, SIGFPE);
    sigprocmask(SIG_UNBLOCK, &sset, &sset);

    //守护进程
//    daemon(1, 1);

    // MyReactor 对象池
    iRet = LITEREACTORPOOL_INSTANCE->Init( g_sysParam.thread_num, newMyReactor);
    if(iRet)
    {
        LOG(LOG_ERROR,"LITEREACTORPOOL_INSTANCE Init error!");
        exit(1);
    }

    LOG(LOG_INFO, "server init ok, MainLoop start!");

    //监听及 accept,  如果有返回则程序退出
    iRet = MainLoop();
    if(iRet)
    {
        LOG(LOG_WARN, "MainLoop exit!!!!!!!!!!!!!!");
        exit(1);
    }



	return 0;
}


