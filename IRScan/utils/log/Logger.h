#ifndef LOGGER__H__
#define LOGGER__H__
#pragma once


#include <stdio.h>
#include <windows.h>


#define DEBUG_OUT_FILE 

#ifdef  DEBUG_OUT_FILE
#define LOG_SETFILENAME(filename)	g_GlbLog.SetFileName(filename)
#define LOG_SETLOGLEVEL_NOTHING		g_GlbLog.SetLogLevel(CLoggerlevelNothing)

#define LOG_SETLOGLEVEL_ERROR		g_GlbLog.SetLogLevel(CLoggerlevelError)
#define LOG_SETLOGLEVEL_INFO		g_GlbLog.SetLogLevel(CLoggerlevelInfo)
#define LOG_SETLOGLEVEL_DEVELOPERINFO	g_GlbLog.SetLogLevel(CLoggerlevelDeveloperInfo)

#define LOGINFO g_GlbLog.Lock();\
	g_GlbLog.SetLineNumber(__LINE__);\
	g_GlbLog.SetSourceFileName(__FILE__);\
	g_GlbLog.LogInfo

#define LOGERROR g_GlbLog.Lock();\
	g_GlbLog.SetLineNumber(__LINE__);\
	g_GlbLog.SetSourceFileName(__FILE__);\
	g_GlbLog.LogError

#define LOGDEVINFO g_GlbLog.Lock();\
	g_GlbLog.SetLineNumber(__LINE__);\
	g_GlbLog.SetSourceFileName(__FILE__);\
	g_GlbLog.LogDevInfo

#define LOGALL g_GlbLog.Lock();\
	g_GlbLog.SetLineNumber(__LINE__);\
	g_GlbLog.SetSourceFileName(__FILE__);\
	g_GlbLog.LogAll

#else
#define LOG_SETFILENAME(filename) 
#define LOG_SETLOGLEVEL_NOTHING 
#define LOG_SETLOGLEVEL_ERROR 
#define LOG_SETLOGLEVEL_INFO 
#define LOG_SETLOGLEVEL_DEVELOPERINFO 
#define LOGINFO 
#define LOGERROR 
#define LOGDEVINFO
#define LOGALL
#endif


typedef enum CLoggerLevels
{
	CLoggerlevelNothing,
	CLoggerlevelError,
	CLoggerlevelInfo,
	CLoggerlevelDeveloperInfo
}CLoggerLevels;


class CLogger
{
private:
	CRITICAL_SECTION m_cs;
	FILE *m_File;
	TCHAR m_FileName[MAX_PATH];
	CLoggerLevels m_LogLevel;
	char m_SourceFile[MAX_PATH];
	int m_LineNumber;
	void LogNow(TCHAR *LoglevelName, TCHAR *LogString);
	void ReplaceCRLF(TCHAR *s);
public:
	CLogger();
	~CLogger();
	void Lock() {EnterCriticalSection(&m_cs);};
	void SetLineNumber(int line) {m_LineNumber = line;};
	void SetSourceFileName(char *filename);
	bool SetFileName(const char* FileName);
	void SetLogLevel(CLoggerLevels LogLevel);
	void LogError(const char *format, ...);
	void LogInfo(TCHAR *format, ...);
	void LogDevInfo(TCHAR *format, ...);
	void LogAll(TCHAR *format, ...);
};


extern CLogger g_GlbLog;


#endif