#ifndef LOG_H
#define LOG_H
#include <string>
#include <assert.h>
#include <cstring>
using namespace std;
#define LOG_FATAL   0
#define LOG_WARN    1
#define LOG_ERROR   2
#define LOG_INFO    3
#define LOG_DEBUG   4

class Log
{
public:
	Log (const char* dir,int lvl,int s,int num,const char* p = NULL);
	~Log() {};

	void hex_log (const char* file_name, const char* buf, int length);
	void write_log (int lvl, const char * file, int line, const char* fmt, ...);
	void write_log_null(int lvl,const char* fmt,...){}

protected:
	static const int LOG_SHIFT_COUNT  = 32;
	int shift_files (const char* file_name);
	string log_file_name (const char* pre_name, int cur_index);
	string log_file_name (int type);

	char	path[256];
	int		level;
	int 	max_log_size;
	int 	max_log_num;

	char	prefix[32];
};

extern Log* g_pLog;

#define LOG(logLevel,format...) g_pLog->write_log(logLevel, __FILE__, __LINE__, format)

#define HEXLOG g_pLog->hex_log

#endif

