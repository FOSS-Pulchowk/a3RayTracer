#pragma once

#define null 0

// Bit operations Macro
#define xBit(pos)				(1 << pos)
#define xSetBit(n, b)			(n |= b)
#define xGetBit(n, b)			(n & b)
#define xClearBit(n, b)			(n &= (~b))
#define xToggleBit(n, b)		(n ^= b)

// Debug Macro
#define xAssert(x) if(!(x))		{ *((int*)(0)) = 0; }
#define xTriggerBreakPoint()	{ *((int*)(0)) = 0; }

/*
	* NOTE(Zero)
	* XDEBUG = 4 : Log everything
	* XDEBUG = 3 : Log error and warning only
	* XDEBUG = 2 : Log errors only
	* XDEBUG = 1 : Internal build, add tweaks here
	* Otherwise build project as RELEASE BUILD
*/

#include "Platform/Logger.h"

#if XDEBUG == 4

#define xLog(fmt, ...)			x::Log(fmt, ##__VA_ARGS__)
#define xLogError(fmt, ...)		x::LogError(fmt, ##__VA_ARGS__)
#define xLogWarn(fmt, ...)		x::LogWarn(fmt, ##__VA_ARGS__)

#elif XDEBUG == 3

#define xLog(fmt, ...)
#define xLogError(fmt, ...)		x::LogError(fmt, ##__VA_ARGS__)
#define xLogWarn(fmt, ...)		x::LogWarn(fmt, ##__VA_ARGS__)

#elif XDEBUG == 2
#define xLog(fmt, ...)
#define xLogError(fmt, ...)		x::LogError(fmt, ##__VA_ARGS__)
#define xLogWarn(fmt, ...)

#elif defined (XINTERNAL)

#define xLog(fmt, ...)			x::Log(fmt, ##__VA_ARGS__)
#define xLogError(fmt, ...)		x::LogError(fmt, ##__VA_ARGS__)
#define xLogWarn(fmt, ...)		x::LogWarn(fmt, ##__VA_ARGS__)
#else

#define xLog(fmt, ...)
#define xLogError(fmt, ...)
#define xLogWarn(fmt, ...)

#endif

#include <stdint.h>
// TODO(Zero): Make our own
#include <math.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef int32_t b32;
typedef int64_t b64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

typedef char utf8;
typedef wchar_t utf16;

typedef unsigned char uutf8;

typedef const char* s8;
typedef const wchar_t* s16;

// v2, v3 and v4 data types are treated as basic types

struct v2
{
	union
	{
		struct { f32 x, y; };
		struct { f32 u, v; };
		f32 values[2];
	};
};

struct v3
{
	union
	{
		struct { f32 x, y, z; };
		struct { f32 u, v, w; };
		struct { f32 r, g, b; };
		f32 values[3];
	};
};

struct v4
{
	union
	{
		struct { f32 x, y, z, w; };
		struct { f32 r, g, b, a; };
		f32 values[4];
	};
};


////////////////// Vector 2 operations ///////////////////////////////////////////

inline v2 operator-(v2& v)
{
	v2 result = {};
	result.x = -v.x;
	result.y = -v.y;
	return result;
}

inline f32 Length(v2 vec)
{
	return sqrtf(vec.x * vec.x + vec.y * vec.y);
}

inline f32 Distance2(v2 lhs, v2 rhs)
{
	return powf(lhs.x - rhs.x, 2.0f) + powf(lhs.y - rhs.y, 2.0f);
}

inline f32 Dot(v2 lhs, v2 rhs)
{
	return(lhs.x * rhs.x + lhs.y * rhs.y);
}

inline v3 Cross(v2 lhs, v2 rhs)
{
	v3 result = {};
	result.z = lhs.x * rhs.y - lhs.y * rhs.x;
	return result;
}

inline v2 Normalize(v2 vec)
{
	float inv_length = 1.0f / Length(vec);
	v2 result;
	result.x = vec.x * inv_length;
	result.y = vec.y * inv_length;
	return result;
}

inline v2& operator*=(v2& v, f32 scalar)
{
	v.x *= scalar;
	v.y *= scalar;
	return v;
}

