#pragma once
#include "Common/Core.h"
#include "STBLibs.h"

//
// DECLARATIONS
//

#define A3MAXLOADGLYPHX 16
#define A3MAXLOADGLYPHY 16
#define A3MAXMATERIALLENGTH 20

namespace a3 {

	struct image
	{
		u8* Pixels;
		i32 Width;
		i32 Height;
		i32 Channels;
	};

	struct character
	{
		i32 GlyphIndex;
		i32 OffsetX;
		i32 OffsetY;
		i32 Width;
		i32 Height;
		b32 HasBitmap;
		f32 NormalX0;
		f32 NormalX1;
		f32 NormalY0;
		f32 NormalY1;
		f32 Advance;
	};

	struct font
	{
		stbtt_fontinfo Info;
		u32 AtlasWidth;
		u32 AtlasHeight;
		u8* Atlas;
		f32 ScalingFactor;
		f32 HeightInPixels;
		character Characters[A3MAXLOADGLYPHX * A3MAXLOADGLYPHY];
	};

	struct font_atlas_info
	{
		stbtt_fontinfo Info;
		f32 ScalingFactor;
		f32 HeightInPixels;
		character Characters[A3MAXLOADGLYPHX * A3MAXLOADGLYPHY];
	};

	struct mesh
	{
		v3* Vertices;
		v2* TextureCoords;
		v3* Normals;
		u32* VertexIndices;
		u32* TextureCoordsIndices;
		u32* NormalIndices;
		u32 NumOfTriangles;
	};

	struct mesh_info
	{
		u64 VerticesSize;
		u64 TextureCoordsSize;
		u64 NormalsSize;
		u64 VertexIndicesSize;
		u64 TextureCoordsIndicesSize;
		u64 NormalIndicesSize;
		u32 NumOfTriangles;
		utf8 Material[A3MAXMATERIALLENGTH];
	};

	struct image_texture
	{
		u32 Id;
		i32 Width;
		i32 Height;
	};

	struct font_texture
	{
		image_texture Texture;
		font_atlas_info AtlasInfo;
	};

	enum texture_type
	{
		TextureType2D
	};

	enum filter
	{
		FilterLinear, FilterNearest
	};

	enum wrap
	{
		WrapClampToEdge, WrapRepeat
	};

	u64 QueryDecodedImageSize(void* buffer, i32 length);
	image DecodeImageFromBuffer(void* imgeBuffer, i32 length, void* destination);
	u64 QueryEncodedImageSize(i32 w, i32 h, i32 channels, i32 bytesPerPixel, void* pixels);
	u64 WriteImageToBuffer(void* buffer, i32 width, i32 height, i32 channels, i32 bytesPerPixel, void* pixels);

	u64 QueryDecodedFontSize(void* buffer, i32 length, f32 scale);
	void QueryMaxFontDimension(void * buffer, i32 length, f32 scale, i32* x, i32* y);
	void QueryAtlasSizeForFontSize(i32 x, i32 y, i32* w, i32* h);
	font DecodeFontFromBuffer(void* buffer, f32 scale, void* destination);
	f32 QueryTTFontKernalAdvance(const stbtt_fontinfo & info, f32 scalingFactor, i32 glyph0, i32 glyph1);

	// NOTE(Zero): Returns number of triangles
	/*u32 ScanMeshFromBuffer(void* buffer, void(*fnVertex)(f32, f32, f32, void*), void(*fnTexCoords)(f32, f32, void*),
						   void(*fnNorms)(f32, f32, void*),
						   void(*fnFaces)(u32, u32, u32, void*), void* ctx);*/

	mesh_info QueryMeshSizeFromBuffer(void* buffer, u64 length);
	mesh DecodeMeshFromBuffer(void* buffer, u64 length, mesh_info* pms, v3* pVertices, v2* pTexCoords, v3* pNormals, u32* pVertexIndices, u32* pTexCoordsIndices, u32* pNormalIndices);

}

//
// DEFINATIONS
//

#ifdef A3_IMPLEMENT_ASSETDATA

#include "Utility/Stream.h"


#define A3_MAX_LOAD_GLYPH (A3MAXLOADGLYPHX * A3MAXLOADGLYPHY)

struct a3_buffer
{
	void* buffer;
	u64 size;
};

