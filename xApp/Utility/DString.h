#include "Common/Core.h"

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
