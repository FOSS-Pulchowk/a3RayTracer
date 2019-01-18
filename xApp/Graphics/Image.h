#pragma once
#include "Common/Core.h"
#include "Platform/Platform.h"

struct stbtt_fontinfo;

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

struct fonts
{
	::stbtt_fontinfo* Info;
	u32 AtlasWidth;
	u32 AtlasHeight;
	u8* Atlas;
	f32 ScalingFactor;
	character Characters[256];
};

image* LoadPNGImage(memory_arena& arena, s8 file);
b32 WritePNGImage(s8 file, i32 width, i32 height, i32 channels, i32 bytesPerPixel, void* pixels);
fonts* LoadTTFont(memory_arena& arena, s8 file, f32 scale);
f32 GetTTFontKernalAdvance(const fonts& font, i32 glyph0, i32 glyph1);

}