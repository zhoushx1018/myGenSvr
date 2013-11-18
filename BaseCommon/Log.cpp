#include "Log.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdarg.h>
#include <time.h>
#include <stdio.h>
#include <sys/time.h>

char fatal_warn_file[] = "fatal_warn";
char error_file[] = "error";
char info_file[] = "info";
char debug_file[] = "debug";


Log::Log (const char* dir,int lvl,int s,int num,const char *p)
{
	max_log_size = s;
	max_log_num = num;
	level = lvl;
	strncpy (path, dir, 255);
	if (p == NULL)
		memset (prefix, 0x0, sizeof prefix);
	else
		strncpy (prefix, p, sizeof prefix);
}

string Log::log_file_name(const char* pre_name,int cur_index)
{
	char str_index[10];
	string 	file_name = pre_name;

	if(cur_index==0)
		file_name += ".log";
	else 
	{
		sprintf(str_index, "%d", cur_index);		
		file_name += string (str_index) + ".log";		
	}

	return file_name;
}

string Log::log_file_name(int type)
{
	string 	file_name;
	char	str_date[9];
	time_t now = time (NULL);
	struct tm tm;

	localtime_r(&now, &tm);
	snprintf(str_date, sizeof(str_date), "%04d%02d%02d",
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
	
	switch(type)
	{
		case LOG_FATAL:
		case LOG_WARN:
			file_name = fatal_warn_file;
			break;

		case LOG_ERROR:
			file_name =  error_file;
			break;			
		case LOG_INFO:
			file_name =  info_file;
			break;			
		case LOG_DEBUG:
			file_name =  debug_file;
			break;			
		default:
		    file_name =  debug_file;
			return file_name;
	}
	file_name = file_name + string(str_date);

	if (strlen (prefix) > 0)
		file_name = string (prefix) + file_name;

	file_name = path + string("/") + file_name;
	return file_name;
}

void Log::hex_log (const char* file_name, const char* buf, int length)
{
  FILE* fp;
  if(!(fp=fopen(file_name, "w+b")))
    return ;

  fwrite(buf, length, 1, fp);
  fclose(fp);	
}
void Log::write_log (int lvl, const char * file, int line, const char* fmt, ...)
{
	if (lvl > level)
		return ;
	string file_name = log_file_name (lvl);
	if (file_name.empty ())
		return ;

	//shift_files��bug���ļ���С�жϲ�׼ȷ������С�ļ����л���������
//	shift_files (file_name.c_str ());
	file_name += ".log";
	FILE *fp = fopen (file_name.c_str(), "at");
	if (fp != NULL)
	{
		struct tm tm;
		char log_buf [2048];
		timeval tv;

		memset(&tv,0, sizeof(tv));
		gettimeofday(&tv, NULL);
		localtime_r(&tv.tv_sec, &tm);

		int pos = snprintf(log_buf, sizeof(log_buf),
            "%02d-%02d %02d:%02d:%02d.%03d [%u] [%s%5d] ",
            tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min,
            tm.tm_sec,tv.tv_usec / 1000, pthread_self(),
            file, line);
	
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(log_buf + pos, sizeof(log_buf) - pos, fmt, ap);
    va_end(ap);

		fprintf (fp, "%s\n", log_buf);

		//��ӡ����׼����
		if( lvl <= LOG_ERROR )
			fprintf ( stderr, "%s\n", log_buf);
		
		fclose (fp);
	}
}

int Log::shift_files (const char* file_name)
{
	string tmp,new_file;
	struct stat file_stat;
	
	//check if the file name need to shift
	tmp = log_file_name (file_name, 0);			
	if (stat (tmp.c_str(), &file_stat) < 0 || file_stat.st_size < max_log_size)
		return 0;

	//delete the oldest log file
	tmp = log_file_name (file_name, max_log_num - 1);
	if (access (tmp.c_str(), F_OK) == 0 && remove (tmp.c_str ()) < 0)
		return -1;

	//rename file 
	for(int i = max_log_num - 2; i >= 0; i--)
	{
		tmp = log_file_name(file_name, i);			
		if (access(tmp.c_str(), F_OK) == 0)
		{
			new_file = log_file_name(file_name, i + 1);	
			if (rename(tmp.c_str(),new_file.c_str()) < 0 )
				return -1;
		}
	}
	
	return 0;	
}

