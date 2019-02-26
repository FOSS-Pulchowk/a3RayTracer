#pragma once
#include "Common/Core.h"

//
// DECLARATIONS
//

namespace a3 {
    
	template <typename Type>
        void Swap(Type* a, Type* b);
    
    // NOTE(Zero):
    // Pseudo Random NUmber Generator uses Well RNG Algorithm
    // Link here: http://lomont.org/Math/Papers/2008/Lomont_PRNG_2008.pdf
    
    // NOTE(Zero): Array length must at least be 16
    void InitializeGenerator(u32* seeds);
    
    u32 RandomU32();
    u32 RandomU32(u32 min, u32 max);
    i32 RandomI32(i32 min, i32 max);
    f32 RandomF32();
    f64 RandomF64();
    f32 RandomF32(f32 min, f32 max);
    f64 RandomF64(f32 min, f32 max);
    v2 RandomV2();
    v3 RandomV3();
    v4 RandomV4();
    
    template <typename Type>
        struct random_generator
    {
        const Type Min;
        const Type Max;
        random_generator(const Type& min, const Type& max);
        Type Get();
    };
    
}

//
// IMPLEMENTATION
//

namespace a3 {

	template <typename Type>
	void Swap(Type * a, Type * b)
	{
		Type temp = *a;
		*a = *b;
		*b = temp;
	}

	template <typename Type>
	random_generator<Type>::random_generator(const Type& min, const Type& max) :
		Min(min), Max(max)
	{
	}

	template <typename Type>
	Type random_generator<Type>::Get()
	{
		return Min + (Type)(RandomF64() * (f64)(Max - Min));
	}

}
    
#ifdef A3_IMPLEMENT_ALGORITHM
    
    namespace a3 {
        
        // NOTE(Zero): init should also reset this to 0
        static u32 s_WellRNGIndex = 0;
        static u32 s_WellRNGState[16];
        
        static u32 WellRNG512()
        {
            u32 a, b, c, d;
            a = s_WellRNGState[s_WellRNGIndex];
            c = s_WellRNGState[(s_WellRNGIndex+13)&15];
            b = a^c^(a<<16)^(c<<15);
            c = s_WellRNGState[(s_WellRNGIndex+9)&15];
            c ^= (c>>11);
            a = s_WellRNGState[s_WellRNGIndex] = b^c;
            d = a^((a<<5)&0xDA442D24UL);
            s_WellRNGIndex = (s_WellRNGIndex + 15)&15;
            a = s_WellRNGState[s_WellRNGIndex];
            s_WellRNGState[s_WellRNGIndex] = a^b^d^(a<<2)^(b<<18)^(c<<28);
            return s_WellRNGState[s_WellRNGIndex];
        }
        
        void InitializeGenerator(u32* seeds)
        {
            s_WellRNGIndex = 0;
            for(i32 i=0; i<16; ++i)
                s_WellRNGState[i] = seeds[i];
        }
        
        u32 RandomU32()
        {
            return WellRNG512();
        }
        
        u32 RandomU32(u32 min, u32 max)
        {
            return min +  (u32)(RandomF32() * (f32)(max - min));
        }
        
        i32 RandomI32(i32 min, i32 max)
        {
            return min +  (i32)(RandomF32() * (f32)(max - min));
        }
        
        f32 RandomF32()
        {
            return (f32)WellRNG512() / (f32)max_u32;
        }
        
        f64 RandomF64()
        {
            return (f64)WellRNG512() / (f64)max_u32;
        }
        
        f32 RandomF32(f32 min, f32 max)
        {
            return min + RandomF32() * (max - min);
        }
        
        f64 RandomF64(f32 min, f32 max)
        {
            return min + RandomF64() * (max - min);
        }
        
        v2 RandomV2()
        {
            v2 result;
            result.x = RandomF32();
            result.y = RandomF32();
            return result;
        }
        
        v3 RandomV3()
        {
            v3 result;
            result.x = RandomF32();
            result.y = RandomF32();
            result.z = RandomF32();
            return result;
        }
        
        v4 RandomV4()
        {
            v4 result;
            result.x = RandomF32();
            result.y = RandomF32();
            result.z = RandomF32();
            result.w = RandomF32();
            return result;
        }
    }
    
#endif