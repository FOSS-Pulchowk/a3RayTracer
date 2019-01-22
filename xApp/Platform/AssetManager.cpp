#include "AssetManager.h"
#include "Platform/Platform.h"
#include "Graphics/Image.h"

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
	a3IsBufferTooLarge(length);
	u64 size = a3::QueryImageSize(buffer, (i32)length);
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
	a3IsBufferTooLarge(length);
	u64 size = a3::QueryFontSize(buffer, (i32)length, scale);
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

void a3_asset::LoadTexture(u64 id)
{
	// TODO(Zero):
	// Implement this!!
}

void a3_asset::Free(u64 id)
{
	a3Assert(id < m_AssetsCount);
	a3::Platform.Free(m_Assets[id]);
	m_Assets[id] = A3NULL;
}
