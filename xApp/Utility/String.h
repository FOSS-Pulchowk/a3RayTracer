#pragma once
#include "Common/Core.h"

namespace a3 {

inline i32 ParseU32(utf8* buffer, u32 length, u32 number, u32 base)
{
	a3Assert(length > 0);
	a3Assert(base == 2 || base == 8 || base == 16 || base == 10);
	if(number == 0)
	{
		buffer[0] = '0';
		buffer[1] = 0;
		return 1;
	}

	i32 bufferIndex = 0;
	while(number != 0)
	{
		u64 rem = number % base;
		buffer[bufferIndex++] = (utf8)((rem > 9) ? (rem - 10) + 'a' : rem + '0');
		if(bufferIndex == length) return -bufferIndex;
		number = number / base;
	}
	if(bufferIndex == length) return -bufferIndex;
	i32 start = 0;
	i32 end = bufferIndex - 1;
	while(start < end)
	{
		utf8 temp = buffer[start];
		buffer[start] = buffer[end];
		buffer[end] = temp;
		start++;
		end--;
	}
	buffer[bufferIndex] = 0;
	return bufferIndex;
}

inline i32 ParseF32(utf8* buffer, u32 length, f32 number)
{
	a3Assert(length > 0);
	u32 num = *((u32*)(&number));

	u32 sign = num >> 31;
	u32 exp = ((num >> 23) & 0xff) - 127;
	u32 man = num & ((1 << 23) - 1);
	man |= 1 << 23;

	i32 bufferIndex = 0;
	if(sign)
		buffer[bufferIndex++] = '-';
	
	if(bufferIndex == length) return -bufferIndex;
	i32 result = ParseU32(buffer + bufferIndex, length - bufferIndex, man >> (23 - exp), 10);
	if(result <= 0) return result;
	bufferIndex += result;

	buffer[bufferIndex++] = '.';
	if(bufferIndex == length) return -bufferIndex;

	u32 frac = man & ((1 << (23 - exp)) - 1);
	u32 base = 1 << (23 - exp);
	i32 c = 0;

	if(!frac)
	{
		buffer[bufferIndex++] = '0';
		if(bufferIndex == length) return -bufferIndex;
	}

	while(frac != 0 && c++ < 6)
	{
		frac *= 10;
		result = ParseU32(buffer + bufferIndex, length - bufferIndex, (u32)(frac / base), 10);
		if(result <= 0) return result;
		bufferIndex += result;
		frac %= base;
	}
	if(bufferIndex == length) return -bufferIndex;
	buffer[bufferIndex] = 0;

	return bufferIndex;
}

inline u64 GetStringLength(s8 s)
{
	u64 len = 0;
	for (utf8 c = s[0]; c != '\0'; ++c)
		len++;
	return (len + 1);
}

inline void MemoryCopy(void* dst, const void* src, u64 size)
{
	u8* pd = (u8*)dst;
	u8* sp = (u8*)src;
	for (u64 i = 0; i < size; ++i)
		*pd++ = *sp++;
}

inline void* MemoryMove(void* dst, const void* src, u64 size)
{
	u8* pd = (u8*)dst;
	u8* sp = (u8*)src;
	for (u64 i = 0; i < size; ++i)
		*pd++ = *sp++;
	return dst;
}

inline void* MemorySet(void* dst, i32 val, u64 size)
{
	u8* p = (u8*)dst;
	for (u64 i = 0; i < size; ++i)
		*p++ = val;
	return dst;
}

}