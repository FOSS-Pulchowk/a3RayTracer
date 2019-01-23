#pragma once
#include "Common/Core.h"

namespace a3 {

	// NOTE(Zero):
	// The implementation of this stack does not return error codes
	// Asserts are placed in case something happens that should not happen
	// This stack MUST not be used for loading dyamically by end user
	// This stack should only be used for engine management for internal use, where size of stack is known before hand
	struct Stack
	{
	private:
		u8* m_Memory;
		u8* m_Current;
		u32 m_Capacity;
		u32 m_Consumed;
	public:
		inline Stack(u8* memory, u32 size): m_Memory(memory), m_Capacity(size) 
		{
			m_Current = m_Memory;
			m_Consumed = 0;
		}

		template <typename Type>
		Type* Push()
		{
			a3Assert(m_Capacity > (m_Consumed + sizeof(Type)));
			Type* dest = (Type*)m_Current;
			*(u32*)((u8*)m_Current + sizeof(Type)) = sizeof(Type);
			m_Consumed += (sizeof(Type) + sizeof(u32));
			m_Current += (sizeof(Type) + sizeof(u32));
			return dest;
		}

		template <typename Type>
		Type* PushArray(u32 num)
		{
			a3Assert(m_Capacity > (m_Consumed + sizeof(Type) * num));
			Type* dest = (Type*)m_Current;
			*(u32*)((u8*)m_Current + sizeof(Type) * num) = sizeof(Type);
			m_Consumed += (sizeof(Type) * num + sizeof(u32));
			m_Current += (sizeof(Type) * num + sizeof(u32));
			return dest;
		}

		template <typename Type>
		Type Pop()
		{
			u32 size = *(u32*)((u8*)m_Current - sizeof(u32));
			Type result = *(Type*)((u8*)m_Current - sizeof(u32) - size);
			m_Current -= (sizeof(u32) + size);
			m_Consumed -= (sizeof(u32) + size);
			return result;
		}
	};

}