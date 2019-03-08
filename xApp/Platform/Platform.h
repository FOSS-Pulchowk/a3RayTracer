#pragma once
#include "Common/Core.h"
#include "Utility/String.h"

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
}

#if defined(A3DEBUG) || defined(A3INTERNAL)

#define A3_MAXLOGMSGSIZE 1024

extern void a3_WriteConsole(const utf8* buffer, u32 length);
extern void a3_LogPrefix(const utf8* file, u32 line, a3::log_type type, utf8* buffer, u32* position);

inline void a3_Log(const utf8* file, u32 line, a3::log_type type, const utf8* msg)
{
	utf8 buffer[A3_MAXLOGMSGSIZE];
	u32 position = 0;

	a3_LogPrefix(file, line, type, buffer, &position);
	position += a3::StringPrintFormatted(&buffer[position], A3_MAXLOGMSGSIZE - position, msg);
	position += a3::StringPrintFormatted(&buffer[position], A3_MAXLOGMSGSIZE - position, "\n\n");

	a3_WriteConsole(buffer, position);
}

template <typename ...Args>
inline void a3_Log(const utf8* file, u32 line, a3::log_type type, const utf8* format, Args&& ...args)
{
	utf8 buffer[A3_MAXLOGMSGSIZE];
	u32 position = 0;

	a3_LogPrefix(file, line, type, buffer, &position);
	position += a3::StringPrintFormatted(&buffer[position], A3_MAXLOGMSGSIZE - position, format, args...);
	position += a3::StringPrintFormatted(&buffer[position], A3_MAXLOGMSGSIZE - position, "\n\n");

	a3_WriteConsole(buffer, position);
}

#endif

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
#define a3AspectRatio() (16.0f / 9.0f)
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
	enum file_type : u32
	{
		FileTypeAny = '0000',
		FileTypePNG = '.png',
		FileTypeOBJ = '.obj'
	};
	enum message_box_result
	{
		MessageBoxResultError = 0,
		MessageBoxResultAbort,
		MessageBoxResultCancel,
		MessageBoxResultContinue,
		MessageBoxResultIgnore,
		MessageBoxResultNo,
		MessageBoxResultOk,
		MessageBoxResultRetry,
		MessageBoxResultTryAgain,
		MessageBoxResultYes
	};
	enum message_box_type : u32
	{
		MessageBoxTypeAbortRetryIgnore,
		MessageBoxTypeCancelTryContinue,
		MessageBoxTypeHelp,
		MessageBoxTypeOk,
		MessageBoxTypeOkCancel,
		MessageBoxTypeRetryCancel,
		MessageBoxTypeYesNo,
		MessageBoxTypeYesNoCancel,
	};
	enum message_box_icon : u32
	{
		MessageBoxIconNone,
		MessageBoxIconExclamation,
		MessageBoxIconWarning,
		MessageBoxIconInformation,
		MessageBoxIconAsterisk,
		MessageBoxIconQuestion,
		MessageBoxIconStop,
		MessageBoxIconError,
		MessageBoxIconHand
	};
}

struct a3_platform
{
	const a3::file_content LoadFileContent(s8 fileName) const;
	void FreeFileContent(a3::file_content fileReadInfo) const;
	b32 WriteFileContent(s8 fileName, const a3::file_content& file) const;
	b32 ReplaceFileContent(s8 fileName, const a3::file_content& file) const;

	// NOTE(Zero):
	// These use generic heap allocators
	// These should be used for general and temporary purposes
#if defined(A3DEBUG) || defined(A3INTERNAL)
	void* Malloc(u64 size, s8 file, i32 line) const;
	void* Calloc(u64 size, s8 file, i32 line) const;
	void* Realloc(void* ptr, u64 size, s8 file, i32 line) const;
	void* Recalloc(void* ptr, u64 size, s8 file, i32 line) const;
#else
	void* Malloc(u64 size) const;
	void* Calloc(u64 size) const;
	void* Realloc(void* ptr, u64 size) const;
	void* Recalloc(void* ptr, u64 size) const;
#endif
	b32 Free(void* ptr) const;

