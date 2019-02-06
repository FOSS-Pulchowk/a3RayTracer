#include "string2.h"

namespace a3
{

	String::String(u64 len) :
		m_Length(len),
		m_Data(a3Malloc(len, char))
	{
	}

	String::String() :
		m_Length(1),
		m_Data(a3Malloc(sizeof(char), char))
	{
	}

	String::String(const char* cstr) :
		m_Length(a3::GetStringLength(cstr) + 1),
		m_Data(a3Malloc(m_Length, char))
	{
		a3::MemoryCopy(m_Data, cstr, m_Length);
	}

	String::String(const String& str) :
		m_Length(str.m_Length),
		m_Data(a3Malloc(m_Length, char))
	{
		a3::MemoryCopy(m_Data, str.m_Data, m_Length);
	}

	String::~String()
	{
		free(m_Data);
	}

	String& String::operator=(const char* cstr)
	{
		m_Length = a3::GetStringLength(cstr) + 1;
		m_Data = a3Realloc(m_Data, m_Length, char);
		a3::MemoryCopy(m_Data, cstr, m_Length);
		m_Length = a3::GetStringLength(cstr) + 1;
		return *this;
	}

	String& String::operator=(const String& str)
	{
		if (this == &str)
			return *this;
		m_Length = str.m_Length;
		m_Data = a3Realloc(m_Data, m_Length, char);
		a3::MemoryCopy(m_Data, str.m_Data, m_Length);
		m_Length = str.m_Length;
		return *this;
	}

	char& String::operator[](u64 i)
	{
		return m_Data[i];
	}

	const char& String::operator[](u64 i) const
	{
		return m_Data[i];
	}

	char* String::CharArray()
	{
		return m_Data;
	}

	const char* String::CharArray() const
	{
		return m_Data;
	}

	u64 String::Length() const
	{
		return m_Length;
	}

	String String::ToUpper(const String & str)
	{
		String ret(str);
		for (u64 i = 0; i < ret.m_Length; i++)
		{
			if (ret.m_Data[i] > 96 && ret.m_Data[i] < 123)
				ret.m_Data[i] -= 32;
		}
		return ret;
	}

	String String::ToLower(const String & str)
	{
		String ret(str);
		for (u64 i = 0; i < ret.m_Length; i++)
		{
			if (ret.m_Data[i] > 64 && ret.m_Data[i] < 91)
				ret.m_Data[i] += 32;
		}
		return ret;
	}

	bool operator==(const String& one, const String& other)
	{
		if (one.m_Length != other.m_Length) return false;
		for (u64 i = 0; i < one.m_Length; i++)
			if (one.m_Data[i] != other.m_Data[i])
				return false;
		return true;
	}

	bool operator!=(const String& one, const String& other)
	{
		return !(one == other);
	}

	String operator+(const String& lhs, const String& rhs)
	{
		String temp(lhs.m_Length + rhs.m_Length - 1);
		a3::MemoryCopy(temp.m_Data, lhs.m_Data, (lhs.m_Length) * sizeof(char));
		a3::MemoryCopy(&temp.m_Data[0], rhs.m_Data, rhs.m_Length * sizeof(char));
		return temp;
	}

	String& String::operator+=(const String& other)
	{
		m_Data = a3Realloc(m_Data, (m_Length + other.m_Length - 1), char);
		a3::MemoryCopy(&m_Data[m_Length - 1], other.m_Data, other.m_Length * sizeof(char));
		m_Length = m_Length + other.m_Length - 1;
		return *this;
	}

}