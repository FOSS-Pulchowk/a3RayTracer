#include "Logger.h"
#include <Windows.h>
#include <stdio.h>
#include <stdarg.h>

#define MAX_LOG_MSG_SIZE 1024

namespace x {

void Log(const char* format, ...)
{
	char buffer[MAX_LOG_MSG_SIZE];
	va_list arg;
	va_start(arg, format);
	vsprintf_s(buffer, MAX_LOG_MSG_SIZE, format, arg);
	va_end(arg);
	OutputDebugStringA(buffer);
}

void LogError(const char* format, ...)
{
	char buffer[MAX_LOG_MSG_SIZE];
	va_list arg;
	va_start(arg, format);
	vsprintf_s(buffer, MAX_LOG_MSG_SIZE, format, arg);
	va_end(arg);
	OutputDebugStringA("[ERROR]: ");
	OutputDebugStringA(buffer);
}

void LogWarn(const char* format, ...)
{
	char buffer[MAX_LOG_MSG_SIZE];
	va_list arg;
	va_start(arg, format);
	vsprintf_s(buffer, MAX_LOG_MSG_SIZE, format, arg);
	va_end(arg);
	OutputDebugStringA("[WARNING]: ");
	OutputDebugStringA(buffer);
}

}