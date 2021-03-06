// これは メイン DLL ファイルです。

/* $Id$
 *
 * Log4Infi.c
 *
 * Copyright 2010-2011, hudson.hu All rights reserved.
 *
 */
//


#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#include <time.h>
#include "Log4Infi.h"


// SHELL API
#include <shlobj.h>
#include <shlwapi.h>
#include <process.h>

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif

//
//TODO: この DLL が MFC DLL に対して動的にリンクされる場合、
//		MFC 内で呼び出されるこの DLL からエクスポートされたどの関数も
//		関数の最初に追加される AFX_MANAGE_STATE マクロを
//		持たなければなりません。
//
//		例:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 通常関数の本体はこの位置にあります
//		}
//
//		このマクロが各関数に含まれていること、MFC 内の
//		どの呼び出しより優先することは非常に重要です。
//		これは関数内の最初のステートメントでなければな 
//		らないことを意味します、コンストラクタが MFC
//		DLL 内への呼び出しを行う可能性があるので、オブ
//		ジェクト変数の宣言よりも前でなければなりません。
//
//		詳細については MFC テクニカル ノート 33 および
//		58 を参照してください。
//

#define ALLONE 		1
#define DAYBYDAY 	2
#define ONETIME		3

#define ON '1'
#define FROM_START_LEVEL 1

#ifdef THREAD_HANDLE
#undef THREAD_HANDLE
#endif

#ifdef SELF_DEBUG
#undef SELF_DEBUG
#endif

#ifdef PATH_DEBUG
#undef PATH_DEBUG
#endif

//#define SELF_DEBUG
#define PATH_DEBUG

int start_priority = 0;
static const char* const priorities[] = {
    "FATAL", 
    "ALERT",
    "CRIT",
    "ERROR",
    "WARN",
    "NOTICE",
    "INFO",
    "DEBUG",
    "TRACE",
    "NOSET",	//NOSET
    "UNKNOWN"	//
};

typedef struct init_property{

	int		output_type;
	int		level_select_type;
	int		start_level;
	char	enable_level[12];
	char	logfilename[1024];
	char	pc_name[256];
	char	logon_name[256];
	char	others[256];
}log_output_setting;
log_output_setting	log_setting;

static char* log_get_log_file(int log_prority, char *file_prefix);
static void log_category_com( void *category);
static  char* log_data_format( int priority, char *logmsg, char *outmsg );
static char* log_get_parameter(char *para_name);
static const char* log_priority_to_string(int a_priority);
static bool log_priority_is_enable(int a_priority);
static void log_get_pcname(char *pcname, int buffersize);
static void  log_get_logon_username(char *lpusername, int buffersize);
static void  log_get_local_time_now( char * timenow );

