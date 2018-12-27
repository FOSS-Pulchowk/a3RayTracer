#include "Logger.h"
#include <Windows.h>
#include <stdio.h>
#include <stdarg.h>

namespace x {

void Log(const wchar_t* format, ...)
{
	wchar_t buffer[500];
	va_list arg;
	va_start(arg, format);
	vswprintf_s(buffer, 500, format, arg);
	va_end(arg);
	OutputDebugStringW(buffer);
}

void LogError(const wchar_t* format, ...)
{
	wchar_t buffer[500];
	va_list arg;
	va_start(arg, format);
	vswprintf_s(buffer, 500, format, arg);
	va_end(arg);
	OutputDebugStringW(L"[ERROR]: ");
	OutputDebugStringW(buffer);
}

void LogWarn(const wchar_t* format, ...)
{
	wchar_t buffer[500];
	va_list arg;
	va_start(arg, format);
	vswprintf_s(buffer, 500, format, arg);
	va_end(arg);
	OutputDebugStringW(L"[WARNING]: ");
	OutputDebugStringW(buffer);
}

}