static inline void a3_STBWriteCallback(void* context, void* data, i32 size)
{
	a3::file_content* fc = (a3::file_content*)context;
	fc->Buffer = a3New u8[size];
	if (fc->Buffer)
	{
		fc->Size = size;
		a3::MemoryCopy(fc->Buffer, data, size);
	}
}

static inline void a3_STBWriteBufferCallback(void* context, void* data, i32 size)
{
	a3_buffer* fc = (a3_buffer*)context;
	fc->buffer = a3New u8[size];
	if (fc->buffer)
	{
		fc->size = size;
		a3::MemoryCopy(fc->buffer, data, size);
	}
}

static inline void a3_STBQueryImageSizeCallback(void* context, void* data, i32 size)
{
	a3_buffer* fc = (a3_buffer*)context;
	fc->size = size;
}

u64 a3::QueryDecodedImageSize(void * buffer, i32 length)
{
	stbi_set_flip_vertically_on_load(1);
	i32 w, h, n;
	stbi_info_from_memory((u8*)buffer, length, &w, &h, &n);
	return w * h * n * sizeof(u8);
}

a3::image a3::DecodeImageFromBuffer(void * imgeBuffer, i32 length, void * destination)
{
	i32 x, y, n;
	stbi_info_from_memory((u8*)imgeBuffer, length, &x, &y, &n);
	u8* pixels = stbi_load_from_memory((u8*)imgeBuffer, length, &x, &y, &n, n);
	if (!pixels)
	{
		return {};
	}
	a3::image result;
	result.Width = x;
	result.Height = y;
	result.Channels = n;
	result.Pixels = (u8*)destination;
	a3::MemoryCopy(destination, pixels, x*y*n);
	stbi_image_free(pixels);
	return result;
}

u64 a3::QueryEncodedImageSize(i32 w, i32 h, i32 channels, i32 bytesPerPixel, void * pixels)
{
	i32 stride;
	if (channels == 1)
		stride = w;
	else if (channels == 3) // TODO(Zero): Test for images with 3 channels
		stride = 4 * ((w * bytesPerPixel + 3) / 4);
	else if (channels == 4)
		stride = w;
	else
		return false; // NOTE(Zero): Ouput only for 1, 3 and 4 channel images

	a3_buffer buffer = {};
	if (stbi_write_png_to_func(a3_STBQueryImageSizeCallback, &buffer, w, h, channels, pixels, stride))
	{
		return buffer.size;
	}
	return 0;
}

u64 a3::WriteImageToBuffer(void * destination, i32 width, i32 height, i32 channels, i32 bytesPerPixel, void * pixels)
{
	stbi_flip_vertically_on_write(1);
	// NOTE(Zero): Stride for images should align to multiple of 4 or 8
	// This is done for SSE optimizations and such
	// Stride reference : https://en.wikipedia.org/wiki/Stride_of_an_array
	// Here the stride is for pitch of the image and not for the pixel
	// Byte alignment didn't work for the single channel image
	// `bytesPerPixel` is only use by images with 3 channel, it's not mistake
	i32 stride;
	if (channels == 1)
		stride = width;
	else if (channels == 3) // TODO(Zero): Test for images with 3 channels
		stride = 4 * ((width * bytesPerPixel + 3) / 4);
	else if (channels == 4)
		stride = width;
	else
		return false; // NOTE(Zero): Ouput only for 1, 3 and 4 channel images

	a3_buffer buffer = {};
	if (stbi_write_png_to_func(a3_STBWriteBufferCallback, &buffer, width, height, channels, pixels, stride))
	{
		a3::MemoryCopy(destination, buffer.buffer, buffer.size);
		a3Delete[] buffer.buffer;
		return buffer.size;
	}
	return 0;
}

