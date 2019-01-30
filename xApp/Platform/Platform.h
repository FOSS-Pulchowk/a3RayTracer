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
namespace a3 {
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
void a3_Log(s8 file, u32 line, a3::log_type type, s8 format, ...);


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
#if A3DEBUG == 4
#define a3Log(fmt, ...)			a3_Log(__FILE__, __LINE__, a3::LogTypeStatus, fmt, ##__VA_ARGS__)
#define a3LogError(fmt, ...)	a3_Log(__FILE__, __LINE__, a3::LogTypeError, fmt, ##__VA_ARGS__)
#define a3LogWarn(fmt, ...)		a3_Log(__FILE__, __LINE__, a3::LogTypeWarn, fmt, ##__VA_ARGS__)
#define a3LogTrace(fmt, ...)	a3_Log(__FILE__, __LINE__, a3::LogTypeTrace, fmt, ##__VA_ARGS__)
#elif A3DEBUG == 3
#define a3Log(fmt, ...)
#define a3LogError(fmt, ...)	a3_Log(__FILE__, __LINE__, a3::LogTypeError, fmt, ##__VA_ARGS__)
#define a3LogWarn(fmt, ...)		a3_Log(__FILE__, __LINE__, a3::LogTypeWarn, fmt, ##__VA_ARGS__)
#define a3LogTrace(fmt, ...)	a3_Log(__FILE__, __LINE__, a3::LogTypeTrace, fmt, ##__VA_ARGS__)
#elif A3DEBUG == 2
#define a3Log(fmt, ...)
#define a3LogError(fmt, ...)	a3_Log(__FILE__, __LINE__, a3::LogTypeError, fmt, ##__VA_ARGS__)
#define a3LogWarn(fmt, ...)
#define a3LogTrace(fmt, ...)	a3_Log(__FILE__, __LINE__, a3::LogTypeTrace, fmt, ##__VA_ARGS__)
#elif defined (A3INTERNAL)
#define a3Log(fmt, ...)			a3_Log(__FILE__, __LINE__, a3::LogTypeStatus, fmt, ##__VA_ARGS__)
#define a3LogError(fmt, ...)	a3_Log(__FILE__, __LINE__, a3::LogTypeError, fmt, ##__VA_ARGS__)
#define a3LogWarn(fmt, ...)		a3_Log(__FILE__, __LINE__, a3::LogTypeWarn, fmt, ##__VA_ARGS__)
#define a3LogTrace(fmt, ...)	a3_Log(__FILE__, __LINE__, a3::LogTypeTrace, fmt, ##__VA_ARGS__)
#else
#define a3Log(fmt, ...)
#define a3LogError(fmt, ...)
#define a3LogWarn(fmt, ...)
#define a3LogTrace(fmt, ...)
#endif

//
// CONTAINS: OS Macros
//
#ifdef _WIN64

#define A3OSWINDOWS

#endif

//
// CONTAINS: Screen Macros
#define a3AspectRatio (16.0f / 9.0f)
#define a3AspectHeight(width) ((width) / a3AspectRatio)
#define a3AspectWidth(height) (a3AspectRatio * (height))

// TODO(Zero):
// Clean this
// Make proper allocators for different systems for the application
struct memory_arena
{
	u8* Memory;
	u32 Capacity;
	u8* Current;
	u32 Consumed;
};

// TODO(Zero): This need to be cleaned
inline void* PushSize(memory_arena& arena, u32 bytes)
{
	a3Assert(bytes != 0);
	a3Assert(arena.Capacity >= (arena.Consumed + bytes));
	void* result = (void*)arena.Current;
	arena.Current += bytes;
	arena.Consumed += bytes;
	return result;
}

#define a3Push(arena, type) (type*)PushSize(arena, sizeof(type))
#define a3PushArray(arena, type, n) (type*)PushSize(arena, sizeof(type) * n)

//
// CONTAINS: Platform specific stuffs
//
namespace a3 {
	struct file_content
	{
		void* Buffer;
		u64 Size;
	};
	enum file_type
	{
		FileTypeAny,
		FileTypePNG,
		FileTypeOBJ
	};
}

struct a3_platform
{
	const a3::file_content LoadFileContent(s8 fileName) const;
	void FreeFileContent(a3::file_content fileReadInfo) const;
	b32 WriteFileContent(s8 fileName, const a3::file_content& file) const;
	b32 ReplaceFileContent(s8 fileName, const a3::file_content& file) const;

	void* Malloc(u64 size) const;
	void* Calloc(u64 size) const;
	void* Realloc(void* ptr, u64 size) const;
	void* Recalloc(void* ptr, u64 size) const;
	b32 Free(void* ptr) const;

	utf8* LoadFromDialogue(s8 title, a3::file_type type) const;
	void FreeDialogueData(utf8* data) const;

#if defined(A3DEBUG) || defined(A3INTERNAL)
	u64 GetTotalHeapAllocated() const;
	u64 GetTotalHeapFreed() const;
#endif
};
namespace a3 {
	extern const a3_platform Platform;
}

//
// NOTE(Zero):
// C++ new and delete operator override
// Should be defined in platform layer
//
void* operator new(u64 size);
void operator delete(void* ptr);

namespace a3 {
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

struct a3_input_system
{
	i32 MouseX;
	i32 MouseY;
	a3::state Buttons[a3::ButtonCount];
};
