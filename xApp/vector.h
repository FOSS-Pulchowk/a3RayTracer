#include "Common/Core.h"
template<typedef T>
class dynamic_array
{
private:
	T* m_Base;
	u64 m_Size;
	u64 m_Capacity;

public:
	dynamic_array()
	{
		n_Base = new T[0];
	};
	dynamic_array(u64 capacity)
	{
		m_Base = new T[capacity];
	};
	b32 Resize(u64 capacity) {
		
	};
	T& GetElement(u64 index) const;
	T& operator[](u64 index) const;
	T* GetPointer(u64 index) const;
	T* GetData() const;
	const T& GetElement(u64 index) const;
	const T& operator[](u64 index) const;
	const T* GetPointer(u64 index) const;
	const T* GetData() const;
	T& Push(const T& e);
	T& Pop();
	u64 GetSize() const;
	u64 GetCapacity() const;
};