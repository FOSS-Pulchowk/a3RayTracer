#pragma once

#include "Common/Core.h"
#include "Platform/Platform.h"
#include "Utility/String.h"

// We will only be supporting PNG currently
// TODO(Zero): Correctly remove all STD libs from here
#define STBI_ONLY_PNG
#define STBI_NO_GIF
#define STBI_NO_STDIO
#define STBI_ASSERT(s)		a3Assert(s)
// TODO(Zero):
// Can't figure out why these are not working!?
// Figure this out
//#define STBI_MALLOC(s)		a3::Platform.Malloc((u32)s)
//#define STBI_REALLOC(p, s)	a3::Platform.Realloc(p, (u32)s)
//#define STBI_FREE(p)		a3::Platform.Free(p)
#define STB_IMAGE_IMPLEMENTATION
#include "External/STBImage.h"

#define STBTT_ifloor(x)		((i32) a3Floorf(x))
#define STBTT_iceil(x)		((i32) a3Ceilf(x))
#define STBTT_sqrt(x)		a3Sqrtf(x)
#define STBTT_pow(x,y)		a3Powf(x,y)
#define STBTT_fmod(x,y)		a3FModf(x,y)
#define STBTT_cos(x)		a3Cosf(x)
#define STBTT_acos(x)		a3ACosf(x)
#define STBTT_fabs(x)		a3FAbsf(x)
#define STBTT_strlen(x)		a3::GetStringLength(x)
#define STBTT_memcpy		a3::MemoryCopy
#define STBTT_memset		a3::MemorySet
#define STBTT_malloc(x, u)	((void)(u),a3::Platform.Malloc(x))
#define STBTT_free(x, u)	((void)(u),a3::Platform.Free(x))
#define STBTT_assert(x)		a3Assert(x)
#define STB_TRUETYPE_IMPLEMENTATION
#include "External/STBTrueType.h"

#define STBI_MSC_SECURE_CRT
#define STBIW_ASSERT(x) a3Assert(x)
#define STBIW_MALLOC(s) a3::Platform.Malloc(s)
#define STBIW_REALLOC(p, s) a3::Platform.Realloc(p, s)
#define STBIW_FREE(p) a3::Platform.Free(p)
#define STBIW_MEMMOVE a3::MemoryMove
#define STBI_WRITE_NO_STDIO
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "External/STBImageWrite.h"

