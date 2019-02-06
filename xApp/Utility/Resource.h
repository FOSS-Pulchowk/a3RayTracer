#pragma once
#include "Common/Core.h"
#include "STBImplementation.h"

#define A3MAXLOADGLYPHX 16
#define A3MAXLOADGLYPHY 16

//
// DECLARATIONS
//

namespace a3 {

typedef void(*RasterizeFontCallback)(i32 w, i32 h, u8* buffer, i32 xOffset, i32 yOffset);

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

u64 QueryPixelBufferSize(void* buffer, i32 length);
image LoadImageFromBufer(void* imgeBuffer, i32 length, void* destination);
u64 QueryImageSize(i32 w, i32 h, i32 chaneels, i32 bytesPerPixel, void* pixels);
u64 WriteImageToBuffer(void* buffer, i32 width, i32 height, i32 channels, i32 bytesPerPixel, void* pixels);
u64 QueryFontSize(void* buffer, i32 length, f32 scale);
void QueryMaxFontDimension(void * buffer, i32 length, f32 scale, i32* x, i32* y);
void QueryAtlasSizeForFontSize(i32 x, i32 y, i32* w, i32* h);
void ResterizeFontsToBuffer(font_atlas_info* i, void* buffer, i32 length, f32 scale, void* drawBuffer, RasterizeFontCallback callback);
font LoadFontFromBuffer(void* buffer, f32 scale, void* destination);
f32 QueryTTFontKernalAdvance(const stbtt_fontinfo & info, f32 scalingFactor, i32 glyph0, i32 glyph1);

}
