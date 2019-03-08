#pragma once
#include "Common/Core.h"

//
// DECLARATIONS
//

namespace a3 {
    
    inline i32 ParseI32(const utf8* buffer);
    inline u32 ParseU32(const utf8* buffer);
    inline f32 ParseF32(const utf8* buffer);
    inline f64 ParseF64(const utf8* buffer);
    
    inline u64 GetUtf8Length(const utf8* s);
    
    inline i32 StringPrint(utf8* buffer, u64 length, i32 value);
    inline i32 StringPrint(utf8* buffer, u64 length, u32 value);
    inline i32 StringPrint(utf8* buffer, u64 length, u64 value);
    inline i32 StringPrint(utf8* buffer, u64 length, f32 value);
    inline i32 StringPrint(utf8* buffer, u64 length, f64 value);
    inline i32 StringPrint(utf8* buffer, u64 length, const utf8* value);
    inline i32 StringPrint(utf8* buffer, u64 length, utf8 value);
    inline i32 StringPrint(utf8* buffer, u64 length, v2 v);
    inline i32 StringPrint(utf8* buffer, u64 length, v3 v);
    inline i32 StringPrint(utf8* buffer, u64 length, v4 v);
    inline i32 StringPrintFormatted(utf8* buffer, u64 length, const utf8* value);
    template <typename Type, typename ...Args>
        inline i32 StringPrintFormatted(utf8* buffer, u64 length, const utf8* format, Type value, Args... args);
    
    inline u32 Hash(const utf8* s);
    
}


//
// IMPLEMENTATION
//


#include <stdio.h>
#include <stdlib.h>

inline i32 a3::ParseI32(const utf8* buffer)
{
    return atoi(buffer);
}

inline u32 a3::ParseU32(const utf8* buffer)
{
    return atol(buffer);
}

inline f32 a3::ParseF32(const utf8* buffer)
{
    return (f32)atof(buffer);
}

inline f64 a3::ParseF64(const utf8* buffer)
{
    return atof(buffer);
}

inline u64 a3::GetUtf8Length(const utf8* s)
{
	u64 len = 0;
	for (utf8* c = (utf8*)s; *c != 0; ++c)
		len++;
	return (len + 1);
}


inline i32 a3::StringPrint(utf8* buffer, u64 length, i32 value)
{
    return snprintf(buffer, length, "%d", value);
}

inline i32 a3::StringPrint(utf8* buffer, u64 length, u32 value)
{
    return snprintf(buffer, length, "%u", value);
}

inline i32 a3::StringPrint(utf8* buffer, u64 length, u64 value)
{
    return snprintf(buffer, length, "%I64u", value);
}

inline i32 a3::StringPrint(utf8* buffer, u64 length, f32 value)
{
    return snprintf(buffer, length, "%f", value);
}

inline i32 a3::StringPrint(utf8* buffer, u64 length, f64 value)
{
    return snprintf(buffer, length, "%f", value);
}

inline i32 a3::StringPrint(utf8* buffer, u64 length, const utf8* value)
{
    return snprintf(buffer, length, "%s", value);
}

inline i32 a3::StringPrint(utf8* buffer, u64 length, utf8 value)
{
    return snprintf(buffer, length, "%c", value);
}

inline i32 a3::StringPrint(utf8* buffer, u64 length, v2 v)
{
    i32 written = snprintf(buffer, length, "(");
    written += snprintf(buffer, length, "%f", v.x);
    written += snprintf(buffer, length, ", ");
    written += snprintf(buffer, length, "%f", v.y);
    written += snprintf(buffer, length, ")");
    return written;
}

inline i32 a3::StringPrint(utf8* buffer, u64 length, v3 v)
{
    i32 written = snprintf(buffer, length, "(");
    written += snprintf(buffer, length, "%f", v.x);
    written += snprintf(buffer, length, ", ");
    written += snprintf(buffer, length, "%f", v.y);
    written += snprintf(buffer, length, ", ");
    written += snprintf(buffer, length, "%f", v.z);
    written += snprintf(buffer, length, ")");
    return written;
}

inline i32 a3::StringPrint(utf8* buffer, u64 length, v4 v)
{
    i32 written = snprintf(buffer, length, "(");
    written += snprintf(buffer, length, "%f", v.x);
    written += snprintf(buffer, length, ", ");
    written += snprintf(buffer, length, "%f", v.y);
    written += snprintf(buffer, length, ", ");
    written += snprintf(buffer, length, "%f", v.z);
    written += snprintf(buffer, length, ", ");
    written += snprintf(buffer, length, "%f", v.w);
    written += snprintf(buffer, length, ")");
    return written;
}

inline i32 a3::StringPrintFormatted(utf8* buffer, u64 length, const utf8* value)
{
    return snprintf(buffer, length, "%s", value);
}

template <typename Type, typename ...Args>
inline i32 a3::StringPrintFormatted(utf8* buffer, u64 length, const utf8* format, Type value, Args... args)
{
    utf8* traverser = (utf8*)format;
    i32 written = 0;
    for (; *traverser != 0; traverser++ ) 
    {
        if (*traverser == '%' ) 
        {
            written += StringPrint(&buffer[written], length, value);
            written += StringPrintFormatted(&buffer[written], length, 
                                            traverser + 1,
                                            args...);
            if(written == length)
                buffer[written-1] = 0;
            else
                buffer[written] = 0;
            return written;
        }
        
        buffer[written] = *traverser;
        written++;
        if(written == length)
        {
            buffer[written-1] = 0;
            return written;
        }
    }
    buffer[written] = 0;
    return written;
}

inline u32 a3::Hash(const utf8* s)
{
	u32 hash = 0;
	i32 c;
	while (c = *s++)
		hash = c + (hash << 6) + (hash << 16) - hash;
	return hash;
}
