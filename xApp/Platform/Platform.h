#pragma once
#include "Common/Core.h"

// NOTE(Zero):
// This file contains declaration for all platform common stuffs
// The actual content of these declarations are present in platform spefcific files
// Identifiers prefixed with `x_` are not to be directly used by the outside
// Identifiers with prefix `x_` are defined in platform layer


//
// CONTAINS: Logging macros
//
namespace x {
	enum log_type
	{
		LogTypeStatus, LogTypeWarn, LogTypeError, LogTypeTrace
	};
	/*
		* NOTE(Zero): Current list of supported data types
		* char, string(s8 or char*)
		* i32(32 bit integers or smaller)
		* u32(32 bit unsigned integers or smaller)
		* f32(float) while f64(double) are converted into floats when displayed
		* v2, v3, v4
	*/
#if 0
	// USAGE(Zero) @ 12/30/2018 20:44
	// Don't use this function directly unless you have a good reason to
	//	and you know what you are doing
	// The format specifiers work the following way:
	xLog("Simple status message"); // display as is, break line is placed automatically at the end
	xLogError("Error message, code = {i}", 55); // displays `Error message, code = 55`
	// for hexadecimal display 'x' in place of 'i'
	// similarly 'o' for octal and 'b' for binary
	// 'u' is used for unsigned integer
	// 'f' for floats and doubles
	// 'c' for character and 's' for string
	// 'v2', 'v3' and 'v4' can be used for resp vectors, example:
	xLogWarn("This vec {v2} may cause errors!", { 5, 6, 7 });
#endif
}
void x_Log(x::log_type type, s8 format, ...);


//
// CONTAINS: Build macros
//
/*
	* NOTE(Zero)
	* XDEBUG = 4 : Log everything
	* XDEBUG = 3 : Log error and warning only
	* XDEBUG = 2 : Log errors only
	* XDEBUG = 1 : Internal build, add tweaks here
	* Otherwise build project as RELEASE BUILD
*/
#if XDEBUG == 4
#define xLog(fmt, ...)			x_Log(x::LogTypeStatus, fmt, ##__VA_ARGS__)
#define xLogError(fmt, ...)		x_Log(x::LogTypeError, fmt, ##__VA_ARGS__)
#define xLogWarn(fmt, ...)		x_Log(x::LogTypeWarn, fmt, ##__VA_ARGS__)
#define xLogTrace(fmt, ...)		x_Log(x::LogTypeTrace, fmt, ##__VA_ARGS__)
#elif XDEBUG == 3
#define xLog(fmt, ...)
#define xLogError(fmt, ...)		x_Log(x::LogTypeError, fmt, ##__VA_ARGS__)
#define xLogWarn(fmt, ...)		x_Log(x::LogTypeWarn, fmt, ##__VA_ARGS__)
#define xLogTrace(fmt, ...)		x_Log(x::LogTypeTrace, fmt, ##__VA_ARGS__)
#elif XDEBUG == 2
#define xLog(fmt, ...)
#define xLogError(fmt, ...)		x_Log(x::LogTypeError, fmt, ##__VA_ARGS__)
#define xLogWarn(fmt, ...)
#define xLogTrace(fmt, ...)		x_Log(x::LogTypeTrace, fmt, ##__VA_ARGS__)
#elif defined (XINTERNAL)
#define xLog(fmt, ...)			x_Log(x::LogTypeStatus, fmt, ##__VA_ARGS__)
#define xLogError(fmt, ...)		x_Log(x::LogTypeError, fmt, ##__VA_ARGS__)
#define xLogWarn(fmt, ...)		x_Log(x::LogTypeWarn, fmt, ##__VA_ARGS__)
#define xLogTrace(fmt, ...)		x_Log(x::LogTypeTrace, fmt, ##__VA_ARGS__)
#else
#define xLog(fmt, ...)
#define xLogError(fmt, ...)
#define xLogWarn(fmt, ...)
#define xLogTrace(fmt, ...)
#endif

//
// CONTAINS: OS Macros
//
#ifdef _WIN64

#define XOSWINDOWS

#endif

// TODO(Zero): Clean this
struct memory_arena
{
	u8* Memory;
	u32 Capacity;
	u8* Current;
	u32 Consumed;
};

inline void* PushSize(memory_arena& arena, u32 bytes)
{
	xAssert(bytes != 0);
	xAssert(arena.Capacity >= (arena.Consumed + bytes));
	void* result = (void*)arena.Current;
	arena.Current += bytes;
	return result;
}

#define xPush(arena, type) (type*)PushSize(arena, sizeof(type))
#define xPushArray(arena, type, n) (type*)PushSize(arena, sizeof(type) * n)

//
// CONTAINS: Platform specific stuffs
//
namespace x {
	struct file_content
	{
		void* Buffer;
		i64 Size;
	};
}

struct x_platform
{
	const x::file_content LoadFileContent(s8 fileName) const;
	void FreeFileContent(x::file_content fileReadInfo) const;
};
namespace x {
	extern const x_platform Platform;
}

namespace x {
	enum button
	{
		ButtonLeft,
		ButtonRight,
		ButtonMiddle,

		ButtonCount
	};

	enum state
	{
		ButtonUp,
		ButtonDown
	};
}

struct x_input_system
{
	i32 MouseX;
	i32 MouseY;
	x::state Buttons[x::ButtonCount];
};