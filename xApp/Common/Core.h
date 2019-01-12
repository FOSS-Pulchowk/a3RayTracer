#pragma once

#define null 0

// Bit operations Macro
#define xBit(pos)				(1 << (pos))
#define xSetBit(n, b)			((n) |= (b))
#define xGetBit(n, b)			((n) & (b))
#define xClearBit(n, b)			((n) &= (~b))
#define xToggleBit(n, b)		((n) ^= (b))

#ifdef XBIGENDIAN
#define xSwapEndian32(n) (n)
#else
#define xSwapEndian32(n) (((n)>>24) | (((n) & 0x00ff0000) >> 8) | (((n) & 0x0000ff00) << 8) | ((n)<<24))
#endif
#define xPack32(a, b, c, d) (((a)<<24) | ((b)<<16) | ((c)<<8) | ((d)<<0))
#define xConsumeBits(n, p, b) (((n) & (((1 << (b)) - 1) << (p))) >> (p))

#define xArrayCount(arr) (sizeof(arr)/sizeof((arr)[0]))
#define xOffsetOf(s, m) (&(((s*)0)->m))

#define xKiloBytes(n) (sizeof(u8) * (n) * 1024u)
#define xMegaBytes(n) (xKiloBytes(n) * 1024u)
#define xGigaBytes(n) (xMegaBytes(n) * 1024u)

// Debug Macro
#if defined(_MSC_VER)
#define xAssert(x) if(!(x))		{ __debugbreak(); }
#define xTriggerBreakPoint()	{ __debugbreak(); }
#else // just write to nullptr to hit break
#define xAssert(x) if(!(x))		{ *((int*)(0)) = 0; }
#define xTriggerBreakPoint()	{ *((int*)(0)) = 0; }
#endif

#include <stdint.h>

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

#define min_i8 INT8_MIN
#define max_i8 INT8_MAX
#define min_i16 INT16_MIN
#define max_i16 INT16_MAX
#define min_i32 INT32_MIN
#define max_i32 INT32_MAX
#define min_i64 INT64_MIN
#define max_i64 INT64_MAX

#define max_u8 UINT8_MAX
#define max_u16 UINT16_MAX
#define max_u32 UINT32_MAX
#define max_u64 UINT64_MAX

typedef const char* s8;
typedef const wchar_t* s16;

#ifndef XSTDLIB
#include <xmmintrin.h>
// TODO(Zero): Remove this, self implement
#include <math.h>

inline f32 xSqrtf(f32 x)
{
	__m128 mx = _mm_set_ps(0, 0, 0, x);
	mx = _mm_sqrt_ps(mx);
	_mm_store1_ps(&x, mx);
	return x;
}

inline f32 xPowf(f32 n, f32 p, f32 precision = 0.000001f)
{
	if (p < 0) return 1 / xPowf(n, -p);
	if (p >= 10) return xSqrtf(xPowf(n, p / 2, precision / 2));
	if (p >= 1) return (n * xPowf(n, p - 1, precision));
	if (precision >= 1) return xSqrtf(n);
	return xSqrtf(xPowf(n, p * 2, precision * 2));
}

inline f32 xSquaref(f32 n)
{
	return (n * n);
}

inline f32 xSinf(f32 n)
{
	return sinf(n);
}

inline f32 xCosf(f32 n)
{
	return cosf(n);
}

inline f32 xTanf(f32 n)
{
	return tanf(n);
}

inline f32 xFAbsf(f32 n)
{
	return fabsf(n);
}

inline f32 xASinf(f32 n)
{
	return asinf(n);
}

inline f32 xATan2f(f32 y, f32 x)
{
	return atan2f(y, x);
}

inline f32 xCopySignf(f32 a, f32 b)
{
	if (b > 0.0f) return (a > 0.0f) ? a : -a;
	return (a > 0.0f) ? -a : a;
}

#else

#include <math.h>

inline f32 xSqrtf(f32 x)
{
	return sqrtf(x);
}

inline f32 xPowf(f32 n, f32 p, f32 precision = 0.000001f)
{
	return powf(n, p);
}

inline f32 xSquaref(f32 n)
{
	return powf(n, 2.0f);
}

inline f32 xSinf(f32 n)
{
	return sinf(n);
}

inline f32 xCosf(f32 n)
{
	return cosf(n);
}

inline f32 xTanf(f32 n)
{
	return tanf(n);
}

inline f32 xFAbsf(f32 n)
{
	return fabsf(n);
}

inline f32 xASinf(f32 n)
{
	return asinf(n);
}

inline f32 xATan2f(f32 y, f32 x)
{
	return atan2f(y, x);
}

inline f32 xCopySignf(f32 a, f32 b)
{
	return copysignf(a, b);
}

#endif


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
		struct { v2 xy; f32 z; };
		struct { f32 x; v2 yz; };
		f32 values[3];
	};
};

struct v4
{
	union
	{
		struct { f32 x, y, z, w; };
		struct { f32 r, g, b, a; };
		struct { v2 xy, zw; };
		struct { v3 xyz; f32 w; };
		struct { f32 x; v3 yzw; };
		f32 values[4];
	};
};


////////////////// Vector 2 operations ///////////////////////////////////////////

inline v2 operator-(v2 v)
{
	v2 result = {};
	result.x = -v.x;
	result.y = -v.y;
	return result;
}

inline f32 Length(v2 vec)
{
	return xSqrtf(vec.x * vec.x + vec.y * vec.y);
}

inline f32 Distance2(v2 lhs, v2 rhs)
{
	return xSquaref(lhs.x - rhs.x) + xSquaref(lhs.y - rhs.y);
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

inline v3 operator-(v3 v)
{
	v3 result = {};
	result.x = -v.x;
	result.y = -v.y;
	result.z = -v.z;
	return result;
}

inline f32 Length(v3 vec)
{
	return xSqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

inline f32 Distance2(v3 lhs, v3 rhs)
{
	return xSquaref(lhs.x - rhs.x) + xSquaref(lhs.y - rhs.y) + xSquaref(lhs.z - rhs.z);
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

inline v4 operator-(v4 v)
{
	v4 result = {};
	result.x = -v.x;
	result.y = -v.y;
	result.z = -v.z;
	result.w = -v.w;
	return result;
}

inline f32 Length(v4 vec)
{
	return xSqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z + vec.w * vec.w);
}

inline f32 Distance2(v4 lhs, v4 rhs)
{
	return xSquaref(lhs.x - rhs.x) + xSquaref(lhs.y - rhs.y) + xSquaref(lhs.z - rhs.z) + xSquaref(lhs.w - rhs.w);
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
