#include "Common/Core.h"
#include "Utility/String.h"
#include "Platform/Platform.h"
#include <Windows.h>

#if defined(A3DEBUG) || defined(A3INTERNAL)

static HANDLE s_ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

void a3_WriteConsole(const utf8* buffer, u32 length)
{
	DWORD written;
	WriteConsoleA(s_ConsoleHandle, buffer, length * sizeof(utf8), &written, 0);
}

void a3_LogPrefix(const utf8* file, u32 line, a3::log_type type, utf8* buffer, u32* position)
{
	switch (type)
	{
	case a3::LogTypeStatus:
	{
		SetConsoleTextAttribute(s_ConsoleHandle, FOREGROUND_GREEN);
		*position += a3::StringPrint(&buffer[*position], A3_MAXLOGMSGSIZE - *position, "[STATUS]  ");
		break;
	}
	case a3::LogTypeWarn:
	{
		SetConsoleTextAttribute(s_ConsoleHandle, FOREGROUND_RED | FOREGROUND_GREEN);
		*position += a3::StringPrint(&buffer[*position], A3_MAXLOGMSGSIZE - *position, "[WARNING] ");
		break;
	}
	case a3::LogTypeError:
	{
		SetConsoleTextAttribute(s_ConsoleHandle, FOREGROUND_RED);
		*position += a3::StringPrint(&buffer[*position], A3_MAXLOGMSGSIZE - *position, "[ERROR]   ");
		break;
	}
	case a3::LogTypeTrace:
	{
		SetConsoleTextAttribute(s_ConsoleHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		*position += a3::StringPrint(&buffer[*position], A3_MAXLOGMSGSIZE - *position, "[TRACE]   ");
		break;
	}
	}

	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	*position += a3::StringPrint(&buffer[*position], A3_MAXLOGMSGSIZE - *position, "[Time:");
	*position += a3::StringPrint(&buffer[*position], A3_MAXLOGMSGSIZE - *position, sysTime.wHour);
	*position += a3::StringPrint(&buffer[*position], A3_MAXLOGMSGSIZE - *position, ":");
	*position += a3::StringPrint(&buffer[*position], A3_MAXLOGMSGSIZE - *position, sysTime.wMinute);
	*position += a3::StringPrint(&buffer[*position], A3_MAXLOGMSGSIZE - *position, ":");
	*position += a3::StringPrint(&buffer[*position], A3_MAXLOGMSGSIZE - *position, sysTime.wSecond);
	*position += a3::StringPrint(&buffer[*position], A3_MAXLOGMSGSIZE - *position, ":");
	*position += a3::StringPrint(&buffer[*position], A3_MAXLOGMSGSIZE - *position, sysTime.wMilliseconds);
	*position += a3::StringPrint(&buffer[*position], A3_MAXLOGMSGSIZE - *position, "] [Thread:");
	*position += a3::StringPrint(&buffer[*position], A3_MAXLOGMSGSIZE - *position, (u64)GetCurrentThreadId());
	*position += a3::StringPrint(&buffer[*position], A3_MAXLOGMSGSIZE - *position, "] [File:");
	*position += a3::StringPrint(&buffer[*position], A3_MAXLOGMSGSIZE - *position, file);
	*position += a3::StringPrint(&buffer[*position], A3_MAXLOGMSGSIZE - *position, "] [Line:");
	*position += a3::StringPrint(&buffer[*position], A3_MAXLOGMSGSIZE - *position, line);
	*position += a3::StringPrint(&buffer[*position], A3_MAXLOGMSGSIZE - *position, "]\n");
}

#endif