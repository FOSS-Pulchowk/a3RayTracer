#pragma once
#include "Common/Core.h"
#include "STBImplementation.h"

#define A3MAXLOADGLYPHX 16
#define A3MAXLOADGLYPHY 16

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
	character Characters[A3MAXLOADGLYPHX * A3MAXLOADGLYPHY];
};

u64 QueryImageSize(void* buffer, i32 length);
image LoadImageFromBufer(void* imgeBuffer, i32 length, void* destination);
b32 WriteImageToBuffer(void* buffer, i32 width, i32 height, i32 channels, i32 bytesPerPixel, void* pixels);
u64 QueryFontSize(void* buffer, i32 length, f32 scale);
font LoadFontFromBuffer(void* buffer, f32 scale, void* destination);
f32 GetTTFontKernalAdvance(const font& font, i32 glyph0, i32 glyph1);

}