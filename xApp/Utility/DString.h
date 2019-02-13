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
		char* m_Data;

	public:
		dstring();
		dstring(const char* cstr);
		dstring(dstring& str);
		~dstring();

		void New(u64 len);
		void Delete();
		void Move(dstring& l, dstring& r); // owndership is transfered DEFAULT!
		void Copy(dstring& l, dstring& r);

		dstring& operator=(const char* cstr);
		dstring& operator=(dstring& str);

		char& operator[](u64 i);
		const char& operator[](u64 i) const;

		char* CharArray();
		const char* CharArray() const;

		u64 Length() const;

		dstring& ToUpper(dstring& str);
		dstring& ToLower(dstring& str);

		friend bool operator==(const dstring& one, const dstring& other);
		friend bool operator!=(const dstring& one, const dstring& other);

		// lhs and rhs remain as they are, ownership is not transfered
		friend dstring operator+(const dstring& lhs, const dstring& rhs); 

		// other remains as it is, ownership is not transfered
		dstring& operator+=(const dstring& other);
	};

}