static inline void a3_CalculateFontBitmapMaxDimension(stbtt_fontinfo& info, f32 scale, i32* w, i32* h)
{
	// NOTE(Zero):
	// Maximum dimention for the bitmap required by the largest character in the font
	// Maximum height is set as the given scale factor
	// 0.5 is added to properly ceil when casting to integer
	// 1 is added to the height so that there's a pixel of gap between each bitmaps
	// If 1 is not added, there may be chance that a line of another bitmap may be shown
	i32 maxWidth = 0;
	i32 maxHeight = (u32)(scale + 0.5f) + 1;

	// NOTE(Zero):
	// Now the scale is changed into unit of points and it is no more pixel
	f32 pscale = stbtt_ScaleForPixelHeight(&info, scale);

	for (i32 index = 0; index < A3_MAX_LOAD_GLYPH; ++index)
	{
		int glyphIndex = stbtt_FindGlyphIndex(&info, index);
		i32 leftSizeBearing; // NOTE(Zero): Ignored
		i32 advance;
		stbtt_GetGlyphHMetrics(&info, glyphIndex, &advance, &leftSizeBearing);
		if (stbtt_IsGlyphEmpty(&info, glyphIndex))
		{
			continue;
		}
		else
		{
			i32 x0, x1, y0, y1;
			stbtt_GetGlyphBitmapBox(&info, glyphIndex, pscale, pscale, &x0, &y0, &x1, &y1);
			i32 w = x1 - x0; i32 h = y1 - y0;
			if (maxWidth < w)maxWidth = w;
		}
	}
	*w = maxWidth + 5; // NOTE(Zero): Similarly for width as well
	*h = maxHeight;
}

u64 a3::QueryDecodedFontSize(void * buffer, i32 length, f32 scale)
{
	stbtt_fontinfo info;
	stbtt_InitFont(&info, (u8*)buffer, stbtt_GetFontOffsetForIndex((u8*)buffer, 0));
	i32 mw, mh;
	a3_CalculateFontBitmapMaxDimension(info, scale, &mw, &mh);
	return (sizeof(u8) * A3_MAX_LOAD_GLYPH * mw * mh);
}

void a3::QueryMaxFontDimension(void * buffer, i32 length, f32 scale, i32* x, i32* y)
{
	stbtt_fontinfo info;
	stbtt_InitFont(&info, (u8*)buffer, stbtt_GetFontOffsetForIndex((u8*)buffer, 0));
	a3_CalculateFontBitmapMaxDimension(info, scale, x, y);
}

void a3::QueryAtlasSizeForFontSize(i32 x, i32 y, i32* w, i32* h)
{
	*w = x * A3MAXLOADGLYPHX * A3MAXLOADGLYPHY;
	*h = y;
}

