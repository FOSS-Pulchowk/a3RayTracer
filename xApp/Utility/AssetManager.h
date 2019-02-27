#pragma once
#include "Common/Core.h"
#include "Utility/AssetData.h"

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
	a3::image* LoadImageFromBuffer(u64 id, void* buffer, u64 length);
	a3::image* LoadImageFromFile(u64 id, s8 file);
	a3::font* LoadFontFromBuffer(u64 id, void* buffer, u64 length, f32 scale);
	a3::font* LoadFontFromFile(u64 id, s8 file, f32 scale);
	a3::image_texture* LoadTexture2DFromPixels(u64 id, void* pixels, i32 w, i32 h, i32 channels, a3::filter filter, a3::wrap wrap);
	a3::image_texture* LoadTexture2DFromBuffer(u64 id, void* buffer, i32 length, a3::filter filter, a3::wrap wrap);
	a3::image_texture* LoadTexture2DFromFile(u64 id, s8 file, a3::filter filter, a3::wrap wrap);
	a3::font_texture* LoadFontTextureAtlasFromBuffer(u64 id, void* buffer, i32 length, f32 scale);
	a3::font_texture* LoadFontTextureAtlasFromFile(u64 id, s8 file, f32 scale);
	a3::mesh* LoadMeshFromBuffer(u64 id, void* buffer, u64 len);
	a3::mesh* LoadMeshFromFile(u64 id, s8 file);

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
#include "Platform/HardwarePlatform.h"

#define A3_ASSET_NUM_JUMP_ON_FULL 10

#define a3IsOutOfMemory(x) if(!(x)) { a3LogWarn("Out of memory"); return A3NULL; }
#define a3IsBufferTooLarge(x) if((x) > (u64)max_i32) { a3LogWarn("Buffer too large"); return A3NULL; }

namespace a3 {
	a3_asset Asset = {};
}

