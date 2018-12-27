#include "Logger.h"
#include <Windows.h>
#include <stdio.h>
#include <stdarg.h>

namespace x {

void Log(const char* format, ...)
{
	char buffer[500];
	va_list arg;
	va_start(arg, format);
	vsprintf_s(buffer, 500, format, arg);
	va_end(arg);
	OutputDebugStringA(buffer);
}

void LogError(const char* format, ...)
{
	char buffer[500];
	va_list arg;
	va_start(arg, format);
	vsprintf_s(buffer, 500, format, arg);
	va_end(arg);
	OutputDebugStringA("[ERROR]: ");
	OutputDebugStringA(buffer);
}

void LogWarn(const char* format, ...)
{
	char buffer[500];
	va_list arg;
	va_start(arg, format);
	vsprintf_s(buffer, 500, format, arg);
	va_end(arg);
	OutputDebugStringA("[WARNING]: ");
	OutputDebugStringA(buffer);
}

}