	// NOTE(Zero):
	// Use these for persistancy application resources
#if defined(A3DEBUG) || defined(A3INTERNAL)
	void* AllocMemory(u64 size, s8 file, i32 line) const;
	void* ResizeMemory(void* usrPtr, u64 size, s8 file, i32 line) const;
#else
	void* AllocMemory(u64 size) const;
	void* ResizeMemory(void* usrPtr, u64 size) const;
#endif
	b32 Release(void* ptr) const;

	utf8* LoadFromDialogue(s8 title, a3::file_type type) const;
	utf8* SaveFromDialogue(s8 title, a3::file_type type) const;
	void FreeDialogueData(utf8* data) const;
	a3::message_box_result MessageBox(s8 title, s8 caption, a3::message_box_type type, a3::message_box_icon icon) const;


#if defined(A3DEBUG) || defined(A3INTERNAL)
	u64 GetTotalHeapAllocated() const;
	u64 GetTotalHeapFreed() const;
	u64 GetPersistantHeapAllocated() const;
	u64 GetPersistantHeapFreed() const;
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
#if defined(A3DEBUG) || defined(A3INTERNAL)
void* operator new(u64 size, s8 file, i32 line);
void* operator new[](u64 size, s8 file, i32 line);
#else
void* operator new(u64 size);
void* operator new[](u64 size);
#endif

void* operator new(u64 size, void* where);
void* operator new[](u64 size, void* where);

void operator delete(void* ptr);
void operator delete[](void* ptr);
void operator delete(void*, void*);
void operator delete[](void*, void*);

// Helper macros for memory allocations
// These should be prefered over other methods
#if defined(A3DEBUG) || defined(A3INTERNAL)
#define a3Malloc(size, type) (type*)a3::Platform.Malloc(size, __FILE__, __LINE__)
#define a3Realloc(ptr, size, type) (type*)a3::Platform.Realloc(ptr, size, __FILE__, __LINE__)
#define a3Calloc(size, type) (type*)a3::Platform.Calloc(size, __FILE__, __LINE__)
#define a3Recalloc(ptr, size, type) (type*)a3::Platform.Recalloc(ptr, size, __FILE__, __LINE__)
#define a3Free(ptr) a3::Platform.Free(ptr)
#define a3Allocate(size, type) (type*)a3::Platform.AllocMemory(size, __FILE__, __LINE__)
#define a3Reallocate(ptr, size, type) (type*)a3::Platform.ResizeMemory(ptr, size, __FILE__, __LINE__)
#define a3Release(ptr) a3::Platform.Release(ptr)
#define a3New new(__FILE__, __LINE__)
#define a3Place(where) new(where)
#define a3Delete delete
#else
#define a3Malloc(size, type) (type*)a3::Platform.Malloc(size)
#define a3Realloc(ptr, size, type) (type*)a3::Platform.Realloc(ptr, size)
#define a3Calloc(size, type) (type*)a3::Platform.Calloc(size)
#define a3Recalloc(ptr, size, type) (type*)a3::Platform.Recalloc(ptr, size)
#define a3Free(ptr) a3::Platform.Free(ptr)
#define a3Allocate(size, type) (type*)a3::Platform.AllocMemory(size)
#define a3Reallocate(ptr, size, type) (type*)a3::Platform.ResizeMemory(ptr, size)
#define a3Release(ptr) a3::Platform.Release(ptr)
#define a3New new
#define a3Place(where) new(where)
#define a3Delete delete
#endif

namespace a3 {
	enum button
	{
		ButtonUnknown,
		ButtonLeft,
		ButtonRight,
		ButtonMiddle,

		ButtonCount
	};

	enum key
	{
		KeyUnknown,
		KeyW,
		KeyA,
		KeyS,
		KeyD,
		KeyUp,
		KeyDown,
		KeyRight,
		KeyLeft,
		KeyRaytrace,

		KeyCount
	};

	struct key_state
	{
		b32 Down;
		b32 Up;
		b32 Pressed;
		i32 Repeats;
		b32 Extended;
	};

	struct button_state
	{
		b32 Down;
		b32 Up;
	};

	struct input_info
	{
		f32 MouseX;
		f32 MouseY;
		i32 WindowWidth;
		i32 WindowHeight;
		button_state Buttons[a3::ButtonCount];
		key_state Keys[a3::KeyCount];
	};
}