inline v2& operator+=(v2& v, v2 vec)
{
	v.x += vec.x;
	v.y += vec.y;
	return v;
}

inline v2& operator-=(v2& v, v2 vec)
{
	v.x -= vec.x;
	v.y -= vec.y;
	return v;
}

inline v2 operator*(v2 vec, f32 scalar)
{
	v2 result;
	result.x = scalar * vec.x;
	result.y = scalar * vec.y;
	return result;
}

inline v2 operator*(f32 scalar, v2 vec)
{
	v2 result;
	result.x = scalar * vec.x;
	result.y = scalar * vec.y;
	return result;
}

inline v2 operator*(v2 vec1, v2 vec2)
{
	v2 result;
	result.x = vec1.x * vec2.x;
	result.y = vec1.y * vec2.y;
	return result;
}

inline v2 operator+(v2 lhs, v2 rhs)
{
	v2 result;
	result.x = lhs.x + rhs.x;
	result.y = lhs.y + rhs.y;
	return result;
}

inline v2 operator-(v2 lhs, v2 rhs)
{
	v2 result;
	result.x = lhs.x - rhs.x;
	result.y = lhs.y - rhs.y;
	return result;
}

inline b32 operator==(v2 lhs, v2 rhs)
{
	return (lhs.x == rhs.x) && (lhs.y == rhs.y);
}

inline b32 operator!=(v2 lhs, v2 rhs)
{
	return !(lhs == rhs);
}

/////////////////////// Vector 3 implementation ///////////////////////////////////

inline v3 operator-(v3& v)
{
	v3 result = {};
	result.x = -v.x;
	result.y = -v.y;
	result.z = -v.z;
	return result;
}

