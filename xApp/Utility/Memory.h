#pragma once
#include "Common/Core.h"

//
// DECLARATIONS
//

namespace a3 {

	template <typename Type>
	struct queue
	{
	private:
		Type* m_Base;
		Type* m_Current;
		u32 m_Counts;
		u32 m_FillCounts;

	public:
		queue(void* memory, u64 size);

		u32 QueryTotalCounts();
		u32 QueryFillCounts();
		u32 QueryEmptyCounts();

		// NOTE(Zero):
		// When the memory is already full and Push/Emplace is called then the function will write out of memory
		// For debug mode assertion is added
		// If is doubt call QueryEmptyCounts to get available number of push
		Type* Push();
		Type* Push(const Type& e);
		template <typename ...Args>
		Type* Emplace(Args&&... args);

		Type* GetBack();
		Type* GetFront();
		void Pop();
		b32 Scan(b32(*fn)(Type* e, void* usrData), void* usrData); // Returns true if whole of queue gets accessed

		void Empty();
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

	template <typename Type>
	queue<Type>::queue(void* memory, u64 size)
	{
		a3Assert(size % sizeof(Type) == 0);
		m_Base = (Type*)memory;
		m_Current = (Type*)memory;
		m_Counts = (u32)(size / sizeof(Type));
		m_FillCounts = 0;
	}

	template <typename Type>
	u32 queue<Type>::QueryTotalCounts()
	{
		return m_Counts;
	}

	template <typename Type>
	u32 queue<Type>::QueryFillCounts()
	{
		return m_FillCounts;
	}

	template <typename Type>
	u32 queue<Type>::QueryEmptyCounts()
	{
		return m_Counts - m_FillCounts;
	}

	template <typename Type>
	Type* queue<Type>::Push()
	{
		a3Assert(m_FillCounts < m_Counts);
		m_FillCounts++;
		return m_Current++;
	}

	template <typename Type>
	Type* queue<Type>::Push(const Type& e)
	{
		a3Assert(m_FillCounts < m_Counts);
		m_FillCounts++;
		Type* dest = m_Current++;
		*dest = e;
		return dest;
	}

	template <typename Type>
	template <typename ...Args>
	Type* queue<Type>::Emplace(Args&&... args)
	{
		a3Assert(m_FillCounts < m_Counts);
		m_FillCounts++;
		Type* dest = m_Current++;
		a3Place(dest) Type(args...);
		return dest;
	}

	template <typename Type>
	Type* queue<Type>::GetBack()
	{
		return m_Current - 1;
	}

	template<typename Type>
	inline Type * queue<Type>::GetFront()
	{
		return m_Base;
	}

	template <typename Type>
	void queue<Type>::Pop()
	{
		m_Current->~Type();
		m_Current--;
		m_FillCounts--;
	}

	template <typename Type>
	b32 queue<Type>::Scan(b32(*fn)(Type* e, void* usrData), void* usrData)
	{
		for (i32 i = 0; i < m_FillCounts; ++i)
		{
			if (!fn(m_Base + i, usrData)) return false;
		}
		return true;
	}

	template <typename Type>
	void queue<Type>::Empty()
	{
		m_Current = m_Base;
		for (u32 i = 0; i < m_FillCounts; ++i)
			(m_Base + i)->~Type();
		m_FillCounts = 0;
	}

	inline void MemoryCopy(void* dst, const void* src, u64 size)
	{
		u8* dp = (u8*)dst;
		u8* sp = (u8*)src;
		for (u64 i = 0; i < size; ++i)
		{
			*dp = *sp;
			dp++; sp++;
		}
	}

	inline void ReverseMemoryCopy(void* dst, const void* src, u64 size)
	{
		u8* dp = (u8*)dst;
		u8* sp = (u8*)src + size - 1;
		for (u64 i = 0; i < size; ++i)
		{
			*dp = *sp;
			dp++; sp--;
		}
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
				*dp = *sp;
				dp++; sp++;
			}
			rowDP -= w;
		}
	}

	inline void* MemoryMove(void* dst, const void* src, u64 size)
	{
		u8* pd = (u8*)dst;
		u8* sp = (u8*)src;
		for (u64 i = 0; i < size; ++i)
		{
			*pd = *sp;
			pd++; sp++;
		}
		return dst;
	}

	inline void* MemorySet(void* dst, u8 val, u64 size)
	{
		u8* p = (u8*)dst;
		for (u64 i = 0; i < size; ++i)
		{
			*p = val;
			p++;
		}
		return dst;
	}


}