a3::font a3::DecodeFontFromBuffer(void * buffer, f32 scale, void * destination)
{
	stbtt_fontinfo info;
	stbtt_InitFont(&info, (u8*)buffer, stbtt_GetFontOffsetForIndex((u8*)buffer, 0));
	a3::font result;
	result.Info = info;
	result.HeightInPixels = scale;

	// NOTE(Zero):
	// These temp buffers are used to store extracted bitmap from stb
	// This is requied because stb extracts bitmap from top-bottom
	// But we use from bottom-top, so we use this buffer to flip the bitmap vertically
	u8* tempBuffer = A3NULL;
	u64 tempBufferSize = 0;

	// NOTE(Zero):
	// Maximum dimention for the bitmap required by the largest character in the font
	// Maximum height is set as the given scale factor
	// 0.5 is added to properly ceil when casting to integer
	// 1 is added to the height so that there's a pixel of gap between each bitmaps
	// If 1 is not added, there may be chance that a line of another bitmap may be shown
	i32 maxWidth = 0;
	i32 maxHeight = (u32)(scale + 0.5f) + 1;

	// NOTE(Zero):
	// Now the scale is changed into unit of points and it is no more pixel
	f32 pscale = stbtt_ScaleForPixelHeight(&result.Info, scale);
	result.ScalingFactor = pscale;

	u8* bitmaps[A3_MAX_LOAD_GLYPH];
	for (i32 index = 0; index < A3_MAX_LOAD_GLYPH; ++index)
	{
		a3::character* c = &result.Characters[index];
		c->GlyphIndex = stbtt_FindGlyphIndex(&result.Info, index);
		i32 leftSizeBearing; // NOTE(Zero): Ignored
		i32 advance;
		stbtt_GetGlyphHMetrics(&result.Info, c->GlyphIndex, &advance, &leftSizeBearing);
		c->Advance = (f32)advance * pscale;
		if (stbtt_IsGlyphEmpty(&result.Info, c->GlyphIndex))
		{
			c->HasBitmap = false;
			continue;
		}
		else
		{
			c->HasBitmap = true;
			i32 x0, x1, y0, y1;
			stbtt_GetGlyphBitmapBox(&result.Info, c->GlyphIndex, pscale, pscale, &x0, &y0, &x1, &y1);
			i32 w = x1 - x0; i32 h = y1 - y0;
			if (tempBufferSize < w*h)
			{
				tempBuffer = a3Realloc(tempBuffer, w*h, u8);
				tempBufferSize = w * h;
			}
			// NOTE(Zero): Here stride is equal to width because OpenGL wants packed pixels
			i32 stride = w;
			stbtt_MakeGlyphBitmap(&result.Info, tempBuffer, w, h, stride, pscale, pscale, c->GlyphIndex);
			c->OffsetX = x0;
			c->OffsetY = -y1;
			c->Width = w;
			c->Height = h;
			bitmaps[index] = a3New u8[w*h];
			a3::ReverseRectCopy(bitmaps[index], tempBuffer, w, h);
			if (maxWidth < w)maxWidth = w;
		}
	}
	a3::Platform.Free(tempBuffer);
	maxWidth++; // NOTE(Zero): Similar case as in height

	// NOTE(Zero):
	// Here we multiple max dimensions by 16 to get altas dimension
	// We extract bitmaps for 256(0-255) characters from the font file
	// sqrt(256) = 16, thus 16 is used here
	result.AtlasWidth = A3MAXLOADGLYPHX * maxWidth;
	result.AtlasHeight = A3MAXLOADGLYPHY * maxHeight;
	result.Atlas = (u8*)destination;

	for (i32 blockY = 0; blockY < A3MAXLOADGLYPHY; ++blockY)
	{
		u8* destBlockY = result.Atlas + blockY * maxHeight * result.AtlasWidth;
		for (i32 blockX = 0; blockX < A3MAXLOADGLYPHX; ++blockX)
		{
			u8* destBlock = destBlockY + blockX * maxWidth;
			i32 index = blockY * A3MAXLOADGLYPHX + blockX;
			a3::character* c = &result.Characters[index];
			c->NormalX0 = (f32)(blockX * maxWidth) / (f32)result.AtlasWidth;
			c->NormalX1 = (f32)(blockX * maxWidth + maxWidth) / (f32)result.AtlasWidth;
			c->NormalY0 = (f32)(blockY * maxHeight) / (f32)result.AtlasHeight;
			c->NormalY1 = (f32)(blockY * maxHeight + scale) / (f32)result.AtlasHeight;
			if (c->HasBitmap)
			{
				u8* src = bitmaps[index];
				for (i32 y = 0; y < c->Height; ++y)
				{
					u8* dest = destBlock + y * result.AtlasWidth;
					for (i32 x = 0; x < c->Width; ++x)
					{
						*(dest++) = *(src++);
					}
				}
				c->Width = maxWidth;
				c->Height = maxHeight;
				a3Delete[] bitmaps[index];
			}
		}
	}
	return result;
}

f32 a3::QueryTTFontKernalAdvance(const stbtt_fontinfo & info, f32 scalingFactor, i32 glyph0, i32 glyph1)
{
	i32 res = stbtt_GetGlyphKernAdvance(&info, glyph0, glyph1);
	return res * scalingFactor;
}

