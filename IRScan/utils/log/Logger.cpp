#include <stdafx.h>
#include <Windows.h>
#include <crtdbg.h>
#include <stdlib.h>
#include <time.h>
#include "Logger.h"


#define MAX_LOG_BUF 1024 * 100


CLogger g_GlbLog;


CLogger::CLogger() {
	m_FileName[0] = '\0';
	m_LogLevel = CLoggerlevelNothing;
	m_File = NULL;
	InitializeCriticalSection(&m_cs);
}

CLogger::~CLogger() {
	if (m_File != NULL)
		fclose(m_File);
	DeleteCriticalSection(&m_cs);
}

bool CLogger::SetFileName(const char* FileName) {
	if (m_File != NULL)
		fclose(m_File);

	TCHAR file[MAX_PATH];
	int r = GetModuleFileName(NULL, file, MAX_PATH);
	if (r == 0)
		return false;

	TCHAR *mid = file + r;
	while (*(--mid) != '\\' && mid > file);
	if (mid <= file)
		return false;

	*(++mid) = '\0';
	strcpy_s(m_FileName, file);
	strcpy_s(m_FileName, FileName);

	fopen_s(&m_File,m_FileName, "ab");
	if (m_File == NULL)
		return false;

	return true;
}

void CLogger::SetSourceFileName(char *filename)
{
	char *mid = filename + strlen(filename);
	while (mid > filename) {
		if (*(--mid) == '\\') {
			mid++;
			break;
		}
	}
	strcpy_s(m_SourceFile, mid);
}

void CLogger::SetLogLevel(CLoggerLevels LogLevel)
{
	m_LogLevel = LogLevel;
}

void CLogger::LogNow(TCHAR *LoglevelName, TCHAR *LogString)
{
	if (m_File == NULL) {
		_ASSERTE(!"Filename is not set...");
		return;
	}

	time_t now;
	time(&now);
	struct tm *tmnow=new struct tm;
	localtime_s(tmnow,&now);
	char strnow[25] = {0x0,};
	strftime(strnow, 24, "%Y-%m-%d %H:%M:%S", tmnow);

#ifdef _UNICODE
	if (m_LogLevel == CLoggerlevelDeveloperInfo)
		fprintf(m_File, "%s %S\t%s, %d\t%S\r\n", strnow, 
		LoglevelName, m_SourceFile, m_LineNumber, LogString);
	else
		fprintf(m_File, "%s %S\t%S\r\n", strnow, LoglevelName, LogString);
#else
	if (m_LogLevel == CLoggerlevelDeveloperInfo)
		fprintf(m_File, "%s %s\t%s, %d\t%s\r\n", strnow, 
		LoglevelName, m_SourceFile, m_LineNumber, LogString);
	else
		fprintf(m_File, "%s %s\t%s\r\n", strnow, LoglevelName, LogString);
#endif

#ifdef LOG_TRACE
	TCHAR mid[MAX_LOG_BUF] = {0};
	wsprintf(mid, _T("%s\r\n"), LogString);
	OutputDebugString(mid);
#endif
#ifdef LOG_STDOUT
	TCHAR mid2[MAX_LOG_BUF] = {0};
	wsprintf(mid2, _T("%s\r\n"), LogString);
	wprintf(mid2);
#endif
	fflush(m_File);
}

void CLogger::ReplaceCRLF(TCHAR *s) {
	TCHAR *mid = s;
	while (*mid != '\0') {
		switch (*mid) {
		case '\r':
			*mid = '|';
			break;
		case '\n':
			*mid = '|';
			break;
		}
		mid++;
	}
}

//2 - Info
void CLogger::LogInfo(TCHAR *format, ...)
{
	if (m_LogLevel == CLoggerlevelDeveloperInfo || 
		m_LogLevel == CLoggerlevelInfo) {
		DWORD LastError = GetLastError();
		TCHAR mid[MAX_LOG_BUF] = {0}; 
		va_list args;
		va_start(args, format);
		vsprintf_s(mid, format, args);
		ReplaceCRLF(mid);
		LogNow(("[INFO]"), mid);
		va_end(args);
		SetLastError(LastError);
	}
	LeaveCriticalSection(&m_cs);
}

//3 - Error
void CLogger::LogError(const char* format, ...)
{
	if (m_LogLevel == CLoggerlevelDeveloperInfo || 
		m_LogLevel == CLoggerlevelInfo || 
		m_LogLevel == CLoggerlevelError) {
		DWORD LastError = GetLastError();
		char mid[MAX_LOG_BUF] = {0}; 
		va_list args;
		va_start(args, format);
		vsprintf_s(mid, format, args);
		ReplaceCRLF(mid);
		LogNow(("[ERROR]"), mid);
		va_end(args);
		SetLastError(LastError);
	}
	LeaveCriticalSection(&m_cs);
}

//1 - All
void CLogger::LogDevInfo(TCHAR *format, ...)
{
	if (m_LogLevel == CLoggerlevelDeveloperInfo) {
		DWORD LastError = GetLastError();
		TCHAR mid[MAX_LOG_BUF] = {0}; 
		va_list args;
		va_start(args, format);
		vsprintf_s(mid, format, args);
		ReplaceCRLF(mid);
		LogNow(("[DEVINFO]"), mid);
		va_end(args);
		SetLastError(LastError);
	}
	LeaveCriticalSection(&m_cs);
}

void CLogger::LogAll(TCHAR *format, ...)
{
	DWORD LastError = GetLastError();
	TCHAR mid[MAX_LOG_BUF] = {0}; 
	va_list args;
	va_start(args, format);
	vsprintf_s(mid, format, args);
	ReplaceCRLF(mid);
	LogNow(("[*ALL*]"), mid);
	va_end(args);
	SetLastError(LastError);
	LeaveCriticalSection(&m_cs);
}