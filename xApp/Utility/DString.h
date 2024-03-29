#include "Common/Core.h"

//
// DECLARATION
//

namespace a3
{
	// NOTE(Zero):
	// The dstring class is different from std::class
	// dstring does not copy or have assignment
	// dstring always moves and so the ownership of the class must be carefully maintained
	// this is done to make the code run faster

	struct dstring
	{
	private:
		u64 m_Length;
		utf8* m_Data;

	public:
		dstring();
		dstring(s8 cstr);
		dstring(dstring& str);
		~dstring();

		void New(u64 len);
		void Delete();
		void Move(dstring& l, dstring& r); // owndership is transfered DEFAULT!
		void Copy(dstring& l, dstring& r);

		dstring& operator=(s8 cstr);
		dstring& operator=(dstring& str);

		utf8& operator[](u64 i);
		const utf8& operator[](u64 i) const;

		utf8* Utf8Array();
		s8 Utf8Array() const;

		u64 Length() const;

		dstring& ToUpper(dstring& str);
		dstring& ToLower(dstring& str);

		friend bool operator==(dstring& one, dstring& other);
		friend bool operator!=(dstring& one, dstring& other);

		friend bool operator==(dstring& one, s8 other);
		friend bool operator!=(dstring& one, s8 other);
		
		friend bool operator==(s8 other, dstring& one);
		friend bool operator!=(s8 other, dstring& one);

		// lhs and rhs remain as they are, ownership is not transfered
		friend dstring operator+(dstring& lhs, dstring& rhs); 
		friend dstring operator+(dstring& lhs, s8 rhs);
		friend dstring operator+(s8 rhs, dstring& one);

		// other remains as it is, ownership is not transfered
		dstring& operator+=(dstring& other);
		dstring& operator+=(s8 other);
	};

}

//
// IMPLEMENTATION
//


#include "Platform/Platform.h"
#include "Memory.h"
#include "String.h"

#ifdef A3_IMPLEMENT_DSTRING

namespace a3
{

	dstring::dstring() :
		m_Length(0),
		m_Data(0)
	{
	}

	dstring::dstring(s8 cstr) :
		m_Length(a3::GetUtf8Length(cstr)),
		m_Data(a3Malloc(m_Length, utf8))
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
		m_Data = a3Realloc(m_Data, len, utf8);
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
		l.m_Data = a3Realloc(l.m_Data, l.m_Length, utf8);
		a3::MemoryCopy(l.m_Data, r.m_Data, r.m_Length);
	}

	dstring& dstring::operator=(s8 cstr)
	{
		m_Length = a3::GetUtf8Length(cstr) + 1;
		m_Data = a3Realloc(m_Data, m_Length, utf8);
		a3::MemoryCopy(m_Data, cstr, m_Length);
		m_Length = a3::GetUtf8Length(cstr) + 1;
		return *this;
	}

	dstring& dstring::operator=(dstring& str)
	{
		Move(*this, str);
		return *this;
	}

	utf8& dstring::operator[](u64 i)
	{
		return m_Data[i];
	}

	const utf8& dstring::operator[](u64 i) const
	{
		return m_Data[i];
	}

	utf8* dstring::Utf8Array()
	{
		return m_Data;
	}

	s8 dstring::Utf8Array() const
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

	bool operator==(dstring& one, dstring& other)
	{
		if (one.m_Length != other.m_Length) return false;
		for (u64 i = 0; i < one.m_Length; i++)
			if (one.m_Data[i] != other.m_Data[i])
				return false;
		return true;
	}

	bool operator!=(dstring& one, dstring& other)
	{
		return !(one == other);
	}

	bool operator==(dstring & one, s8 other)
	{
		u64 len = a3::GetUtf8Length(other);
		if (len != one.m_Length) return false;
		for (u64 i = 0; i < len; ++i)
		{
			if (other[i] != one.m_Data[i]) return false;
		}
		return true;
	}

	bool operator!=(dstring & one, s8 other)
	{
		return !(one == other);
	}

	bool operator==(s8 other, dstring & one)
	{
		return one == other;
	}

	bool operator!=(s8 other, dstring & one)
	{
		return one != other;
	}

	dstring operator+(dstring& lhs, dstring& rhs)
	{
		dstring temp;
		temp.New((lhs.m_Length + rhs.m_Length - 1));
		a3::MemoryCopy(temp.m_Data, lhs.m_Data, (lhs.m_Length) * sizeof(utf8));
		a3::MemoryCopy(&temp.m_Data[lhs.m_Length - 1], rhs.m_Data, rhs.m_Length * sizeof(utf8));
		return temp;
	}

	dstring operator+(dstring & lhs, s8 rhs)
	{
		dstring temp;
		u64 len = a3::GetUtf8Length(rhs);
		temp.New(lhs.m_Length + len - 1);
		a3::MemoryCopy(temp.m_Data, lhs.m_Data, lhs.m_Length * sizeof(utf8));
		a3::MemoryCopy(&temp.m_Data[lhs.m_Length - 1], rhs, len * sizeof(utf8));
		return temp;
	}

	dstring operator+(s8 rhs, dstring & lhs)
	{
		dstring temp;
		u64 len = a3::GetUtf8Length(rhs);
		temp.New(lhs.m_Length + len - 1);
		a3::MemoryCopy(&temp.m_Data, rhs, len * sizeof(utf8));
		a3::MemoryCopy(&temp.m_Data[len - 1], lhs.m_Data, lhs.m_Length * sizeof(utf8));
		return temp;
	}

	dstring& dstring::operator+=(dstring& other)
	{
		m_Data = a3Realloc(m_Data, (m_Length + other.m_Length - 1), utf8);
		a3::MemoryCopy(&m_Data[m_Length - 1], other.m_Data, other.m_Length * sizeof(utf8));
		m_Length = m_Length + other.m_Length - 1;
		return *this;
	}

	dstring & dstring::operator+=(s8 other)
	{
		u64 len = a3::GetUtf8Length(other);
		m_Data = a3Realloc(m_Data, (m_Length + len - 1), utf8);
		a3::MemoryCopy(&m_Data[m_Length - 1], other, len * sizeof(utf8));
		m_Length = m_Length + len - 1;
		return *this;
	}

}

#endif