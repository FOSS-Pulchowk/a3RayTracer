#include "Common/Core.h"
#include <stdarg.h>

#define MAX_LOG_MSG_SIZE 1024

#include <Windows.h>

static HANDLE s_ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

namespace x {

// NOTE(Zero)
// When `display` is true, the buffer is flushed and `ch` is not displayed
// When log buffer gets full then it is automatically flushed
void InternalPutCharToBuffer(char ch, b32 display = false)
{
	static char s_LogBuffer[MAX_LOG_MSG_SIZE];
	static u32 s_LogBufferIndex;

	// Flush when true and not to display `ch`
	if(display)
	{
		DWORD written;
		WriteConsoleA(s_ConsoleHandle, s_LogBuffer, s_LogBufferIndex * sizeof(char), &written, 0);
		s_LogBufferIndex = 0;
		return;
	}

	// Need to flush because buffer is full
	if(s_LogBufferIndex == (MAX_LOG_MSG_SIZE - 2))
	{
		s_LogBuffer[s_LogBufferIndex + 1] = '\0';
		DWORD written;
		WriteConsoleA(s_ConsoleHandle, s_LogBuffer, s_LogBufferIndex * sizeof(char), &written, 0);
		s_LogBufferIndex = 0;
		return;
	}
	s_LogBuffer[s_LogBufferIndex++] = ch;
}

void InternalParseAndLogUnsignedInteger(u64 n, u32 b)
{
	if(n == 0)
	{
		InternalPutCharToBuffer('0');
		return;
	}
	char buffer[100] = {};
	i32 bufferIndex = 0;
	while(n != 0)
	{
		u64 rem = n % b;
		buffer[bufferIndex++] = (char)((rem > 9) ? (rem - 10) + 'a' : rem + '0');
		xAssert(bufferIndex < 100);
		n = n / b;
	}
	i32 start = 0;
	i32 end = bufferIndex - 1;
	while(start < end)
	{
		char temp = buffer[start];
		buffer[start] = buffer[end];
		buffer[end] = temp;
		start++;
		end--;
	}
	for(i32 i = 0; i < bufferIndex; ++i)
		InternalPutCharToBuffer(buffer[i]);
}

// HACK(Zero): May error when value are too large or too small
void InternalPraseAndLogFloat(u32 num)
{
	u32 sign = num >> 31;
	u32 exp = ((num >> 23) & 0xff) - 127;
	u32 man = num & ((1 << 23) - 1);
	man |= 1 << 23;
	if(sign)
		InternalPutCharToBuffer('-');
	InternalParseAndLogUnsignedInteger(man >> (23 - exp), 10);
	InternalPutCharToBuffer('.');
	u32 frac = man & ((1 << (23 - exp)) - 1);
	u32 base = 1 << (23 - exp);
	int c = 0;
	while(frac != 0 && c++ < 6)
	{
		frac *= 10;
		InternalParseAndLogUnsignedInteger((u32)(frac / base), 10);
		frac %= base;
	}
}

void InternalParseAndLogString(const char* string)
{
	for(i32 si = 0; string[si] != '\0'; ++si)
		InternalPutCharToBuffer(string[si]);
}

void Log(LogType type, const char* format, ...)
{
	switch(type)
	{
		case x::LogTypeStatus:
		{
			SetConsoleTextAttribute(s_ConsoleHandle, FOREGROUND_GREEN);
			InternalParseAndLogString("[STATUS]  ");
			break;
		}
		case x::LogTypeWarn:
		{
			SetConsoleTextAttribute(s_ConsoleHandle, FOREGROUND_RED | FOREGROUND_GREEN);
			InternalParseAndLogString("[WARNING] ");
			break;
		}
		case x::LogTypeError:
		{
			SetConsoleTextAttribute(s_ConsoleHandle, FOREGROUND_RED);
			InternalParseAndLogString("[ERROR]   ");
			break;
		}
		case x::LogTypeTrace:
		{
			SetConsoleTextAttribute(s_ConsoleHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			InternalParseAndLogString("[TRACE]   ");
			break;
		}
	}

	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	InternalParseAndLogString("[Time:");
	InternalParseAndLogUnsignedInteger(sysTime.wHour, 10);
	InternalParseAndLogString(":");
	InternalParseAndLogUnsignedInteger(sysTime.wMinute, 10);
	InternalParseAndLogString(":");
	InternalParseAndLogUnsignedInteger(sysTime.wSecond, 10);
	InternalParseAndLogString(":");
	InternalParseAndLogUnsignedInteger(sysTime.wMilliseconds, 10);
	InternalParseAndLogString("] [Thread:");
	InternalParseAndLogUnsignedInteger(GetCurrentThreadId(), 10);
	InternalParseAndLogString("] ");

	va_list arg;
	char* traverser;
	va_start(arg, format);
	for(traverser = (char*)format; *traverser != '\0'; ++traverser)
	{
		if(*traverser == '{')
		{
			if(*(traverser + 2) == '}')
			{
				switch(*(traverser + 1))
				{
					case 'c': // character
					{
						InternalPutCharToBuffer(*traverser);
						traverser += 2;
						break;
					}
					case 's': // string
					{
						InternalParseAndLogString(va_arg(arg, char*));
						traverser += 2;
						break;
					}
					case 'i': // integer
					{
						i32 num = va_arg(arg, i32);
						if(num < 0)
						{
							num = -num;
							InternalPutCharToBuffer('-');
						}
						u64 unum = (u64)num;
						InternalParseAndLogUnsignedInteger((u64)num, 10);
						traverser += 2;
						break;
					}
					case 'x': // integer to hex
					{
						InternalParseAndLogUnsignedInteger(va_arg(arg, u32), 16);
						traverser += 2;
						InternalPutCharToBuffer('h');
						break;
					}
					case 'o': // integer to oct
					{
						InternalParseAndLogUnsignedInteger(va_arg(arg, u32), 8);
						traverser += 2;
						InternalPutCharToBuffer('o');
						break;
					}
					case 'b': // integer to binary
					{
						InternalParseAndLogUnsignedInteger(va_arg(arg, u32), 2);
						traverser += 2;
						InternalPutCharToBuffer('b');
						break;
					}
					case 'u': // unsigned integer
					{
						InternalParseAndLogUnsignedInteger(va_arg(arg, u32), 10);
						traverser += 2;
						break;
					}
					case 'f': // floats
					{
						f32 num = (f32)va_arg(arg, f64);
						InternalPraseAndLogFloat(*((u32*)&num));
						traverser += 2;
						break;
					}
					default: // unknown
					{
						InternalPutCharToBuffer(*traverser);
						break;
					}
				}
			}
			else if(*(traverser + 3) == '}')
			{
				if(*(traverser + 1) == 'v')
				{
					switch(*(traverser + 2))
					{
						case '2': // v2
						{
							v2 vec = va_arg(arg, v2);
							InternalPutCharToBuffer('(');
							InternalPraseAndLogFloat(*((u32*)&vec.x));
							InternalPutCharToBuffer(',');
							InternalPraseAndLogFloat(*((u32*)&vec.y));
							InternalPutCharToBuffer(')');
							traverser += 3;
							traverser += 3;
							break;
						}
						case '3': // v3
						{
							v3 vec = va_arg(arg, v3);
							InternalPutCharToBuffer('(');
							InternalPraseAndLogFloat(*((u32*)&vec.x));
							InternalPutCharToBuffer(',');
							InternalPraseAndLogFloat(*((u32*)&vec.y));
							InternalPutCharToBuffer(',');
							InternalPraseAndLogFloat(*((u32*)&vec.z));
							InternalPutCharToBuffer(')');
							traverser += 3;
							break;
						}
						case '4': // v4
						{
							v4 vec = va_arg(arg, v4);
							InternalPutCharToBuffer('(');
							InternalPraseAndLogFloat(*((u32*)&vec.x));
							InternalPutCharToBuffer(',');
							InternalPraseAndLogFloat(*((u32*)&vec.y));
							InternalPutCharToBuffer(',');
							InternalPraseAndLogFloat(*((u32*)&vec.z));
							InternalPutCharToBuffer(',');
							InternalPraseAndLogFloat(*((u32*)&vec.w));
							InternalPutCharToBuffer(')');
							traverser += 3;
							traverser += 3;
							break;
						}
						default:
						{
							InternalPutCharToBuffer(*traverser);
							break;
						}
					}
				}
				else
				{
					InternalPutCharToBuffer(*traverser);
				}
			}
			else
			{
				InternalPutCharToBuffer(*traverser);
			}
		}
		else
		{
			InternalPutCharToBuffer(*traverser);
		}
	}
	va_end(arg);
	InternalPutCharToBuffer('\n');
	InternalPutCharToBuffer(0, true);
}

}