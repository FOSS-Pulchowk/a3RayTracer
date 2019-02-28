#pragma once
#include "Common/Core.h"
#include "Platform/Platform.h"

namespace a3 {

	// TODO(Zero):
	// If we are not able to access more memory or reallocating fails, then there is very high probability for the program to crash
	// Handling when we are in low memory

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
		inline ~darray();
		inline b32 Resize(u64 capacity);
		inline Type& GetElement(u64 index);
		inline Type& operator[](u64 index);
		inline Type* GetPointer(u64 index);
		inline Type* GetData();
		inline Type& Push(const Type& e);
		inline Type& PushFront(const Type& e);

		template <typename ...Args>
		inline Type& Emplace(Args&&... args);

		template <typename ...Args>
		inline Type& EmplaceFront(Args&&... args);

		inline Type& Pop();
		inline void PopFront();
		inline u64 QuerySize() const;
		inline u64 QueryCapacity() const;
		inline u64 QueryEmpty() const;
		inline Type* Begin() const;
		inline Type* End() const;
		inline void New(u64 capacity);
		inline void Delete();
		inline void Empty();
	};

	template<typename Type>
	inline darray<Type>::darray()
	{
		m_Base = 0;
		m_Capacity = 0;
		m_Size = 0;
	}

	template<typename Type>
	inline darray<Type>::darray(u64 capacity) : darray()
	{
		New(capacity);
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
	inline darray<Type>::~darray()
	{
		a3Free(m_Base);
	}

	template<typename Type>
	inline b32 darray<Type>::Resize(u64 capacity)
	{
		Type * temp;
		temp = a3Realloc(m_Base, capacity * sizeof(Type), Type);
		if (temp)
		{
			m_Base = temp;
			m_Capacity = capacity;
			return true;
		}
		return false;
	}

	template<typename Type>
	inline Type * darray<Type>::GetData()
	{
		return m_Base;
	}

	template<typename Type>
	inline Type & darray<Type>::GetElement(u64 index)
	{
		a3Assert(index < m_Size);
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
	inline Type & darray<Type>::Push(const Type & e)
	{
		if (m_Size == m_Capacity) {
			if (!Resize(m_Capacity + 10))
			{
				a3TriggerBreakPoint();
			}
		}
		m_Base[m_Size++] = e;
		return m_Base[m_Size - 1];
	}

	template<typename Type>
	inline Type & darray<Type>::PushFront(const Type & e)
	{
		if (m_Size == m_Capacity) 
		{
			if (!Resize(m_Capacity + 10))
			{
				a3TriggerBreakPoint();
			}
		}
		for (i32 i = m_Size - 1; i >= 0; --i)
			m_Base[i + 1] = m_Base[i];
		m_Base[0] = e;
		return m_Base[0];
	}

	template<typename Type>
	template<typename ...Args>
	inline Type & darray<Type>::Emplace(Args&& ...args)
	{
		if (m_Size == m_Capacity) {
			if (!Resize(m_Capacity + 10))
			{
				a3TriggerBreakPoint();
			}
		}
		a3Place(&m_Base[m_Size++]) Type(args...);
		return m_Base[m_Size - 1];
	}

	template<typename Type>
	template<typename ...Args>
	inline Type & darray<Type>::EmplaceFront(Args && ...args)
	{
		if (m_Size == m_Capacity) 
		{
			if (!Resize(m_Capacity + 10))
			{
				a3TriggerBreakPoint();
			}
		}
		for (i32 i = m_Size - 1; i >= 0; --i)
			m_Base[i + 1] = m_Base[i];
		a3Place(&m_Base[0]) Type(args...);
		return m_Base[0];
	}

	template<typename Type>
	inline Type & darray<Type>::Pop()
	{
		return m_Base[m_Size--];
	}

	template<typename Type>
	inline void darray<Type>::PopFront()
	{
		for (i32 i = 0; i < m_Size - 1; ++i)
			m_Base[i] = m_Base[i + 1];
		m_Size--;
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
		return m_Base;
	}

	template<typename Type>
	inline Type * darray<Type>::End() const
	{
		return &GetElement(m_Size - 1);
	}

	template<typename Type>
	inline void darray<Type>::New(u64 capacity)
	{
		m_Capacity = capacity;
		if (m_Capacity < m_Size) m_Size = m_Capacity;
		m_Base = a3Realloc(m_Base, m_Capacity, Type);
	}

	template<typename Type>
	inline void darray<Type>::Delete()
	{
		m_Capacity = 0;
		m_Size = 0;
		a3Free(m_Base);
		m_Base = A3NULL;
	}

	template<typename Type>
	inline void darray<Type>::Empty()
	{
		m_Size = 0;
	}

}


#define a3ForEach(e, a) for(auto& e = *a.Begin(); &e != (a.End()+1); ++&e++)
