#pragma once
#include "Common/Core.h"
#include "Platform/Platform.h"

#define a3AspectRatio (16.0f / 9.0f)
#define a3AspectHeight(width) ((width) / a3AspectRatio)
#define a3AspectWidth(height) (a3AspectRatio * (height))

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
	i32 XMin;
	i32 XMax;
	i32 YMin;
	i32 YMax;
	f32 BearingX;
	f32 BearingY;
	f32 Advance;
	i32 LeftSideBearing;
	b32 HasBitmap;
	image Bitmap;
};

struct fonts
{
	character Characters[256];
	f32 ScalingFactor;
};

struct gl_textures
{
	u32 textures[256];
	fonts* font;
};

image* LoadPNGImage(memory_arena& arena, s8 file);
b32 WritePNGImage(s8 file, i32 width, i32 height, i32 channels, i32 bytesPerPixel, void* pixels);
fonts* LoadTTFont(memory_arena& arena, s8 file, f32 scale);

}