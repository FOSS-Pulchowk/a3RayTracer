#pragma once

//
// CONTAINS: Logging macros
// NOTE(Zero): Logging functions are declared here only and are implemented in platform layer
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

void Log(log_type type, s8 format, ...);

}


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
#define xLog(fmt, ...)			x::Log(x::LogTypeStatus, fmt, ##__VA_ARGS__)
#define xLogError(fmt, ...)		x::Log(x::LogTypeError, fmt, ##__VA_ARGS__)
#define xLogWarn(fmt, ...)		x::Log(x::LogTypeWarn, fmt, ##__VA_ARGS__)
#define xLogTrace(fmt, ...)		x::Log(x::LogTypeTrace, fmt, ##__VA_ARGS__)
#elif XDEBUG == 3
#define xLog(fmt, ...)
#define xLogError(fmt, ...)		x::Log(x::LogTypeError, fmt, ##__VA_ARGS__)
#define xLogWarn(fmt, ...)		x::Log(x::LogTypeWarn, fmt, ##__VA_ARGS__)
#define xLogTrace(fmt, ...)		x::Log(x::LogTypeTrace, fmt, ##__VA_ARGS__)
#elif XDEBUG == 2
#define xLog(fmt, ...)
#define xLogError(fmt, ...)		x::Log(x::LogTypeError, fmt, ##__VA_ARGS__)
#define xLogWarn(fmt, ...)
#define xLogTrace(fmt, ...)		x::Log(x::LogTypeTrace, fmt, ##__VA_ARGS__)
#elif defined (XINTERNAL)
#define xLog(fmt, ...)			x::Log(x::LogTypeStatus, fmt, ##__VA_ARGS__)
#define xLogError(fmt, ...)		x::Log(x::LogTypeError, fmt, ##__VA_ARGS__)
#define xLogWarn(fmt, ...)		x::Log(x::LogTypeWarn, fmt, ##__VA_ARGS__)
#define xLogTrace(fmt, ...)		x::Log(x::LogTypeTrace, fmt, ##__VA_ARGS__)
#else
#define xLog(fmt, ...)
#define xLogError(fmt, ...)
#define xLogWarn(fmt, ...)
#define xLogTrace(fmt, ...)
#endif

//
//#ifndef GLAPI
//# if defined(GLAD_GLAPI_EXPORT)
//#  if defined(_WIN32) || defined(__CYGWIN__)
//#   if defined(GLAD_GLAPI_EXPORT_BUILD)
//#    if defined(__GNUC__)
//#     define GLAPI __attribute__ ((dllexport)) extern
//#    else
//#     define GLAPI __declspec(dllexport) extern
//#    endif
//#   else
//#    if defined(__GNUC__)
//#     define GLAPI __attribute__ ((dllimport)) extern
//#    else
//#     define GLAPI __declspec(dllimport) extern
//#    endif
//#   endif
//#  elif defined(__GNUC__) && defined(GLAD_GLAPI_EXPORT_BUILD)
//#   define GLAPI __attribute__ ((visibility ("default"))) extern
//#  else
//#   define GLAPI extern
//#  endif
//# else
//#  define GLAPI extern
//# endif
//#endif