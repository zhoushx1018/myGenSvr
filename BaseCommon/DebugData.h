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
     Ŀ�ģ���16���Ƹ�ʽ��ʾĳ���ڴ������
     ������
     void *	input					�ڴ�ε���ʼ��ַ
     int	iSize					��ʾ����
     int 	iShowTail 				�Ƿ���ʾĩβ���ı��ַ� 0����ʾ  1��ʾ
     const char * PROGRAMNAME		Դ������
     int		LINE				��������
     ����ֵ��
     ��
     ˵����
     �ú�������ʾ��ʽ����Ultra Edit��ʾ16�������ݸ�ʽ
     ��ÿһ���ֽ���һ����λ��16����������ʾ:
     00000000h: 00 00 00 00 00 00 00 00 00 00 04 4d 00 00 00 00 ; M
     00000001h: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ;
     00000002h: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ;
     00000003h: 00 00 00 00 00 00 00 00 00 00 00 00 ;

     ����ShowHex���������ڵ��Գ����Ŀ�ģ�����б�Ҫ������ShowHex�ĺ������ڵ�
     Դ�������͵��õ�������ʾ����
     �����÷���
     int iTmp;
     ShowHex( &iTmp, sizeof(int), 0, __FILE__, __LINE__ );
     *********************************************************/
    static void ShowHex(void * input, int iSize, int iShowTail, const char * PROGRAMNAME, int LINE)
    {

        const int LINECHAR = 16; /*ÿ����ʾ�ֽ�����Ĭ��Ϊ16*/
        int iLine = 0; /*����*/
        int iCount = 0; /*ÿ����ʾ�ۼ�*/
        char cHead[1024]; /*��ͷ*/
        char cHex[1024]; /*16�����ַ�*/
        char cTail[1024]; /*�ı��ַ�*/
        char cTmp[8];
        int i = 0, iExit = 0;

        unsigned char * ptrChar = (unsigned char *) input;

        printf("\n File[%s] Line[%d] ShowHex, Bytes[%d]\n", PROGRAMNAME, LINE, iSize);
        printf("-------------------------------\n");
        if(iSize == 0)
            return;

        while(1)
        {
            /* �˳���� iExitΪ1 ����LINECHAR * iLine ���õ���iSize ��ʱ���˳�ѭ��*/
            if(iExit == 1 || (LINECHAR * iLine >= iSize))
                break;

            iCount = 0;
            strcpy(cHex, ":");
            strcpy(cTail, " ; ");

            for(i = 0; i < LINECHAR; i++)
            {
                /*�ж��Ƿ񳬳���ӡ��Χ*/
                if((++iCount + LINECHAR * iLine) > iSize)
                {
                    iExit = 1;
                    break;
                }

                /*���ַ���ʾ*/
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
     *	˵�����������õĴ�ӡ����
     *
     *	�÷���
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
     *@brief  ��16������ʾ��ʽ, ��ʾĳ���ڴ������
     *@param ptrSrc  �ڴ���ʼ��ַ
     *@param iLen    �ڴ泤��
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
     *@brief  ��16������ʾ��ʽ, ��ʾĳ���ڴ������
     *@param strSrc  string�ַ���
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
