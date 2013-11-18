#ifndef LIST_H
#define LIST_H

#include <typeinfo>
#include <string>
#include <list>
#include "CommDef.h"
#include "BaseArchive.h"

using namespace std;


//���л�ר�õ� list ����
//	����֧�����л���list��������
template <typename Type>
class List:public BaseArchive, public list<Type>
{
	public:
		typedef typename list<Type>::iterator Iter;
		typedef typename list<Type>::value_type value_type;

		Serializer& Serialize(Serializer &serializer,bool isLoading)
		{
			if(isLoading)
			{
				UInt16 size = 0;
				serializer>>size;
				
				if(size>0)
				{
					Type t;
					for(int i=0;i<size;i++)
					{
						serializer>>t;
						this->push_back(t);
					}
				}
			}
			else
			{
				UInt16 size = this->size();
				serializer<<size;

				if(size>0)
				{
					for(Iter iter=this->begin();iter!=this->end();iter++)
					{
						serializer<<(*iter);
					}
				}
			}

			return serializer;
		}


};

#endif
