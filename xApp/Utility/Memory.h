#pragma once
#include "Common/Core.h"

//
// DECLARATIONS
//

namespace a3 {

	// NOTE(Zero):
	// The implementation of this stack does not return error codes
	// Asserts are placed in case something happens that should not happen
	// User of this class is responsible for accessing or pushing invalid memory address and other invalid calls
	struct mstack
	{
	private:
		u8* m_Memory;
		u8* m_Current;
		u32 m_Capacity;
		u32 m_Consumed;
	public:
		inline mstack(u8* memory, u32 size);

		template <typename Type>
		inline Type* Push();

		template <typename Type>
		inline Type* PushArray(u32 num);

		template <typename Type>
		inline Type Pop();

		inline u32 QueryCapacity() const;
		inline u32 QueryConsumed() const;
		inline u32 QueryEmpty() const;
	};

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

	inline mstack::mstack(u8 * memory, u32 size) : 
		m_Memory(memory), m_Capacity(size), m_Current(memory), m_Consumed(0)
	{
	}

	inline u32 mstack::QueryCapacity() const
	{
		return m_Capacity;
	}

	inline u32 mstack::QueryConsumed() const
	{
		return m_Consumed;
	}

	inline u32 mstack::QueryEmpty() const
	{
		return m_Capacity - m_Consumed;
	}

	template<typename Type>
	inline Type * mstack::Push()
	{
		a3Assert(m_Capacity > (m_Consumed + sizeof(Type)));
		Type* dest = (Type*)m_Current;
		*(u32*)((u8*)m_Current + sizeof(Type)) = sizeof(Type);
		m_Consumed += (sizeof(Type) + sizeof(u32));
		m_Current += (sizeof(Type) + sizeof(u32));
		return dest;
	}

	template<typename Type>
	inline Type * mstack::PushArray(u32 num)

	{
		a3Assert(m_Capacity > (m_Consumed + sizeof(Type) * num));
		Type* dest = (Type*)m_Current;
		*(u32*)((u8*)m_Current + sizeof(Type) * num) = sizeof(Type);
		m_Consumed += (sizeof(Type) * num + sizeof(u32));
		m_Current += (sizeof(Type) * num + sizeof(u32));
		return dest;
	}

	template<typename Type>
	inline Type mstack::Pop()
	{
		u32 size = *(u32*)((u8*)m_Current - sizeof(u32));
		Type result = *(Type*)((u8*)m_Current - sizeof(u32) - size);
		m_Current -= (sizeof(u32) + size);
		m_Consumed -= (sizeof(u32) + size);
		return result;
	}


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