inline f32 Length(v3 vec)
{
	return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

inline f32 Distance2(v3 lhs, v3 rhs)
{
	return powf(lhs.x - rhs.x, 2.0f) + powf(lhs.y - rhs.y, 2.0f) + powf(lhs.z - rhs.z, 2.0f);
}

inline f32 Dot(v3 lhs, v3 rhs)
{
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

inline v3 Cross(v3 lhs, v3 rhs)
{
	v3 result;
	result.x = lhs.y * rhs.z - lhs.z * rhs.y;
	result.y = lhs.z * rhs.x - lhs.x * rhs.z;
	result.z = lhs.x * rhs.y - lhs.y * rhs.x;
	return result;
}

inline v3 Normalize(v3 vec)
{
	float inverse_length(1.0f / Length(vec));
	v3 result;
	result.x = vec.x * inverse_length;
	result.y = vec.y * inverse_length;
	result.z = vec.z * inverse_length;
	return result;
}

inline v3& operator*=(v3& v, f32 scalar)
{
	v.x *= scalar;
	v.y *= scalar;
	v.z *= scalar;
	return v;
}

inline v3& operator+=(v3& v, v3 vec)
{
	v.x += vec.x;
	v.y += vec.y;
	v.z += vec.z;
	return v;
}

inline v3& operator-=(v3& v, v3 vec)
{
	v.x -= vec.x;
	v.y -= vec.y;
	v.z -= vec.z;
	return v;
}

inline v3 operator*(v3 vec, f32 scalar)
{
	v3 result;
	result.x = vec.x * scalar;
	result.y = vec.y * scalar;
	result.z = vec.z * scalar;
	return result;
}

inline v3 operator*(f32 scalar, v3 vec)
{
	v3 result;
	result.x = vec.x * scalar;
	result.y = vec.y * scalar;
	result.z = vec.z * scalar;
	return result;
}

inline v3 operator*(v3 lhs, v3 rhs)
{
	v3 result;
	result.x = lhs.x * rhs.x;
	result.y = lhs.y * rhs.y;
	result.z = lhs.z * rhs.z;
	return result;
}

inline v3 operator+(v3 lhs, v3 rhs)
{
	v3 result;
	result.x = lhs.x + rhs.x;
	result.y = lhs.y + rhs.y;
	result.z = lhs.z + rhs.z;
	return result;
}

inline v3 operator-(v3 lhs, v3 rhs)
{
	v3 result;
	result.x = lhs.x - rhs.x;
	result.y = lhs.y - rhs.y;
	result.z = lhs.z - rhs.z;
	return result;
}

inline b32 operator==(v3 lhs, v3 rhs)
{
	return (lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z);
}

inline b32 operator!=(v3 lhs, v3 rhs)
{
	return !(lhs == rhs);
}

//////////////////////// Vector 4 implementation ///////////////////////////////////

inline v4 operator-(v4& v)
{
	v4 result = {};
	result.x = -v.x;
	result.y = -v.y;
	result.z = -v.z;
	result.w = -v.w;
	return result;
}

inline f32 Length(v4& vec)
{
	return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z + vec.w * vec.w);
}

inline f32 Distance2(v4 lhs, v4 rhs)
{
	return powf(lhs.x - rhs.x, 2.0f) + powf(lhs.y - rhs.y, 2.0f) + powf(lhs.z - rhs.z, 2.0f) + powf(lhs.w - rhs.w, 2.0f);
}

inline v3 GetXYZ(v4 vec)
{
	v3 result;
	result.x = vec.x;
	result.y = vec.y;
	result.z = vec.z;
	return result;
}

inline f32 Dot(v4 lhs, v4 rhs)
{
	return (
		lhs.x * rhs.x +
		lhs.y * rhs.y +
		lhs.z * rhs.z +
		lhs.w * rhs.w
		);
}

inline v4 Cross(v4 lhs, v4 rhs)
{
	v4 result;
	result.x = lhs.y*rhs.z - rhs.y*lhs.z;
	result.y = lhs.z*rhs.x - rhs.z*lhs.x;
	result.z = lhs.x*rhs.y - rhs.x*lhs.y;
	result.w = 1.0f;
	return result;
}

inline v4 Normalize(v4 vec)
{
	float inv_length = 1.0f / Length(vec);
	v4 result;
	result.x = vec.x * inv_length;
	result.y = vec.y * inv_length;
	result.z = vec.z * inv_length;
	result.w = vec.w * inv_length;
	return result;
}

inline v4& operator*=(v4& v, f32 scalar)
{
	v.x *= scalar;
	v.y *= scalar;
	v.z *= scalar;
	v.w *= scalar;
	return v;
}

inline v4& operator+=(v4& v, v4 vec)
{
	v.x += vec.x;
	v.y += vec.y;
	v.z += vec.z;
	v.w += vec.w;
	return v;
}

inline v4& operator-=(v4& v, v4 vec)
{
	v.x -= vec.x;
	v.y -= vec.y;
	v.z -= vec.z;
	v.w -= vec.w;
	return v;
}

inline v4 operator*(v4 vec, f32 scalar)
{
	v4 result;
	result.x = scalar * vec.x;
	result.y = scalar * vec.y;
	result.z = scalar * vec.z;
	result.w = scalar * vec.w;
	return result;
}

inline v4 operator*(f32 scalar, v4 vec)
{
	v4 result;
	result.x = scalar * vec.x;
	result.y = scalar * vec.y;
	result.z = scalar * vec.z;
	result.w = scalar * vec.w;
	return result;
}

inline v4 operator*(v4 lhs, v4 rhs)
{
	v4 result;
	result.x = lhs.x * rhs.x;
	result.y = lhs.y * rhs.y;
	result.z = lhs.z * rhs.z;
	result.w = lhs.w * rhs.w;
	return result;
}

inline v4 operator+(v4 lhs, v4 rhs)
{
	v4 result;
	result.x = lhs.x + rhs.x;
	result.y = lhs.y + rhs.y;
	result.z = lhs.z + rhs.z;
	result.w = lhs.w + rhs.w;
	return result;
}

inline v4 operator-(v4 lhs, v4 rhs)
{
	v4 result;
	result.x = lhs.x - rhs.x;
	result.y = lhs.y - rhs.y;
	result.z = lhs.z - rhs.z;
	result.w = lhs.w - rhs.w;
	return result;
}

inline b32 operator==(v4 lhs, v4 rhs)
{
	return (lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z) && (lhs.w == rhs.w);
}

inline b32 operator!=(v4 lhs, v4 rhs)
{
	return !(lhs == rhs);
}
