#ifndef COMMDEF_H
#define COMMDEF_H



//epoll 相关
#define MAXEPOLLFDNUM					5000									//epoll 关注的最大fd数
#define	MAXEVENTS							100										//每次epoll返回能处理的事件数
#define MAXSOCKBUFF						65535									//socket 数据缓存

#define MAX_TRYSENDNUM  20              //最大尝试发送次数


//数据类型
typedef bool Bool;
typedef char SByte;
typedef unsigned char Byte;
typedef short Int16;
typedef unsigned short UInt16;
typedef int Int32;
typedef unsigned int UInt32;
typedef long long Int64;
typedef unsigned long long UInt64;
typedef float Single;
typedef double Double;

typedef long LONG;
typedef unsigned long ULONG;

//系统错误定义
#define ERR_SUCCESS								0			//成功
#define ERR_SYSTEM_TIMEOUT						1			//消息超时          
#define ERR_SYSTEM_LARGEMSG						2			//消息包超过允许长度
#define ERR_SYSTEM_SVRNOTEXISTS 				3			//应用服务不存在
#define ERR_SYSTEM_SVRNOTSTART	    			4			//应用服务未启动
#define ERR_SYSTEM_DBERROR 						5			//数据库操作失败    
#define ERR_SYSTEM_DBNORECORD					6			//数据库记录不存在  
#define ERR_SYSTEM_SERERROR						7			//序列化错误        
#define ERR_SYSTEM_SVRLOGINED					8			//服务器已登录      
#define ERR_SYSTEM_SVRACCESS					9			//应用服务访问故障(无法成功发送消息)
#define ERR_SYSTEM_ROLENOTLOGIN		    		10		//角色未登录
#define ERR_SYSTEM_DATANOTEXISTS		    	11		//数据不存在
#define ERR_SYSTEM_PARAM						12		//参数错误
#define ERR_SYSTEM_DATAEXISTS					13		//数据已存在

//应用错误定义
#define ERR_APP_ROLENOTEXISTS					1001	//角色不存在
//角色错误
#define ERR_ROLE_LEVELNOTENOUGH				2001	//级别不够

#endif

