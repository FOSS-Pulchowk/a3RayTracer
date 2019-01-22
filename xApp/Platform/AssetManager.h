#pragma once
#include "Common/Core.h"

struct a3_asset
{
private:
	void** m_Assets;
	u64 m_AssetsCount;
	void Resize(u64 count);
public:
	void LoadImageFromBuffer(u64 id, void* buffer, u64 length);
	void LoadImageFromFile(u64 id, s8 file);
	void LoadFontFromBuffer(u64 id, void* buffer, u64 length, f32 scale);
	void LoadFontFromFile(u64 id, s8 file, f32 scale);
	void LoadTexture(u64 id);

	void Free(u64 id);

	template <typename Type>
	Type* Get(u64 id);
};

namespace a3 {
	extern a3_asset Asset;
}

template<typename Type>
inline Type* a3_asset::Get(u64 id)
{
	a3Assert(id < m_AssetsCount);
	return (Type*)m_Assets[id];
}
