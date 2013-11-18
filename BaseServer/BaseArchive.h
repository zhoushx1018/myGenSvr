#ifndef BASEARCHIVE_H
#define BASEARCHIVE_H

#include <vector>
#include "Serializer.h"

using namespace std;


//�����л���Ļ���
//��Ҫ�������л����������ͣ����Ը���Ϊ����
class BaseArchive
{
public:
	//���л��Ĳ�������������ʵ��
		//isLoading	false  ���л���save���ݴ��,  �� << �Ĳ���
		//					true	 ���л���load���ݽ��,  �� >> �Ĳ���
	virtual Serializer& Serialize(Serializer &serializer,bool isLoading) = 0;
	virtual ~BaseArchive()
	{
	}
};


#define BEGIN_SERIAL_MAP()  Serializer& Serialize(Serializer &serializer,bool isLoading){

#define SERIAL_ENTRY(col) if(!serializer.GetErrorCode()){if(isLoading){serializer>>col;}else{serializer<<col;}} 

#define END_SERIAL_MAP() return serializer;}


#endif