a3::mesh_info a3::QueryMeshSizeFromBuffer(void * buffer, u64 length)
{
	a3::mesh_info result = {};

	u32 nVertices = 0;
	u32 nTexCoords = 0;
	u32 nNormals = 0;
	u32 nVertexIndices = 0;
	u32 nTexCoordIndices = 0;
	u32 nNormalIndices = 0;
	u32 nTriangles = 0;

	a3::stream s;
	s.SetWorkingBuffer((u8*)buffer, length);
	a3::stream line;

	b32 fail = false;
	while (!fail)
	{
		if (s.GetWorkingBufferPointer() == s.GetWorkingBufferEnd() - 1) break;

		u64 len = s.GetWorkingBufferEnd() - s.GetWorkingBufferPointer();
		line.SetWorkingBuffer(s.GetWorkingBufferPointer(), len);
		u8 first = line.PopCurrentPointerValue();

		switch (first)
		{
		case '#':
		{
		} break;

		case 'v':
		{
			u8 id = line.GetCurrentPointerValue();
			if (id == ' ')
				nVertices++;
			else if (id == 't')
				nTexCoords++;
			else if (id == 'n')
				nNormals++;
			else
				fail = true;
		} break;

		case 'l':
		{
		} break;

		case 'o':
		{
		} break;

		case 's':
		{
		} break;

		case 'g':
		{
		} break;

		case 'f':
		{
			while (line.GetCurrentPointerValue() != '\n' && line.GetWorkingBufferPointer() != line.GetWorkingBufferEnd() - 1)
			{
				line.MoveCharacterForward();
				i32 nSlashes = line.CountInLine('/', ' ');

				if (nSlashes < 0 && nSlashes > 3)
				{
					fail = true;
					break;
				}

				if (nSlashes == 2)
				{
					u8 s0 = *(line.GetWorkingBufferPointer() + 1);
					u8 s1 = *(line.GetWorkingBufferPointer() + 2);
					if (s0 != s1)
						nSlashes++;
				}

				if (nSlashes == 1)
					nTexCoordIndices++;
				else if (nSlashes == 2)
					nNormalIndices++;
				else if (nSlashes == 3)
				{
					nTexCoordIndices++;
					nNormalIndices++;
				}

				nVertexIndices++;
				if (nVertexIndices % 3 == 0)
					nTriangles++;

				line.MoveLineForwardTo(' ');
			}

		} break;

		case 'm':
		{
			if (line.FindWordInLine("tllib") < 0) fail = true;
		} break;

		case 'u':
		{
			if (line.FindWordInLine("semtl") < 0)
			{
				fail = true;
			}
			else
			{
				line.MoveLineForwardPass(' ');
				result.Material[A3MAXMATERIALLENGTH - 1] = 0;
				for (i32 i = 0; i < A3MAXMATERIALLENGTH - 1; ++i)
				{
					if (line.GetCurrentPointerValue() == '\n' || line.GetCurrentPointerValue() == '\r')
					{
						result.Material[i] = 0;
						break;
					}
					result.Material[i] = line.PopCurrentPointerValue();
				}
			}
		} break;

		default:
		{
			fail = true;
		} break;
		}

		s.MoveToNextLine();
	}

	if (fail)
	{
		result = {};
		return result;
	}
	result.VerticesSize = sizeof(v3) * nVertices;
	result.TextureCoordsSize = sizeof(v2) * nTexCoords;
	result.NormalsSize = sizeof(v3) * nNormals;
	result.VertexIndicesSize = sizeof(u32) * nVertexIndices;
	result.TextureCoordsIndicesSize = sizeof(u32) * nTexCoordIndices;
	result.NormalIndicesSize = sizeof(u32) * nNormalIndices;
	result.NumOfTriangles = nTriangles;

	return result;
}