void __stdcall infini_log_init(void)
{
	char *lplog_type = NULL;
	char *selecttype = NULL;
	char *plogname = NULL;
	char *lplogfile=NULL, *find_point=NULL;
	char szPath[512] = {0};
	char szDirPath[1024] = {0};

	__try
	{
		//log type daybyday or allone
		lplog_type = (char*)log_get_parameter("log_output_type");

		if( lplog_type == NULL)
		{
			log_setting.output_type = ALLONE;
		}
		else if( *lplog_type == '2')
		{
			log_setting.output_type = DAYBYDAY;
		}
		else if( *lplog_type == '3')
		{
			log_setting.output_type = ONETIME;
		}
		else
		{
			log_setting.output_type = ALLONE;
		}

		selecttype = log_get_parameter("select_priority_type");
		if(selecttype == NULL)
		{
			log_setting.level_select_type = 0;
		}
		else
		{
			log_setting.level_select_type = atoi(selecttype);
		}
		
		char *lppriority = log_get_parameter("start_priority");
		if( lppriority == NULL)	//default level is LOG_PRIORITY_ERRROR
		{
			log_setting.start_level = LOG_PRIORITY_ERROR;
		}
		else
		{
			log_setting.start_level = atoi(lppriority);
		}
		
		lppriority = log_get_parameter("enable_priority");
		if( lppriority == NULL)
		{
			memset(log_setting.enable_level,0x00, sizeof(log_setting.enable_level));
		}
		else
		{
			_snprintf_s(log_setting.enable_level, sizeof(log_setting.enable_level), 11, "%s", lppriority);
		}
		 
		//get logon user name
		log_get_logon_username(log_setting.logon_name, sizeof(log_setting.logon_name));
		//get pc name 
		log_get_pcname(log_setting.pc_name, sizeof(log_setting.pc_name));

		 //Set lsog file's saving folder 	
		//when win7  this derictory is variable CSIDL_COMMON_APPDATA

		lppriority = log_get_parameter("is_service");
		if( lppriority == NULL)
		{
			if(!SUCCEEDED(SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, szDirPath))) 
			{
				//GetModuleFileName (NULL, szDirPath, sizeof (szDirPath)) ;CSIDL_PERSONAL
				if(_mkdir(szDirPath) == 0 )	//if create foleder success
				{
					//SetFileAttributes(szPath, FILE_ATTRIBUTE_HIDDEN |FILE_ATTRIBUTE_SYSTEM);
					SetFileAttributes(szDirPath, FILE_ATTRIBUTE_HIDDEN );
				}
				find_point = strrchr (szDirPath, '\\');

				if(find_point) 
					*find_point = 0;

				_snprintf(szPath, sizeof(szPath)-1, "%s\\Infini_Log\\", szDirPath);
			}
			else
			{
				_snprintf(szPath, sizeof(szPath)-1, "%s\\Infini_Log\\", szDirPath);
				if(_mkdir(szPath) == 0 )	//if create foleder success
				{
					//SetFileAttributes(szPath, FILE_ATTRIBUTE_HIDDEN |FILE_ATTRIBUTE_SYSTEM);
					SetFileAttributes(szPath, FILE_ATTRIBUTE_HIDDEN );
				}
				find_point = strrchr (szPath, '\\');

				if(find_point) 
					*find_point = 0;

				//_snprintf(szPath, sizeof(szPath)-1, "%s\\Infini_Log\\", szDirPath);
				//_snprintf(szPath, sizeof(szPath)-1, "%s\\Infini_Log\\", szDirPath);

			}
			//DWORD flag = MB_ICONINFORMATION | MB_SETFOREGROUND | MB_TOPMOST ;

			//_snprintf(szPath, sizeof(szPath)-1, "%s\\Infini_Log\\", szDirPath);
			//open the log file with append mode 
		}
		else
		{
			GetModuleFileName (NULL, szDirPath, sizeof (szDirPath)) ;
			find_point = strrchr (szDirPath, '\\');

			if(find_point) 
				*find_point = 0;

			_snprintf(szPath, sizeof(szPath)-1, "%s\\Infini_Log\\", szDirPath);

			if(GetFileAttributes(szPath) == -1)
			{
				if(_mkdir(szPath) == 0 )	//if create foleder success
				{
					//SetFileAttributes(szPath, FILE_ATTRIBUTE_HIDDEN |FILE_ATTRIBUTE_SYSTEM);
					SetFileAttributes(szPath, FILE_ATTRIBUTE_HIDDEN );
				}
				else				//if create directory error then save as execution file's folder
				{
					GetModuleFileName (NULL, szPath, sizeof (szPath)) ;
			
					find_point = strrchr (szPath, '\\');

					if(find_point) 
						*find_point = 0;
				}
			}
		}
#ifdef PATH_DEBUG
		DWORD flag = MB_ICONINFORMATION | MB_SETFOREGROUND | MB_TOPMOST ;
		MessageBox(GetForegroundWindow(), szPath, "Read ini file success!", flag);
#endif
		plogname = log_get_parameter("prefix_file_name");

		lplogfile = log_get_log_file(0, plogname);	//get log file name
		
		//get full log file path
		int i = _snprintf(log_setting.logfilename, sizeof(log_setting.logfilename), "%s\\%s", szPath, lplogfile) ;
#ifdef PATH_DEBUG
		 flag = MB_ICONINFORMATION | MB_SETFOREGROUND | MB_TOPMOST ;
		MessageBox(GetForegroundWindow(), log_setting.logfilename, "Read ini file success!", flag);
#endif
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
#ifdef SELF_DEBUG
		DWORD flag = MB_ICONINFORMATION | MB_SETFOREGROUND | MB_TOPMOST ;
		MessageBox(GetForegroundWindow(), "Init error!", "ALOKY.NET.Log", flag);

#endif	
		return;
	}

	return;
}

