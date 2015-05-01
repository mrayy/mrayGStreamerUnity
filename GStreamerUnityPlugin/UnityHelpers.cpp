

#include "UnityHelpers.h"

#include "UnityPlugin.h"
#include <windows.h>


FuncPtr Debug;

void LogMessage(const std::string& msg, ELogLevel level)
{
	std::string m;
	if (level == ELL_INFO)
		m = "Info: ";
	if (level == ELL_WARNING)
		m = "Warning: ";
	if (level == ELL_SUCCESS)
		m = "Success: ";
	if (level == ELL_ERROR)
		m = "Error: ";

	m += msg;
#if UNITY_WIN
	OutputDebugStringA(m.c_str());
#else
	printf("%s", m.c_str());
#endif
	if (Debug)
		Debug(m.c_str());
}

extern "C" EXPORT_API void mray_SetDebugFunction(FuncPtr f)
{
	Debug = f;
}