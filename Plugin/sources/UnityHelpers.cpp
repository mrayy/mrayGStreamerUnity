

#include "UnityHelpers.h"

#ifdef WIN32
#include <windows.h>
#endif
//#include <gl/gl.h>
#include <memory.h>



#include "PlatformBase.h"
#include "PixelUtil.h"
#include "ImageInfo.h"


#include <vector>

using namespace mray;
using namespace video;

FuncPtr Debug;
FuncFloatRetPtr GetEngineTimePtr;

std::string g_PluginName = "GST";
void g_SetPluginName(const char* name)
{
	g_PluginName = name;
}


std::vector<std::string> log_buffer;
void LogMessage_internal(ELogLevel level,const std::string& msg)
{
	std::string m= g_PluginName+": " ;
	if (level == ELL_INFO)
		m += "Info: ";
	if (level == ELL_WARNING)
		m += "Warning: ";
	if (level == ELL_SUCCESS)
		m += "Success: ";
	if (level == ELL_ERROR)
		m += "Error: ";

	m += msg;
#if UNITY_WIN
	OutputDebugStringA(m.c_str());
#else
	printf("%s", m.c_str());
#endif
	if (Debug)
	{
		for (int i = 0; i < log_buffer.size(); ++i)
			Debug(log_buffer[i].c_str());
		log_buffer.clear();
		Debug(m.c_str());
	}
	else
		log_buffer.push_back(m);
	//LogManager::Instance()->LogMessage(m);
}


void LogMessage(ELogLevel level,const char *format, ...)
{
	va_list args;
	char buffer[256];
	va_start(args, format);
	vsprintf(buffer,format, args);
	LogMessage_internal(level, buffer);
	va_end(args);
}
float GetEngineTime()
{
	if (GetEngineTimePtr)
	{
		return GetEngineTimePtr();
	}
	return 0;
}

extern "C" UNITY_INTERFACE_EXPORT void mray_SetDebugFunction(FuncPtr f)
{
	Debug = f;
}
extern "C" UNITY_INTERFACE_EXPORT void mray_SetGetEngineTime(FuncFloatRetPtr f)
{
	GetEngineTimePtr = f;
}

LogManager* LogManager::s_instance=0;
LogManager* LogManager::Instance()
{
	if (!s_instance)
	{
		s_instance = new LogManager();
	}
	return s_instance;
}

LogManager::LogManager()
{
	fileName = "GStreamerLog.txt";
	m_logFile = fopen("GStreamerLog.txt", "w");
	fclose(m_logFile);
	m_logFile = 0;
}
LogManager::~LogManager()
{
	fclose(m_logFile);
}
void LogManager::LogMessage(const std::string& msg)
{
	m_logFile = fopen("GStreamerLog.txt", "a");
	fprintf(m_logFile, "%s\n", msg.c_str());
	fclose(m_logFile);
	m_logFile = 0;
}




