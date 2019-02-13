#include "Platform/Platform.h"
#include "DString.h"
#include "Memory.h"
#include "String.h"

namespace a3
{

	dstring::dstring() :
		m_Length(0),
		m_Data(0)
	{
	}

	dstring::dstring(const char* cstr) :
		m_Length(a3::GetStringLength(cstr) + 1),
		m_Data(a3Malloc(m_Length, char))
	{
		a3::MemoryCopy(m_Data, cstr, m_Length);
	}

	dstring::dstring(dstring& str)
	{
		Move(*this, str);
	}

	dstring::~dstring()
	{
		a3Free(m_Data);
	}

	void dstring::New(u64 len)
	{
		m_Length = len;
		m_Data = a3Realloc(m_Data, len, char);
	}

	void dstring::Delete()
	{
		m_Length = 0;
		a3Free(m_Data);
		m_Data = A3NULL;
	}

	void dstring::Move(dstring & l, dstring & r)
	{
		l.m_Length = r.m_Length;
		l.m_Data = r.m_Data;
		r.m_Length = 0;
		r.m_Data = A3NULL;
	}

	void dstring::Copy(dstring & l, dstring & r)
	{
		l.m_Length = r.m_Length;
		l.m_Data = a3Realloc(l.m_Data, l.m_Length, char);
		a3::MemoryCopy(l.m_Data, r.m_Data, r.m_Length);
	}

	dstring& dstring::operator=(const char* cstr)
	{
		m_Length = a3::GetStringLength(cstr) + 1;
		m_Data = a3Realloc(m_Data, m_Length, char);
		a3::MemoryCopy(m_Data, cstr, m_Length);
		m_Length = a3::GetStringLength(cstr) + 1;
		return *this;
	}

	dstring& dstring::operator=(dstring& str)
	{
		Move(*this, str);
		return *this;
	}

	char& dstring::operator[](u64 i)
	{
		return m_Data[i];
	}

	const char& dstring::operator[](u64 i) const
	{
		return m_Data[i];
	}

	char* dstring::CharArray()
	{
		return m_Data;
	}

	const char* dstring::CharArray() const
	{
		return m_Data;
	}

	u64 dstring::Length() const
	{
		return m_Length;
	}

	dstring& dstring::ToUpper(dstring & str)
	{
		for (u64 i = 0; i < str.m_Length; i++)
		{
			if (str.m_Data[i] > 96 && str.m_Data[i] < 123)
				str.m_Data[i] -= 32;
		}
		return str;
	}

	dstring& dstring::ToLower(dstring & str)
	{
		for (u64 i = 0; i < str.m_Length; i++)
		{
			if (str.m_Data[i] > 64 && str.m_Data[i] < 91)
				str.m_Data[i] += 32;
		}
		return str;
	}

	bool operator==(const dstring& one, const dstring& other)
	{
		if (one.m_Length != other.m_Length) return false;
		for (u64 i = 0; i < one.m_Length; i++)
			if (one.m_Data[i] != other.m_Data[i])
				return false;
		return true;
	}

	bool operator!=(const dstring& one, const dstring& other)
	{
		return !(one == other);
	}

	dstring operator+(const dstring& lhs, const dstring& rhs)
	{
		dstring temp;
		temp.New((lhs.m_Length + rhs.m_Length - 1));
		a3::MemoryCopy(temp.m_Data, lhs.m_Data, (lhs.m_Length) * sizeof(char));
		a3::MemoryCopy(&temp.m_Data[0], rhs.m_Data, rhs.m_Length * sizeof(char));
		return temp;
	}

	dstring& dstring::operator+=(const dstring& other)
	{
		m_Data = a3Realloc(m_Data, (m_Length + other.m_Length - 1), char);
		a3::MemoryCopy(&m_Data[m_Length - 1], other.m_Data, other.m_Length * sizeof(char));
		m_Length = m_Length + other.m_Length - 1;
		return *this;
	}

}