a3::mesh a3::DecodeMeshFromBuffer(void * buffer, u64 length, mesh_info* pms, v3 * pVertices, v2 * pTexCoords, v3 * pNormals, u32 * pVertexIndices, u32 * pTexCoordsIndices, u32 * pNormalIndices)
{
	a3::mesh result;
	result.Vertices = pVertices;
	result.TextureCoords = pTexCoords;
	result.Normals = pNormals;
	result.VertexIndices = pVertexIndices;
	result.TextureCoordsIndices = pTexCoordsIndices;
	result.NormalIndices = pNormalIndices;

	u32 nVertexIndices = (u32)(pms->VertexIndicesSize / sizeof(v3));
	u32 nTexCoordIndices = (u32)(pms->TextureCoordsIndicesSize / sizeof(v2));
	u32 nNormalIndices = (u32)(pms->NormalIndicesSize / sizeof(v3));

	a3::stream s;
	s.SetWorkingBuffer((u8*)buffer, length);
	a3::stream line;

	auto parseIndex = [](u8* buffer, b32* fail, u32 max) {
		i32 i = a3::ParseI32((utf8*)buffer);
		if (i < 0)
			i = max - i;
		else if (i > 0)
			i = i - 1;
		else
			*fail = true;
		return i;
	};

	b32 fail = false;
	while (!fail)
	{
		if (s.GetWorkingBufferPointer() == s.GetWorkingBufferEnd() - 1) break;

		u64 len = s.GetWorkingBufferEnd() - s.GetWorkingBufferPointer();
		line.SetWorkingBuffer(s.GetWorkingBufferPointer(), len);
		u8 first = line.PopCurrentPointerValue();

		switch (first)
		{
		case '#':
		{
		} break;

		case 'v':
		{
			u8 id = line.PopCurrentPointerValue();
			if (id == ' ')
			{
				f32 x = a3::ParseF32((utf8*)line.GetWorkingBufferPointer());
				line.MoveForwardPass(' ');
				f32 y = a3::ParseF32((utf8*)line.GetWorkingBufferPointer());
				line.MoveForwardPass(' ');
				f32 z = a3::ParseF32((utf8*)line.GetWorkingBufferPointer());
				*pVertices++ = v3{ x, y, z };
			}
			else if (id == 't')
			{
				f32 u = a3::ParseF32((utf8*)line.GetWorkingBufferPointer());
				line.MoveForwardPass(' ');
				f32 v = a3::ParseF32((utf8*)line.GetWorkingBufferPointer());
				*pTexCoords++ = v2{ u, v };
			}
			else if (id == 'n')
			{
				f32 x = a3::ParseF32((utf8*)line.GetWorkingBufferPointer());
				line.MoveForwardPass(' ');
				f32 y = a3::ParseF32((utf8*)line.GetWorkingBufferPointer());
				line.MoveForwardPass(' ');
				f32 z = a3::ParseF32((utf8*)line.GetWorkingBufferPointer());
				*pNormals++ = v3{ x, y, z };
			}
			else
			{
				fail = true;
			}
		} break;

		case 'l':
		{
		} break;

		case 'o':
		{
		} break;

		case 's':
		{
		} break;

		case 'g':
		{
		} break;

		case 'f':
		{
			while (line.GetCurrentPointerValue() != '\r' && line.GetCurrentPointerValue() != '\n' && line.GetWorkingBufferPointer() != line.GetWorkingBufferEnd() - 1)
			{
				line.MoveCharacterForward();
				i32 nSlashes = line.CountInLine('/', ' ');

				if (nSlashes < 0 && nSlashes > 3)
				{
					fail = true;
					break;
				}

				if (nSlashes == 2)
				{
					u8 s0 = *(line.GetWorkingBufferPointer() + 1);
					u8 s1 = *(line.GetWorkingBufferPointer() + 2);
					if (s0 != s1)
						nSlashes++;
				}

				if (nSlashes == 0)
				{
					*pVertexIndices++ = parseIndex(line.GetWorkingBufferPointer(), &fail, nVertexIndices);
					line.MoveLineForwardTo(' ');
				}
				if (nSlashes == 1)
				{
					*pVertexIndices++ = parseIndex(line.GetWorkingBufferPointer(), &fail, nVertexIndices);
					line.MoveForwardPass('/');
					*pTexCoordsIndices++ = parseIndex(line.GetWorkingBufferPointer(), &fail, nTexCoordIndices);
					line.MoveLineForwardTo(' ');
				}
				else if (nSlashes == 2)
				{
					*pVertexIndices++ = parseIndex(line.GetWorkingBufferPointer(), &fail, nVertexIndices);
					line.MoveForwardPass('/');
					line.MoveForwardPass('/');
					*pNormalIndices++ = parseIndex(line.GetWorkingBufferPointer(), &fail, nNormalIndices);
					line.MoveLineForwardTo(' ');
				}
				else if (nSlashes == 3)
				{
					*pVertexIndices++ = parseIndex(line.GetWorkingBufferPointer(), &fail, nVertexIndices);
					line.MoveForwardPass('/');
					*pTexCoordsIndices++ = parseIndex(line.GetWorkingBufferPointer(), &fail, nTexCoordIndices);
					line.MoveLineForwardTo('/');
					*pNormalIndices++ = parseIndex(line.GetWorkingBufferPointer(), &fail, nNormalIndices);
					line.MoveLineForwardTo(' ');
				}

			}

		} break;

		case 'm':
		{
			if (line.FindWordInLine("tllib") < 0) fail = true;
		} break;

		case 'u':
		{
			if (line.FindWordInLine("semtl") < 0) fail = true;
		} break;

		default:
		{
			fail = true;
		} break;
		}

		s.MoveToNextLine();
	}

	if (fail)
	{
		result = {};
		return result;
	}

	result.NumOfTriangles = pms->NumOfTriangles;
	return result;
}


#endif