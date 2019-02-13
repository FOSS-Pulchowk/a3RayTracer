#pragma once
#include "Common/Core.h"
#include "Platform/Platform.h"

namespace a3 {

	template<typename Type>
	class darray
	{
	private:
		Type* m_Base;
		u64 m_Size;
		u64 m_Capacity;

	public:
		inline darray();
		inline darray(u64 capacity);
		inline darray(darray<Type>& da); // NOTE(Zero): Transfers ownership i.e. moves doesn't copy
		inline darray<Type>& operator=(darray<Type>& da); // NOTE(Zero): Transfers ownership i.e. moves doesn't copy
		inline b32 Resize(u64 capacity);
		inline Type& GetElement(u64 index);
		inline Type& operator[](u64 index);
		inline Type* GetPointer(u64 index);
		inline Type* GetData() const;
		inline const Type& GetElement(u64 index) const;
		inline const Type& operator[](u64 index) const;
		inline const Type* GetPointer(u64 index) const;
		inline const Type* GetData() const;
		inline Type& Push(const Type& e);
		inline Type& Pop();
		inline u64 QuerySize() const;
		inline u64 QueryCapacity() const;
		inline u64 QueryEmpty() const;
		inline Type* Begin() const;
		inline Type* End() const;
	};

	template<typename Type>
	inline darray<Type>::darray()
	{
		m_Base = 0;
		m_Capacity = 0;
		m_Size = 0;
	}

	template<typename Type>
	inline darray<Type>::darray(u64 capacity)
	{
		m_Base = a3Malloc(capacity, Type);
		if (m_Base)
		{
			m_Capacity = capacity;
		}
	}

	template<typename Type>
	inline darray<Type>::darray(darray & da)
	{
		m_Base = da.m_Base;
		m_Capacity = da.m_Capacity;
		m_Size = da.m_Size;
		da.m_Size = da.m_Capacity = 0;
		da.m_Base = A3NULL;
	}

	template<typename Type>
	inline darray<Type>& darray<Type>::operator=(darray & da)
	{
		if (this == &da) return *this;
		m_Base = da.m_Base;
		m_Capacity = da.m_Capacity;
		m_Size = da.m_Size;
		da.m_Size = da.m_Capacity = 0;
		da.m_Base = A3NULL;
		return *this;
	}

	template<typename Type>
	inline b32 darray<Type>::Resize(u64 capacity)
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
	inline Type & darray<Type>::GetElement(u64 index)
	{
		return m_Base[index];
	}

	template<typename Type>
	inline Type & darray<Type>::operator[](u64 index)
	{
		return GetElement(index);
	}

	template<typename Type>
	inline Type * darray<Type>::GetPointer(u64 index)
	{
		return &GetElement(index);
	}

	template<typename Type>
	inline Type * darray<Type>::GetData() const
	{
		return m_Base;
	}

	template<typename Type>
	inline const Type & darray<Type>::GetElement(u64 index) const
	{
		return m_Base[index];
	}

	template<typename Type>
	inline const Type & darray<Type>::operator[](u64 index) const
	{
		return GetElement(index);
	}

	template<typename Type>
	inline const Type * darray<Type>::GetPointer(u64 index) const
	{
		return &GetElement(index);
	}

	template<typename Type>
	inline Type & darray<Type>::Push(const Type & e)
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
	inline Type & darray<Type>::Pop()
	{
		return m_Base[m_Size--];
	}

	template<typename Type>
	inline u64 darray<Type>::QuerySize() const
	{
		return m_Size;
	}

	template<typename Type>
	inline u64 darray<Type>::QueryCapacity() const
	{
		return m_Capacity;
	}

	template<typename Type>
	inline u64 darray<Type>::QueryEmpty() const
	{
		return m_Capacity - m_Size;
	}

	template<typename Type>
	inline Type * darray<Type>::Begin() const
	{
		return &GetElement(0);
	}

	template<typename Type>
	inline Type * darray<Type>::End() const
	{
		return &GetElement(m_Size - 1);
	}


}