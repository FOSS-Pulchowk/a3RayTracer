#pragma once
#include "Common/Core.h"

//
// DECLARATIONS
//

namespace a3 {
    
	// Helper memory management functions
	inline void MemoryCopy(void* dst, const void* src, u64 size);
	inline void ReverseMemoryCopy(void* dst, const void* src, u64 size);
	inline void ReverseRectCopy(void* dst, const void* src, i32 w, i32 h);
	inline void* MemoryMove(void* dst, const void* src, u64 size);
	inline void* MemorySet(void* dst, u8 val, u64 size);
    
}

//
// IMPLEMENTATION
//

namespace a3 {
    
	inline void MemoryCopy(void* dst, const void* src, u64 size)
	{
		u8* dp = (u8*)dst;
		u8* sp = (u8*)src;
		for (u64 i = 0; i < size; ++i)
			*dp++ = *sp++;
	}
    
	inline void ReverseMemoryCopy(void* dst, const void* src, u64 size)
	{
		u8* dp = (u8*)dst;
		u8* sp = (u8*)src + size - 1;
		for (u64 i = 0; i < size; ++i)
			*dp++ = *sp--;
	}
    
	inline void ReverseRectCopy(void* dst, const void* src, i32 w, i32 h)
	{
		u8* sp = (u8*)src;
		u8* rowDP = (u8*)dst + w * h - w;
		for (i32 y = 0; y < h; ++y)
		{
			u8* dp = rowDP;
			for (i32 x = 0; x < w; ++x)
			{
				*dp++ = *sp++;
			}
			rowDP -= w;
		}
	}
    
	inline void* MemoryMove(void* dst, const void* src, u64 size)
	{
		u8* pd = (u8*)dst;
		u8* sp = (u8*)src;
		for (u64 i = 0; i < size; ++i)
			*pd++ = *sp++;
		return dst;
	}
    
	inline void* MemorySet(void* dst, u8 val, u64 size)
	{
		u8* p = (u8*)dst;
		for (u64 i = 0; i < size; ++i)
			*p++ = val;
		return dst;
	}
    
    
}
