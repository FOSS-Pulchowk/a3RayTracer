#pragma once
#include "Common/Core.h"

//
// DECLARATIONS
//

namespace a3 {

	inline i32 WriteU32ToBuffer(utf8* buffer, u32 length, u32 number, u32 base);
	inline i32 WriteF32ToBuffer(utf8* buffer, u32 length, f32 number);

	inline u32 ParseU32(s8 buffer, utf8 end = 0);
	inline f32 ParseF32(s8 buffer, utf8 end = 0);

	inline u64 GetStringLength(s8 s);

}


//
// IMPLEMENTATION
//

inline i32 a3::WriteU32ToBuffer(utf8* buffer, u32 length, u32 number, u32 base)
{
	a3Assert(length > 0);
	a3Assert(base == 2 || base == 8 || base == 16 || base == 10);
	if (number == 0)
	{
		buffer[0] = '0';
		buffer[1] = 0;
		return 1;
	}

	i32 bufferIndex = 0;
	while (number != 0)
	{
		u64 rem = number % base;
		buffer[bufferIndex++] = (utf8)((rem > 9) ? (rem - 10) + 'a' : rem + '0');
		if (bufferIndex == length) return -bufferIndex;
		number = number / base;
	}
	if (bufferIndex == length) return -bufferIndex;
	i32 start = 0;
	i32 end = bufferIndex - 1;
	while (start < end)
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

inline i32 a3::WriteF32ToBuffer(utf8* buffer, u32 length, f32 number)
{
	a3Assert(length > 0);
	u32 num = *((u32*)(&number));

	u32 sign = num >> 31;
	u32 exp = ((num >> 23) & 0xff) - 127;
	u32 man = num & ((1 << 23) - 1);
	man |= 1 << 23;

	i32 bufferIndex = 0;
	if (sign)
		buffer[bufferIndex++] = '-';

	if (bufferIndex == length) return -bufferIndex;
	i32 result = WriteU32ToBuffer(buffer + bufferIndex, length - bufferIndex, man >> (23 - exp), 10);
	if (result <= 0) return result;
	bufferIndex += result;

	buffer[bufferIndex++] = '.';
	if (bufferIndex == length) return -bufferIndex;

	u32 frac = man & ((1 << (23 - exp)) - 1);
	u32 base = 1 << (23 - exp);
	i32 c = 0;

	if (!frac)
	{
		buffer[bufferIndex++] = '0';
		if (bufferIndex == length) return -bufferIndex;
	}

	while (frac != 0 && c++ < 6)
	{
		frac *= 10;
		result = WriteU32ToBuffer(buffer + bufferIndex, length - bufferIndex, (u32)(frac / base), 10);
		if (result <= 0) return result;
		bufferIndex += result;
		frac %= base;
	}
	if (bufferIndex == length) return -bufferIndex;
	buffer[bufferIndex] = 0;

	return bufferIndex;
}

u32 a3::ParseU32(s8 buffer, utf8 end)
{
	u32 result = 0;
	for (utf8* s = (utf8*)buffer; *s != end; ++s)
	{
		result = result * 10 + (*s - 48);
	}
	return result;
}

f32 a3::ParseF32(s8 buffer, utf8 end)
{
	b32 period = false;
	f32 negative = 1.0f;
	f32 result = 0.0f;
	f32 divisor = 1.0f;
	f32 dividend = 0.0f;
	for (utf8* s = (utf8*)buffer; *s != end && *s != 'f' && *s != 'F'; ++s)
	{
		if (*s == '.')
		{
			period = true;
			continue;
		}
		if (*s == '-')
		{
			negative = -1.0f;
			continue;
		}
		if (period)
		{
			dividend = dividend * 10.0f + (f32)(*s - 48);
			divisor *= 10.0f;
		}
		else
			result = result * 10.0f + (f32)(*s - 48);
	}
	return negative * (result + dividend / divisor);
}

inline u64 a3::GetStringLength(s8 s)
{
	u64 len = 0;
	for (utf8* c = (utf8*)s; *c != 0; ++c)
		len++;
	return (len + 1);
}
