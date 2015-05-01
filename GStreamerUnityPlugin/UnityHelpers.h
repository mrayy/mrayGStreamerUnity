

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


struct Vector2d
{
	int x, y;
	Vector2d() :x(0), y(0){}
	Vector2d(int a,int b) :x(a), y(b){}

	bool operator ==(const Vector2d& o)
	{
		return x == o.x && y == o.y;
	}
	bool operator !=(const Vector2d& o)
	{
		return x != o.x || y != o.y;
	}
};

template <class T>
static void Swap(T &a, T &b)
{
	T tmp = a;
	a = b;
	b = tmp;
}

#endif