/*************************************************************************
Name:  infini_category_log
Function:  output log data
Description: output formatted log data, extern interface
Param:
	char *a_category
	char *a_priority
	const char* a_format output log's data format

Return:
	void
*************************************************************************/
void __stdcall infini_log_output(char* a_category, int a_priority,  const char* a_format, ...)
{    
	char message[1024] = {0};
	__try{
		if(log_priority_is_enable(a_priority))
		{	
			char message[1024] = {0};
			char loginfo[1024] = {0};
			//char temp[1024] = {0};
			char outmsg[2046] = {0};
			va_list va;	
			int n;
			char *lplog = NULL;

			va_start(va, a_format);	

			memset(message, 0x00, sizeof(message));
			memset(loginfo, 0x00, sizeof(loginfo));
			memset(outmsg, 0x00, sizeof(outmsg));
			n = vsnprintf_s(message, sizeof(message), sizeof(message)-1, a_format, va);
			va_end(va);
			if( n > -1 && n > 1024)
			{
				//CloseHandle(hMutex) ;
				return;
			}
			
			lplog = log_data_format(a_priority, message, outmsg);
			_snprintf(loginfo, sizeof(loginfo)-1, "[%s] %s", a_category, outmsg);

	#ifdef THREAD_HANDLE
			_beginthread(log_category_com, 0, lplog);
			Sleep(10);
	#else
			log_category_com( loginfo );
			//Sleep(0);

	#endif
			    
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		if(message != NULL)
		{	
			char temp[1024] = {0};

			_snprintf(temp, sizeof(temp)-1, "[%s] %s\n", "Log ouput error, trace message is:", message);
			log_category_com(temp);
		}
		else
			log_category_com("Log output function has Error for msg is incorrect\n");
		return;
	}
	return;
}

/*************************************************************************
Name:  log_category_com
Function:  output log data
Description: output formatted log data
Param:
	char *log_infocategory
	char *category

Return:
	FALSE log output is failure TRUE log output is success
*************************************************************************/
static void log_category_com( void* category)
{
	FILE	*fp;
	errno_t err;

	if(category == NULL) 
	{
#ifdef THREAD_HANDLE
		_endthread();
#endif
		return ;
	}
	
	//open the log file with append mode 
	if((err = fopen_s(&fp, log_setting.logfilename, "a")) == 0) 
	//if((fp = fopen(log_setting.logfilename, "a")) != NULL) 
	{
		fprintf(fp, "%s", category) ;
	    fclose(fp);
#ifdef THREAD_HANDLE
		 _endthread();
#endif
	    return ;
	}
	
#ifdef THREAD_HANDLE
	_endthread();
#endif THREAD_HANDLE

	return ;
}


/*************************************************************************
Name:  log_data_format
Function:  format log data
Description: format output log data
Param:
	int log_prority output log's proirity
	char *logmsg output log message

Return:
	formatted log message
*************************************************************************/
static  char* log_data_format( int priority, char *logmsg, char *outmsg )
{
    char buffer[2046] = {0};
	char pcname[256] = {0};
	char logon_username[256] = {0};

	FILETIME  ft,local_ft;
	SYSTEMTIME stime;
	
	GetSystemTimeAsFileTime(&local_ft);

	if(FileTimeToLocalFileTime (&local_ft, &ft) == NULL)
		return buffer;

	//output log data
	if ( FileTimeToSystemTime(&ft, &stime))
   	{
		if( (priority)>= LOG_PRIORITY_TRACE )
		{
			log_get_pcname(pcname ,sizeof(pcname));	//get pc name
			log_get_logon_username(logon_username, sizeof(logon_username)); //get username
			_snprintf(outmsg, 2045, "[%s:%s] {[%04d/%02d/%02d %02d:%02d:%02d] [%-8s] %s}\n",
			 pcname, logon_username,
			 stime.wYear, stime.wMonth , stime.wDay,
			 stime.wHour, stime.wMinute, stime.wSecond,
			 log_priority_to_string(priority),
			 logmsg);
		}
		else
		{
			log_get_pcname(pcname ,sizeof(pcname));	//get pc name
			log_get_logon_username(logon_username, sizeof(logon_username)); //get username
			_snprintf(outmsg, 2045, "[%s:%s] {[%04d/%02d/%02d %02d:%02d:%02d] [%-8s] %s}\n",
			 pcname, logon_username,
			 stime.wYear, stime.wMonth , stime.wDay,
			 stime.wHour, stime.wMinute, stime.wSecond,
			 log_priority_to_string(priority),
			 logmsg);


			/*_snprintf(buffer, sizeof(buffer), "{%04d/%02d/%02d %02d:%02d:%02d %-8s  %s}\n",
			 stime.wYear, stime.wMonth , stime.wDay,
			 stime.wHour, stime.wMinute, stime.wSecond,
			 log_priority_to_string(priority),
			 logmsg);*/
		}
    }
    return outmsg;
}

/*************************************************************************
Name:  log_get_parameter
Function:  read parameter from configuration file
Description: read parameter form configration file
Param:
	char* 	parameter name

Return:
	parameter's value

Note:file format
[outputsetting]
start_priority=200
log_output_type=daybyday 
specified_priority=300
*************************************************************************/
static char* log_get_parameter(char *para_name)
{
	char	szPath[1024] = {0};
	char	current_dir[1024] = {0} ;
	char    output[1024] = {0};
	char	*find_point;

	GetModuleFileName (NULL, current_dir, sizeof (current_dir)) ;
	
	find_point = strrchr (current_dir, '\\');

	if(find_point) 
		*find_point = 0;
	
	if(strlen(current_dir) != 0) 
	{
		// Read parameter from configuration file 
		sprintf(szPath, "%s\\logrc.ini", current_dir) ;
		
		if(strlen(szPath) && GetFileAttributes(szPath) != -1) 
		{
			if(!GetPrivateProfileString ("outputsetting", para_name, NULL, output, sizeof(output), szPath)) 
				return NULL ;

			return output;
		}
	}
	
	return NULL;
}

/*************************************************************************
Name:  log_get_log_file
Function:  get log file's name
Description: get log file's name
Param:
	int log_prority output log's proirity
	char *file_prefix log file's prefix name

Return:
	output log file name
*************************************************************************/
static char* log_get_log_file(int log_prority, char *file_prefix)
{
	char logfile[256] = {0};
	char prefix[50] ={0};

	//if have prefix or not
	if(file_prefix == NULL)
	{
		strcpy(prefix, "INFINI");
	}
	else
	{
		_snprintf(prefix, sizeof(prefix) -1, file_prefix);
	}

	//if logfile type is daybyday
	if(log_setting.output_type == DAYBYDAY)
	{
		char	timebuf[128], timestamp[24];
		char	yy[5],mm[3],dd[3],nf[3];
		
		_strdate( timebuf );
		strcpy(nf,&timebuf[6]);
		
		if(atoi(nf) >= 50) 
		{
			strcpy(yy,"19");
		}
		else
		{
			strcpy(yy,"20");
		}
		
		strcat(yy, nf);
		strncpy(mm, &timebuf[0], 2);
		mm[2]=0x00;
		strncpy(dd, &timebuf[3], 2);
		dd[2]=0x00;

		strcpy(timestamp, yy);
		strcat(timestamp, mm);
		strcat(timestamp, dd);
		
		_snprintf (logfile, sizeof(logfile) - 1, "LOG_%s_%s.log", prefix, timestamp);
	}
	else if(log_setting.output_type == ONETIME) // one time
	{
		char timestamp[36] = {0};
		log_get_local_time_now(timestamp);
		_snprintf (logfile, sizeof(logfile) - 1, "LOG_%s_%s.log", prefix, timestamp);
	}
	else //all one
	{
		_snprintf (logfile, sizeof(logfile) - 1, "LOG_%s_%s.log", prefix, log_setting.logfilename);
	}
	
	return logfile;
}

/*************************************************************************
Name:  log_priority_to_string
Function:  get priority's name
Description: get priority's name
Param:
	int *a_priority

Return:
	priority name
*************************************************************************/
static const char* log_priority_to_string(int a_priority)
{
    a_priority /= 100;
    if ( (a_priority < 0) || (a_priority > 10) ) 
	a_priority = 10;

    return priorities[a_priority];
}

/*************************************************************************
Name:  log_priority_is_enable
Function:  get priority's name
Description: get priority's name
Param:
	int *a_priority

Return:
	FALSE log output is failure TRUE log output is success
*************************************************************************/
static bool log_priority_is_enable(int a_priority)
{
	
	//from start level
	if(log_setting.level_select_type == FROM_START_LEVEL )
	{

		if( log_setting.start_level >= a_priority)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		if(( LOG_PRIORITY_UNKNOWN >= a_priority) &&( a_priority >= LOG_PRIORITY_FATAL) )
		{
			if(log_setting.enable_level[(a_priority + 100)/100 - 1] == ON)
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}

		return FALSE;
	}
}

/*************************************************************************
Name:  log_get_pcname
Function:  get computer name
Description: get computer name
Param:
	char *pcname[OUT]

Return:
	void
*************************************************************************/
static void  log_get_pcname(char *pcname, int buffersize)
{
	char	lpBuffer[1024];
	DWORD	lpSize = 1024;

	if (GetComputerName(lpBuffer, &lpSize))
	{
		_snprintf (pcname, buffersize, "%s", lpBuffer) ;
	}
	else
	{
		_snprintf (pcname, buffersize, "%s", "ERROR") ;
	}

	return;
}

/*************************************************************************
Name:  log_get_logon_username
Function:  get logon username
Description: get logon username
Param:
	char *lpusername[OUT]

Return:
	void
*************************************************************************/
static void  log_get_logon_username(char *lpusername, int buffersize)
{
	char logon_name[256];
	DWORD	dwSize;
	
	dwSize = sizeof(logon_name);

	if( GetUserName ((LPSTR)&logon_name, (LPDWORD)&dwSize))
	{
		_snprintf (lpusername, buffersize, "%s", logon_name);
	}
	else
	{
		_snprintf (lpusername, buffersize, "%s", "ERROR");
	}
	return;					
}

/*************************************************************************
Name:  log_get_local_time
Function:  get local time
Description: get local format time
Param:
	char *timenow[OUT]

Return:
	void
*************************************************************************/
static void log_get_local_time (char *timenow)
{
	char timebuf[48], tmpbuf[128], timestamp[24];
	char yy[5],mm[3],dd[3],nf[3];

	time_t now;
	time(&now);
	_strdate(timebuf);
	_strtime( tmpbuf );

	strcpy(nf,&timebuf[6]);
	if(atoi(nf)>=50) 
	{
		strcpy(yy,"19");
	}
	else
	{
		strcpy(yy,"20");
	}

	strcat(yy,nf);
	strncpy(mm,&timebuf[0],2);
	mm[2]='\0';
	strncpy(dd,&timebuf[3],2);
	dd[2]='\0';

	strcpy(timestamp,yy);
	strcat(timestamp,"/");
	strcat(timestamp,mm);
	strcat(timestamp,"/");
	strcat(timestamp,dd);
	strcat(timestamp," ");
	strcat(timestamp,tmpbuf);

	strcpy (timenow, timestamp);

	return ;
}

/*************************************************************************
Name:  log_get_local_time_now
Function:  get local time now 
Description: get local format time
Param:
	char *timenow[OUT]

Return:
	void
*************************************************************************/
static void  log_get_local_time_now( char * timenow )
{
	char	timebuf[ 48 ], 
			 tmpbuf[ 128 ], 
			 timestamp[ 24 ];
	char	yy[ 5 ], 
			 mm[ 3 ], 
			 dd[ 3 ], 
			 ho[ 3 ], 
			 min[ 3 ], 
			 sec[ 3 ], 
			 nf[ 3 ];

	time_t	now;
	time( &now );
	_strdate( timebuf );
	_strtime( tmpbuf );

	strcpy( nf, &timebuf[ 6 ] );
	if( atoi( nf ) >= 50 )
	{
		strcpy( yy, "19" );
	}
	else
	{
		strcpy( yy, "20" );
	}

	strcat( yy, nf );
	strncpy( mm, &timebuf[ 0 ], 2 );
	mm[ 2 ] = '\0';
	strncpy( dd, &timebuf[ 3 ], 2 );
	dd[ 2 ] = '\0';

	strncpy( ho, &tmpbuf[ 0 ], 2 );
	ho[ 2 ] = '\0';
	strncpy( min, &tmpbuf[ 3 ], 2 );
	min[ 2 ] = '\0';
	strncpy( sec, &tmpbuf[ 6 ], 2 );
	sec[ 2 ] = '\0';

	strcpy( timestamp, yy );
	strcat( timestamp, mm );
	strcat( timestamp, dd );
	strcat( timestamp, ho );
	strcat( timestamp, min );
	strcat( timestamp, sec );

	strcpy( timenow, timestamp );

	return;
}