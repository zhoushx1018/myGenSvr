#ifndef COMMDEF_H
#define COMMDEF_H



//epoll ���
#define MAXEPOLLFDNUM					5000									//epoll ��ע�����fd��
#define	MAXEVENTS							100										//ÿ��epoll�����ܴ�����¼���
#define MAXSOCKBUFF						65535									//socket ���ݻ���

#define MAX_TRYSENDNUM  20              //����Է��ʹ���


//��������
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

//ϵͳ������
#define ERR_SUCCESS								0			//�ɹ�
#define ERR_SYSTEM_TIMEOUT						1			//��Ϣ��ʱ          
#define ERR_SYSTEM_LARGEMSG						2			//��Ϣ������������
#define ERR_SYSTEM_SVRNOTEXISTS 				3			//Ӧ�÷��񲻴���
#define ERR_SYSTEM_SVRNOTSTART	    			4			//Ӧ�÷���δ����
#define ERR_SYSTEM_DBERROR 						5			//���ݿ����ʧ��    
#define ERR_SYSTEM_DBNORECORD					6			//���ݿ��¼������  
#define ERR_SYSTEM_SERERROR						7			//���л�����        
#define ERR_SYSTEM_SVRLOGINED					8			//�������ѵ�¼      
#define ERR_SYSTEM_SVRACCESS					9			//Ӧ�÷�����ʹ���(�޷��ɹ�������Ϣ)
#define ERR_SYSTEM_ROLENOTLOGIN		    		10		//��ɫδ��¼
#define ERR_SYSTEM_DATANOTEXISTS		    	11		//���ݲ�����
#define ERR_SYSTEM_PARAM						12		//��������
#define ERR_SYSTEM_DATAEXISTS					13		//�����Ѵ���

//Ӧ�ô�����
#define ERR_APP_ROLENOTEXISTS					1001	//��ɫ������
//��ɫ����
#define ERR_ROLE_LEVELNOTENOUGH				2001	//���𲻹�

#endif

