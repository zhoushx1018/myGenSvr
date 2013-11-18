#ifndef DEBUGDATA_H
#define	DEBUGDATA_H

#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <string>

class DebugData
{
public:

    /*******************************************************
     目的：以16进制格式显示某段内存的内容
     参数：
     void *	input					内存段的起始地址
     int	iSize					显示长度
     int 	iShowTail 				是否显示末尾的文本字符 0不显示  1显示
     const char * PROGRAMNAME		源程序名
     int		LINE				程序行数
     返回值：
     无
     说明：
     该函数的显示格式参照Ultra Edit显示16进制数据格式
     即每一个字节用一个两位的16进制数来显示:
     00000000h: 00 00 00 00 00 00 00 00 00 00 04 4d 00 00 00 00 ; M
     00000001h: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ;
     00000002h: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ;
     00000003h: 00 00 00 00 00 00 00 00 00 00 00 00 ;

     由于ShowHex更多是用于调试程序的目的，因此有必要将调用ShowHex的函数所在的
     源程序名和调用的行数显示出来
     典型用法：
     int iTmp;
     ShowHex( &iTmp, sizeof(int), 0, __FILE__, __LINE__ );
     *********************************************************/
    static void ShowHex(void * input, int iSize, int iShowTail, const char * PROGRAMNAME, int LINE)
    {

        const int LINECHAR = 16; /*每行显示字节数，默认为16*/
        int iLine = 0; /*行数*/
        int iCount = 0; /*每行显示累计*/
        char cHead[1024]; /*行头*/
        char cHex[1024]; /*16进制字符*/
        char cTail[1024]; /*文本字符*/
        char cTmp[8];
        int i = 0, iExit = 0;

        unsigned char * ptrChar = (unsigned char *) input;

        printf("\n File[%s] Line[%d] ShowHex, Bytes[%d]\n", PROGRAMNAME, LINE, iSize);
        printf("-------------------------------\n");
        if(iSize == 0)
            return;

        while(1)
        {
            /* 退出标记 iExit为1 或者LINECHAR * iLine 正好等于iSize 的时候，退出循环*/
            if(iExit == 1 || (LINECHAR * iLine >= iSize))
                break;

            iCount = 0;
            strcpy(cHex, ":");
            strcpy(cTail, " ; ");

            for(i = 0; i < LINECHAR; i++)
            {
                /*判断是否超出打印范围*/
                if((++iCount + LINECHAR * iLine) > iSize)
                {
                    iExit = 1;
                    break;
                }

                /*按字符显示*/
                sprintf(cTmp, " %02x", *ptrChar);
                strcat(cHex, cTmp);
                sprintf(cTmp, "%c", *ptrChar);
                strcat(cTail, cTmp);
                ptrChar++;
            }

            sprintf(cHead, "%08xh", iLine);
            strcat(cHead, cHex);

            if(iShowTail)
                strcat(cHead, cTail);
            printf("%s-\n", cHead);

            iLine++;
        };

        printf("--\n--\n--\n");
    }

    /*
     *	说明：供调试用的打印函数
     *
     *	用法：
     *		debug_printf( __FILE__, __LINE__, "int[%d] string [%s], iTmp, szTmp );
     */
    static void Printf(const char * filename, int line, const char * fmt, ... )
    {
        char szBuff[1024];
        int nSize = 0;
        memset( szBuff, 0 ,sizeof(szBuff) );

        sprintf( szBuff, "\n[%s] Line.[%d]-->", filename, line );
        nSize = strlen( szBuff );

        va_list ap;
        va_start( ap, fmt );

        vsnprintf( szBuff+nSize, sizeof( szBuff )-nSize-1, fmt, ap );
        strcat( szBuff, "\n" );

        fprintf( stderr, "%s", szBuff );

        va_end(ap);
        return;
    }

    /**
     *@brief  以16进制显示格式, 显示某段内存的内容
     *@param ptrSrc  内存起始地址
     *@param iLen    内存长度
     */
    static std::string HexString(const char *ptrSrc, int iLen)
    {
        char szTmp[128];
        std::string strRetVal;

        memset(szTmp, 0, sizeof(szTmp));
        sprintf( szTmp, "HexString, Bytes[%d] \n", iLen);

        strRetVal += std::string(szTmp);
        for (int i = 0; i < iLen; i++)
        {
            sprintf (szTmp, "%02x", (unsigned char)(ptrSrc[i]));
            if( 0 == (i+1)%16 )
                strcat( szTmp, "\n" );
            else
                strcat( szTmp, " ");

            strRetVal += std::string(szTmp);
        }

        return strRetVal;
    }

    /**
     *@brief  以16进制显示格式, 显示某段内存的内容
     *@param strSrc  string字符串
     */
    static std::string HexString( const std::string& strSrc )
    {
        return HexString( strSrc.c_str(), (int)strSrc.size() );
    }

};

//#ifdef DEBUG

#define DEBUG_PRINTF(format...) DebugData::Printf(__FILE__,__LINE__,format)
#define DEBUG_SHOWHEX(ptr,size,flag) DebugData::ShowHex(ptr,size,flag, __FILE__, __LINE__)
#define DEBUG_HEXSTRING DebugData::HexString

//#else
//#define DEBUG_PRINTF(format...) NULL;
//#define DEBUG_SHOWHEX(ptr,size,flag) NULL;
//#define DEBUG_HEXSTRING NULL;
//#endif


#endif
