#include "Common/Core.h"
#include "Platform/Platform.h"
#include "String.h"

namespace a3
{
	struct String
	{
	private:
		String(u64 len);

	public:
		String();
		String(const char* cstr);
		String(const String& str);
		~String();

		String& operator=(const char* cstr);
		String& operator=(const String& str);

		char& operator[](u64 i);
		const char& operator[](u64 i) const;

		char* CharArray();
		const char* CharArray() const;

		u64 Length() const;

		static String ToUpper(const String& str);
		static String ToLower(const String& str);

		friend bool operator==(const String& one, const String& other);
		friend bool operator!=(const String& one, const String& other);

		friend String operator+(const String& lhs, const String& rhs);
		String& operator+=(const String& other);

	private:
		u64 m_Length;
		char* m_Data;
	};

}
