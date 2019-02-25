#pragma once
#include "Common/Core.h"

namespace a3 {

	template <typename Type>
	void Swap(Type* a, Type* b);

}


namespace a3 {

	template<typename Type>
	void Swap(Type * a, Type * b)
	{
		Type temp = *a;
		*a = *b;
		*b = temp;
	}

}