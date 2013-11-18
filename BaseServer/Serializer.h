#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <string>
#include "DataBuffer.h"
#include "CommDef.h"
#include "DebugData.h"
#include "DateTime.h"


using namespace std;


class BaseArchive;

//���л�������
class Serializer
{
	public:

		Serializer(DataBuffer *input, bool changeByteOrder = false);

		~Serializer();

		DataBuffer *GetDataBuffer() const;

		void SetDataBuffer( DataBuffer *input);

		int GetErrorCode();


		Serializer& operator<<(const Bool value);
		Serializer& operator>>(Bool& value);


		Serializer& operator<<(const Byte value);
		Serializer& operator>>(Byte& value);

		Serializer& operator<<(const SByte value);
		Serializer& operator>>(SByte& value);

		Serializer& operator<<(const Int16 value);
		Serializer& operator>>(Int16& value);

		
		Serializer& operator<<(const UInt16 value);
		Serializer& operator>>(UInt16& value);

		

		Serializer& operator<<(const Int32 value);
		Serializer& operator>>(Int32& value);

		Serializer& operator<<(const UInt32 value);
		Serializer& operator>>(UInt32& value);
		

		Serializer& operator<<(const Int64 value);
		Serializer& operator>>(Int64& value);

		Serializer& operator<<(const UInt64 value);
		Serializer& operator>>(UInt64& value);

		Serializer& operator<<(const Single value);
		Serializer& operator>>(Single& value);

		Serializer& operator<<(const Double value);
		Serializer& operator>>(Double& value);
		

		Serializer& operator<<( const string & value);
		Serializer& operator>>(string &  value);

		Serializer& operator<<(const BaseArchive& value);
		Serializer& operator>>(BaseArchive& value);

		Serializer& operator<<(const DateTime& value);
		Serializer& operator>>(DateTime& value);

	private:
		DataBuffer *_pBuffer;

		//�������л�����״̬  0 �ɹ�    ��0 ʧ��
		int	_iError;

		//�Ƿ�ת���ֽ���   true ��       false ��
		bool _ChangeByteOrfer;

};

#endif //SERIALIZER_H
