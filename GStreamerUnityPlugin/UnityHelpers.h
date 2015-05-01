

#ifndef __UNITYHELPERS__
#define __UNITYHELPERS__

#include <string>
#include <sstream>


enum ELogLevel{
	ELL_INFO,
	ELL_SUCCESS,
	ELL_WARNING,
	ELL_ERROR
};

extern void LogMessage(const std::string& msg, ELogLevel level);


typedef void(*FuncPtr)(const char*);

#endif
