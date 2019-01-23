#include "AssetManager.h"
#include "Platform/Platform.h"
#include "Utility/Resource.h"
#include "GLResources.h"

#define A3_ASSET_NUM_JUMP_ON_FULL 10

#define a3IsOutOfMemory(x) if(!(x)) { a3LogWarn("Out of memory"); return; }
#define a3IsBufferTooLarge(x) if((x) > (u64)max_i32) { a3LogWarn("Buffer too large"); return; }

namespace a3 {
	a3_asset Asset = {};
}

void a3_asset::Resize(u64 count)
{
	void* temp = a3::Platform.Recalloc(m_Assets, count * sizeof(void*));
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
	m_Assets[id] = a3::Platform.Realloc(m_Assets[id], size + sizeof(a3::image));
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
	m_Assets[id] = a3::Platform.Realloc(m_Assets[id], size + sizeof(a3::font));
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

void a3_asset::LoadTextureFromBuffer(u64 id, void* buffer, i32 length, u32 type, u32 filter, u32 wrap)
{
	if (m_AssetsCount <= id) Resize(id + A3_ASSET_NUM_JUMP_ON_FULL);
	a3IsBufferTooLarge(sizeof(a3::texture));
	u64 size = a3::QueryImageSize(buffer, length);
	u8* dest = new u8[size];
	a3::image img = a3::LoadImageFromBufer(buffer, length, dest);
	m_Assets[id] = a3::Platform.Realloc(m_Assets[id], sizeof(a3::texture));
	a3IsOutOfMemory(m_Assets[id]);
	a3::texture* m = (a3::texture*)m_Assets[id];
	*m = a3::GLMakeTextureFromBuffer(type, filter, wrap, img.Pixels, img.Width, img.Height, img.Channels);
	delete[] dest;
}

void a3_asset::LoadTextureFromFile(u64 id, s8 file, u32 type, u32 filter, u32 wrap)
{
	a3::file_content fc = a3::Platform.LoadFileContent(file);
	LoadTextureFromBuffer(id, fc.Buffer, (i32)fc.Size, type, filter, wrap);
	a3::Platform.FreeFileContent(fc);
}

void a3_asset::Free(u64 id)
{
	a3Assert(id < m_AssetsCount);
	a3::Platform.Free(m_Assets[id]);
	m_Assets[id] = A3NULL;
}
