#ifndef _INIFILE_H__
#define _INIFILE_H__
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <fstream>

using namespace std;

typedef map<string, string, less<string> > strMap;
typedef strMap::iterator strMapIt;

const char*const MIDDLESTRING = "_____***_______";

struct analyzeini
{
public:	
	analyzeini(strMap & strmap):_pmap(&strmap){}
	
	void operator()( const string & strini)
	{
		int first =strini.find('[');
		int last = strini.rfind(']');
		
		if( first != (int)string::npos && last != (int)string::npos && first != last+1)
		{
					_strsect = strini.substr(first+1,last-first-1);
					return ;
		}
		
		if(_strsect.empty())
				return ;
		
		if((first=strini.find('='))== (int)string::npos)
			return ;
		
		string strtmp1= strini.substr(0,first);
		string strtmp2=strini.substr(first+1, string::npos);
		first= strtmp1.find_first_not_of(" \t");
		last = strtmp1.find_last_not_of(" \t");

		if(first == (int)string::npos || last == (int)string::npos)
			return ;
		
		string strkey = strtmp1.substr(first, last-first+1);
		first = strtmp2.find_first_not_of(" \t");
		
    if(((last = strtmp2.find("\t#", first )) != -1) ||
            ((last = strtmp2.find(" #", first )) != -1) ||
            ((last = strtmp2.find("\t//", first )) != -1)||
            ((last = strtmp2.find(" //", first )) != -1))
    {
            strtmp2 = strtmp2.substr(0, last-first);
    }
		
    last = strtmp2.find_last_not_of(" \t");
		
    if(first == (int)string::npos || last == (int)string::npos)
    	return ;
		
		string value = strtmp2.substr(first, last-first+1);
		string mapkey = _strsect + MIDDLESTRING;
		mapkey += strkey;
		(*_pmap)[mapkey]=value;

	//	cout <<mapkey <<"=" <<value <<endl;
    return ;
	}

private:
	string _strsect;
	strMap *_pmap;

};

class IniFile
{
public:
	IniFile( ){};
	~IniFile( ){};
	bool open(const char* pinipath)
	{
		return do_open(pinipath);
	}

	//��ȡ����
	//����ֵ 0�ɹ�  ��0 ʧ�ܻ���δ�ҵ�
	int read(const char*psect, const char*pkey, string &output)
	{
		string mapkey = psect;
		mapkey += MIDDLESTRING;
		mapkey += pkey;
		strMapIt it = c_inimap.find(mapkey);
		//cout << mapkey << "=" << it->second <<endl;
		if(it == c_inimap.end())
		{
			output = "";
			return -1;
		}
		else
			output = it->second;

		return 0;
	}

private:
	bool do_open(const char* pinipath)
	{
		ifstream fin(pinipath);
		if(!fin.is_open())
			return false;
		
		vector<string> strvect;
		while(!fin.eof())
		{
			string inbuf, subbuf;
			getline(fin, inbuf,'\n');
			int last_pos = inbuf.find_last_of("\r\n");
			if (last_pos != (int)string::npos) 
				subbuf = inbuf.substr(0, last_pos);
			else
				subbuf = inbuf;

			strvect.push_back(subbuf);
		}
		
		if(strvect.empty())
			return false;
		
		for_each(strvect.begin(), strvect.end(), analyzeini(c_inimap));
		
		return !c_inimap.empty();		
	}
	
private:
    strMap    c_inimap;
};

#endif

