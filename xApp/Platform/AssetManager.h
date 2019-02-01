#pragma once
#include "Common/Core.h"
#include "Assets.h"

//
// DECLARATIONS
//

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
	void LoadTexture2DFromBuffer(u64 id, void* buffer, i32 length, u32 filter, u32 wrap);
	void LoadTexture2DFromFile(u64 id, s8 file, u32 filter, u32 wrap);
	void LoadFontTextureAtlasFromBuffer(u64 id, void* buffer, i32 length, f32 scale);
	void LoadFontTextureAtlasFromFile(u64 id, s8 file, f32 scale);

	void Free(u64 id);

	template <typename Type>
	Type* Get(u64 id);
};

namespace a3 {
	extern a3_asset Asset;
}

//
// IMPLEMENTATION
//
#ifdef A3_IMPLEMENT_ASSETMANAGER

#include "Platform/Platform.h"

#define A3_ASSET_NUM_JUMP_ON_FULL 10

#define a3IsOutOfMemory(x) if(!(x)) { a3LogWarn("Out of memory"); return; }
#define a3IsBufferTooLarge(x) if((x) > (u64)max_i32) { a3LogWarn("Buffer too large"); return; }

namespace a3 {
	a3_asset Asset = {};
}

void a3_asset::Resize(u64 count)
{
	void* temp = a3::Platform.ResizeMemory(m_Assets, count * sizeof(void*));
	if (!temp)
	{
		a3LogWarn("Out of memory!");
	}
	else
	{
		m_AssetsCount = count;
		m_Assets = (void**)temp;
	}
}

void a3_asset::LoadImageFromBuffer(u64 id, void* buffer, u64 length)
{
	if (m_AssetsCount <= id) Resize(id + A3_ASSET_NUM_JUMP_ON_FULL);
	u64 size = a3::QueryImageSize(buffer, (i32)length);
	a3IsBufferTooLarge(size + sizeof(a3::image));
	m_Assets[id] = a3::Platform.ResizeMemory(m_Assets[id], size + sizeof(a3::image));
	a3IsOutOfMemory(m_Assets[id]);
	a3::image* m = (a3::image*)m_Assets[id];
	u8* dest = (u8*)m_Assets[id] + sizeof(a3::image);
	*m = a3::LoadImageFromBufer(buffer, (i32)length, dest);
}

void a3_asset::LoadImageFromFile(u64 id, s8 file)
{
	a3::file_content fc = a3::Platform.LoadFileContent(file);
	LoadImageFromBuffer(id, fc.Buffer, fc.Size);
	a3::Platform.FreeFileContent(fc);
}

void a3_asset::LoadFontFromBuffer(u64 id, void* buffer, u64 length, f32 scale)
{
	if (m_AssetsCount <= id) Resize(id + A3_ASSET_NUM_JUMP_ON_FULL);
	u64 size = a3::QueryFontSize(buffer, (i32)length, scale);
	a3IsBufferTooLarge(size + sizeof(a3::font));
	m_Assets[id] = a3::Platform.ResizeMemory(m_Assets[id], size + sizeof(a3::font));
	a3IsOutOfMemory(m_Assets[id]);
	a3::font* m = (a3::font*)m_Assets[id];
	u8* dest = (u8*)m_Assets[id] + sizeof(a3::font);
	*m = a3::LoadFontFromBuffer(buffer, scale, dest);
}

void a3_asset::LoadFontFromFile(u64 id, s8 file, f32 scale)
{
	a3::file_content fc = a3::Platform.LoadFileContent(file);
	LoadFontFromBuffer(id, fc.Buffer, fc.Size, scale);
	a3::Platform.FreeFileContent(fc);
}

void a3_asset::LoadTexture2DFromBuffer(u64 id, void* buffer, i32 length, u32 filter, u32 wrap)
{
	if (m_AssetsCount <= id) Resize(id + A3_ASSET_NUM_JUMP_ON_FULL);
	a3IsBufferTooLarge(sizeof(a3::texture));
	u64 size = a3::QueryImageSize(buffer, length);
	u8* dest = new u8[size];
	a3::image img = a3::LoadImageFromBufer(buffer, length, dest);
	m_Assets[id] = a3::Platform.ResizeMemory(m_Assets[id], sizeof(a3::texture));
	a3IsOutOfMemory(m_Assets[id]);
	a3::texture* m = (a3::texture*)m_Assets[id];
	*m = a3::GLMakeTexture2DFromBuffer(filter, wrap, img.Pixels, img.Width, img.Height, img.Channels);
	delete[] dest;
}

void a3_asset::LoadTexture2DFromFile(u64 id, s8 file, u32 filter, u32 wrap)
{
	a3::file_content fc = a3::Platform.LoadFileContent(file);
	LoadTexture2DFromBuffer(id, fc.Buffer, (i32)fc.Size, filter, wrap);
	a3::Platform.FreeFileContent(fc);
}

void a3_asset::LoadFontTextureAtlasFromBuffer(u64 id, void * buffer, i32 length, f32 scale)
{
	if (m_AssetsCount <= id) Resize(id + A3_ASSET_NUM_JUMP_ON_FULL);
	a3IsBufferTooLarge(sizeof(a3::font_texture));
	m_Assets[id] = a3::Platform.ResizeMemory(m_Assets[id], sizeof(a3::font_texture));
	a3IsOutOfMemory(m_Assets[id]);
	a3::font_texture* ft = (a3::font_texture*)m_Assets[id];
	i32 x, y, w, h;
	a3::QueryMaxFontDimension(buffer, length, scale, &x, &y);
	a3::QueryAtlasSizeForFontSize(x, y, &w, &h);
	u8* temp = new u8[x*y];
	ft->Texture = a3::GLMakeTexture(GL_TEXTURE_2D, GL_LINEAR, GL_CLAMP_TO_EDGE);
	a3GL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
	a3GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, A3NULL));
	a3::ResterizeFontsToBuffer(&ft->AtlasInfo, buffer, length, scale, temp, [](i32 w, i32 h, u8* data, i32 xoffset, i32 yoffset)
	{
		a3GL(glTexSubImage2D(GL_TEXTURE_2D, 0, xoffset, yoffset, w, h, GL_RED, GL_UNSIGNED_BYTE, data));
	});
	a3GL(glPixelStorei(GL_UNPACK_ALIGNMENT, 4));
	delete[] temp;
}

void a3_asset::LoadFontTextureAtlasFromFile(u64 id, s8 file, f32 scale)
{
	a3::file_content fc = a3::Platform.LoadFileContent(file);
	a3::Asset.LoadFontTextureAtlasFromBuffer(id, fc.Buffer, (i32)fc.Size, scale);
	a3::Platform.FreeFileContent(fc);
}

void a3_asset::Free(u64 id)
{
	a3Assert(id < m_AssetsCount);
	a3::Platform.Free(m_Assets[id]);
	m_Assets[id] = A3NULL;
}

#endif

template<typename Type>
inline Type* a3_asset::Get(u64 id)
{
	a3Assert(id < m_AssetsCount);
	return (Type*)m_Assets[id];
}
