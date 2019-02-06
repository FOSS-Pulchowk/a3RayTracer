#pragma once
#include "Common/Core.h"
#include "Platform/Platform.h"

namespace a3 {

	template<typename Type>
	class dynamic_array
	{
	private:
		Type* m_Base;
		u64 m_Size;
		u64 m_Capacity;

	public:
		dynamic_array();
		dynamic_array(Type* s);
		dynamic_array(const Type s);
		dynamic_array(u64 capacity);
		b32 Resize(u64 capacity);
		Type& GetElement(u64 index) const;
		Type& operator[](u64 index) const;
		Type* GetPointer(u64 index) const;
		Type* GetData() const;
		//const Type& GetElement(u64 index) const;
		//const Type& operator[](u64 index) const;
		//const Type* GetPointer(u64 index) const;
		//const Type* GetData() const;
		Type& Push(const Type& e);
		Type& Pop();
		u64 GetSize() const;
		u64 GetCapacity() const;
	};

	template<typename Type>
	inline dynamic_array<Type>::dynamic_array()
	{
		m_Base = 0;
		m_Capacity = 0;
		m_Size = 0;
	}

	template<typename Type>
	inline dynamic_array<Type>::dynamic_array(Type * s)
	{
	}

	template<typename Type>
	inline dynamic_array<Type>::dynamic_array(const Type s)
	{
	}

	template<typename Type>
	inline dynamic_array<Type>::dynamic_array(u64 capacity)
	{
		m_Base = a3Malloc(capacity, Type);
		if (m_Base)
		{
			m_Capacity = capacity;
		}
	}

	template<typename Type>
	inline b32 dynamic_array<Type>::Resize(u64 capacity)
	{
		Type * temp;
		temp = a3Realloc(m_Base, capacity, Type);
		if (temp) 
		{
			m_Base = temp;
			m_Capacity = capacity;
			return true;
		}
		return false;
	}

	template<typename Type>
	inline Type & dynamic_array<Type>::GetElement(u64 index) const
	{
		return m_Base[index];
	}

	template<typename Type>
	inline Type & dynamic_array<Type>::operator[](u64 index) const
	{
		return GetElement(index);
	}

	template<typename Type>
	inline Type * dynamic_array<Type>::GetPointer(u64 index) const
	{
		return &GetElement(index);
	}

	template<typename Type>
	inline Type * dynamic_array<Type>::GetData() const
	{
		return m_Base;
	}

	template<typename Type>
	inline Type & dynamic_array<Type>::Push(const Type & e)
	{
		if (m_Size == m_Capacity) {
			if (!Resize(m_Capacity + 10)) 
			{
				a3TriggerBreakPoint();
			}
		}
		m_Base[m_Size++] = e;
		return m_Base[m_Size-1];
	}

	template<typename Type>
	inline Type & dynamic_array<Type>::Pop()
	{
		return m_Base[m_Size--];
	}

	template<typename Type>
	inline u64 dynamic_array<Type>::GetSize() const
	{
		return m_Size;
	}

	template<typename Type>
	inline u64 dynamic_array<Type>::GetCapacity() const
	{
		return m_Capacity;
	}


}