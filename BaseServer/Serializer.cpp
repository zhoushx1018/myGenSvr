#include <arpa/inet.h>
#include "BaseArchive.h"
#include "Serializer.h"

Serializer::Serializer(DataBuffer *input, bool changeByteOrder)
	:_pBuffer(input),_iError(0),_ChangeByteOrfer(changeByteOrder)
{
}

Serializer::~Serializer()
{
}

DataBuffer *Serializer::GetDataBuffer() const
{
	return _pBuffer;
}

void Serializer::SetDataBuffer( DataBuffer *input)
{
	_pBuffer = input;
}
	
int Serializer::GetErrorCode()
{
	return _iError;
}


Serializer& Serializer::operator<<(const Bool value)
{
	bool v = value;
	_iError = _pBuffer->Write(&v,1);
	return (*this);
}

Serializer& Serializer::operator >>(Bool &value)
{
	bool v;
	_iError = _pBuffer->Read(&v,1);
	value = v;
	
	return (*this);
	
}

Serializer& Serializer::operator<<(const Byte value)
{
	_iError = _pBuffer->Write(&value,1);
	return (*this);
}

Serializer& Serializer::operator>>(Byte& value)
{
	_iError = _pBuffer->Read(&value,1);
	return (*this);
}

Serializer& Serializer::operator<<(const SByte value)
{
	_iError = _pBuffer->Write(&value,1);
	return (*this);
}

Serializer& Serializer::operator>>(SByte& value)
{
	_iError = _pBuffer->Read(&value,1);
	return (*this);
}

Serializer& Serializer::operator<<(const Int16 value)
{
    Int16 sTmp = value;

    //转换字节序
    if(_ChangeByteOrfer)
    {
        sTmp = htons(sTmp);
    }
	_iError = _pBuffer->Write(&sTmp,sizeof(Int16));

	return (*this);
}

Serializer& Serializer::operator>>(Int16& value)
{
	_iError = _pBuffer->Read(&value,sizeof(Int16));

	//转换字节序
	if(_ChangeByteOrfer)
	{
	    value = ntohs(value);
	}

	return (*this);
}


Serializer& Serializer::operator<<(const UInt16 value)
{
    UInt16 sTmp = value;

    //转换字节序
    if(_ChangeByteOrfer)
    {
        sTmp = htons(sTmp);
    }

	_iError = _pBuffer->Write(&sTmp,sizeof(UInt16));
	return (*this);
}

Serializer& Serializer::operator>>(UInt16& value)
{
	_iError = _pBuffer->Read(&value,sizeof(UInt16));

	//转换字节序
	if(_ChangeByteOrfer)
	{
	    value = ntohs(value);
	}

	return (*this);
}

Serializer& Serializer::operator<<(const Int32 value)
{
    Int32 sTmp = value;

    //转换字节序
    if(_ChangeByteOrfer)
    {
        sTmp = htonl(sTmp);
    }

	_iError = _pBuffer->Write(&sTmp,sizeof(Int32));
	return (*this);
}

Serializer& Serializer::operator>>(Int32& value)
{
	_iError = _pBuffer->Read(&value,sizeof(Int32));

    //转换字节序
    if(_ChangeByteOrfer)
    {
        value = ntohl(value);
    }


	return (*this);
}

Serializer& Serializer::operator<<(const UInt32 value)
{
    UInt32 sTmp = value;

    //转换字节序
    if(_ChangeByteOrfer)
    {
        sTmp = htonl(sTmp);
    }
	_iError = _pBuffer->Write(&sTmp,sizeof(UInt32));
	return (*this);
}

Serializer& Serializer::operator>>(UInt32& value)
{
	_iError = _pBuffer->Read(&value,sizeof(UInt32));

	//转换字节序
	if(_ChangeByteOrfer)
	{
	    value = ntohl(value);
	}

	return (*this);
}

Serializer& Serializer::operator<<(const Int64 value)
{
	_iError = _pBuffer->Write(&value,sizeof(Int64));
	return (*this);
}


Serializer& Serializer::operator>>(Int64& value)
{
	_iError = _pBuffer->Read(&value,sizeof(Int64));
	return (*this);
}

Serializer& Serializer::operator<<(const UInt64 value)
{
	_iError = _pBuffer->Write(&value,sizeof(UInt64));
	return (*this);
}

Serializer& Serializer::operator>>(UInt64& value)
{
	_iError = _pBuffer->Read(&value,sizeof(UInt64));
	return (*this);
}

Serializer& Serializer::operator<<(const Single value)
{
	_iError = _pBuffer->Write(&value,sizeof(Single));
	return (*this);
}

Serializer& Serializer::operator>>(Single& value)
{
	_iError = _pBuffer->Read(&value,sizeof(Single));
	return (*this);
}


Serializer& Serializer::operator<<(const Double value)
{
	_iError = _pBuffer->Write(&value,sizeof(Double));
	return (*this);
}


Serializer& Serializer::operator>>(Double& value)
{
	_iError = _pBuffer->Read(&value,sizeof(Double));
	return (*this);
}

Serializer& Serializer::operator<<(const string& value)
{//字符类型  相当于先 strcpy， 再追加一个 '\0'

//DEBUG_SHOWHEX( _pBuffer->GetReadPtr(), _pBuffer->GetDataSize(), 0 );
    _iError = _pBuffer->Write( value.c_str(), value.size()+1 );

//DEBUG_SHOWHEX( _pBuffer->GetReadPtr(), _pBuffer->GetDataSize(), 0 );

	return (*this);
}

Serializer& Serializer::operator>>(string& value)
{//字符类型  相当于先 strcpy， 再追加一个 '\0'

    //要读取的字符串长度
    int lenToRead = strlen(_pBuffer->GetReadPtr());

	if(lenToRead>0)
	{
		//是否超出范围
		if( lenToRead > _pBuffer->GetDataSize() )
		{
			_iError = -1;
		}
		else
		{
			value.assign( _pBuffer->GetReadPtr(), lenToRead );
			_iError = _pBuffer->MoveReadPtr(lenToRead+1);  //跳过包体中的字符串结束符 '\0'
		}
	}
	else
	{
		value.clear();
	}
	return (*this);
}

Serializer& Serializer::operator<<(const BaseArchive& value)
{
	const_cast<BaseArchive&>(value).Serialize(*this,false);
	return (*this);
}

Serializer& Serializer::operator>>(BaseArchive& value)
{
	value.Serialize(*this,true);
	return (*this);
}

Serializer& Serializer::operator<<(const DateTime& value)
{
	double timespan = value.GetTime()*1000;	
	_iError = _pBuffer->Write(&timespan,sizeof(double));
	return (*this);
}

Serializer& Serializer::operator>>(DateTime& value)
{
	double millsec = 0;
	_iError = _pBuffer->Read(&millsec,sizeof(Double));
	value.SetTime(millsec/1000);	
	return (*this);
}