void a3_asset::Resize(u64 count)
{
	void* temp = a3Reallocate(m_Assets, count * sizeof(void*), void);
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

a3::image* a3_asset::LoadImageFromBuffer(u64 id, void* buffer, u64 length)
{
	if (m_AssetsCount <= id) Resize(id + A3_ASSET_NUM_JUMP_ON_FULL);
	u64 size = a3::QueryDecodedImageSize(buffer, (i32)length);
	a3IsBufferTooLarge(size + sizeof(a3::image));
	m_Assets[id] = a3Reallocate(m_Assets[id], size + sizeof(a3::image), void);
	a3IsOutOfMemory(m_Assets[id]);
	a3::image* m = (a3::image*)m_Assets[id];
	u8* dest = (u8*)m_Assets[id] + sizeof(a3::image);
	*m = a3::DecodeImageFromBuffer(buffer, (i32)length, dest);
	return m;
}

a3::image* a3_asset::LoadImageFromFile(u64 id, s8 file)
{
	a3::file_content fc = a3::Platform.LoadFileContent(file);
	a3::image* res = LoadImageFromBuffer(id, fc.Buffer, fc.Size);
	a3::Platform.FreeFileContent(fc);
	return res;
}

a3::font* a3_asset::LoadFontFromBuffer(u64 id, void* buffer, u64 length, f32 scale)
{
	if (m_AssetsCount <= id) Resize(id + A3_ASSET_NUM_JUMP_ON_FULL);
	u64 size = a3::QueryDecodedFontSize(buffer, (i32)length, scale);
	a3IsBufferTooLarge(size + sizeof(a3::font));
	m_Assets[id] = a3Reallocate(m_Assets[id], size + sizeof(a3::font), void);
	a3IsOutOfMemory(m_Assets[id]);
	a3::font* m = (a3::font*)m_Assets[id];
	u8* dest = (u8*)m_Assets[id] + sizeof(a3::font);
	*m = a3::DecodeFontFromBuffer(buffer, scale, dest);
	return m;
}

a3::font* a3_asset::LoadFontFromFile(u64 id, s8 file, f32 scale)
{
	a3::file_content fc = a3::Platform.LoadFileContent(file);
	a3::font* res = LoadFontFromBuffer(id, fc.Buffer, fc.Size, scale);
	a3::Platform.FreeFileContent(fc);
	return res;
}

a3::image_texture* a3_asset::LoadTexture2DFromPixels(u64 id, void * pixels, i32 w, i32 h, i32 channels, a3::filter filter, a3::wrap wrap)
{
	if (m_AssetsCount <= id) Resize(id + A3_ASSET_NUM_JUMP_ON_FULL);
	a3IsBufferTooLarge(sizeof(a3::image_texture));
	m_Assets[id] = a3Reallocate(m_Assets[id], sizeof(a3::image_texture), void);
	a3IsOutOfMemory(m_Assets[id]);
	a3::image_texture* m = (a3::image_texture*)m_Assets[id];
	*m = a3::GPU.CreateTexture2DFromBuffer(filter, wrap, pixels, w, h, channels);
	return m;
}

a3::image_texture* a3_asset::LoadTexture2DFromBuffer(u64 id, void* buffer, i32 length, a3::filter filter, a3::wrap wrap)
{
	u64 size = a3::QueryDecodedImageSize(buffer, length);
	u8* dest = a3New u8[size];
	a3::image img = a3::DecodeImageFromBuffer(buffer, length, dest);
	a3::image_texture* res = LoadTexture2DFromPixels(id, img.Pixels, img.Width, img.Height, img.Channels, filter, wrap);
	delete[] dest;
	return res;
}

a3::image_texture* a3_asset::LoadTexture2DFromFile(u64 id, s8 file, a3::filter filter, a3::wrap wrap)
{
	a3::file_content fc = a3::Platform.LoadFileContent(file);
	a3::image_texture* res = LoadTexture2DFromBuffer(id, fc.Buffer, (i32)fc.Size, filter, wrap);
	a3::Platform.FreeFileContent(fc);
	return res;
}

a3::font_texture* a3_asset::LoadFontTextureAtlasFromBuffer(u64 id, void * buffer, i32 length, f32 scale)
{
	if (m_AssetsCount <= id) Resize(id + A3_ASSET_NUM_JUMP_ON_FULL);
	a3IsBufferTooLarge(sizeof(a3::font_texture));
	m_Assets[id] = a3Reallocate(m_Assets[id], sizeof(a3::font_texture), void);
	a3IsOutOfMemory(m_Assets[id]);
	a3::font_texture* ft = (a3::font_texture*)m_Assets[id];
	i32 x, y, w, h;
	a3::QueryMaxFontDimension(buffer, length, scale, &x, &y);
	a3::QueryAtlasSizeForFontSize(x, y, &w, &h);
	u8* temp = a3New u8[x*y];
	ft->Texture = a3::GPU.CreateTexture2DFromBuffer(a3::FilterLinear, a3::WrapClampToEdge, A3NULL, w, h, 1);
	a3::ResterizeFontsToBuffer(&ft->AtlasInfo, buffer, length, scale, temp, [](void* userData, i32 w, i32 h, u8* data, i32 xoffset, i32 yoffset)
	{
		a3::GPU.DrawSubImageTexture2D((a3::image_texture*)userData, xoffset, yoffset, w, h, 1, data);
	}, &ft->Texture);
	delete[] temp;
	return ft;
}

a3::font_texture* a3_asset::LoadFontTextureAtlasFromFile(u64 id, s8 file, f32 scale)
{
	a3::file_content fc = a3::Platform.LoadFileContent(file);
	a3::font_texture* res = a3::Asset.LoadFontTextureAtlasFromBuffer(id, fc.Buffer, (i32)fc.Size, scale);
	a3::Platform.FreeFileContent(fc);
	return res;
}

a3::mesh * a3_asset::LoadMeshFromBuffer(u64 id, void * buffer, u64 len)
{
	if (m_AssetsCount <= id) Resize(id + A3_ASSET_NUM_JUMP_ON_FULL);

	a3::mesh_size ms = a3::QueryMeshSizeFromBuffer(buffer, len);
	
	v3* vertices = 0;
	v2* texCoords = 0;
	v3* normals = 0;
	u32* inVertices = 0;
	u32* inTexCoords = 0;
	u32* inNorms = 0;

	u64 size = ms.VerticesSize + ms.TextureCoordsSize + ms.NormalsSize + ms.VertexIndicesSize + ms.TextureCoordsIndicesSize + ms.NormalIndicesSize + sizeof(a3::mesh);
	a3IsBufferTooLarge(sizeof(a3::mesh) + size);

	m_Assets[id] = a3Reallocate(m_Assets[id], size, void*);
	a3IsOutOfMemory(m_Assets[id]);

	a3::mesh* result = (a3::mesh*)m_Assets[id];
	u8* ptr = (u8*)m_Assets[id] + sizeof(a3::mesh);

	if (ms.VerticesSize) vertices = (v3*)ptr;
	ptr += ms.VerticesSize;
	
	if (ms.TextureCoordsSize) texCoords = (v2*)ptr;
	ptr += ms.TextureCoordsSize;
	
	if (ms.NormalsSize) normals = (v3*)ptr;
	ptr += ms.NormalsSize;

	if (ms.VertexIndicesSize) inVertices = (u32*)ptr;
	ptr += ms.VertexIndicesSize;

	if (ms.TextureCoordsIndicesSize) inTexCoords = (u32*)ptr;
	ptr += ms.TextureCoordsIndicesSize;

	if (ms.NormalIndicesSize) inNorms = (u32*)ptr;
	ptr += ms.NormalIndicesSize;

	*result = a3::DecodeMeshFromBuffer(buffer, len, &ms, vertices, texCoords, normals, inVertices, inTexCoords, inNorms);

	return result;
}

a3::mesh * a3_asset::LoadMeshFromFile(u64 id, s8 file)
{
	a3::file_content fc = a3::Platform.LoadFileContent(file);
	a3::mesh* res = a3::Asset.LoadMeshFromBuffer(id, fc.Buffer, fc.Size);
	a3::Platform.FreeFileContent(fc);
	return res;
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
