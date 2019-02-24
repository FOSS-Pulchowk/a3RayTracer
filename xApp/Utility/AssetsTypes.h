#pragma once
#include "Common/Core.h"
#include "STBLibs.h"

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

struct image_texture
{
	u32 Id;
	i32 Width;
	i32 Height;
};

struct font_texture
{
	a3::image_texture Texture;
	a3::font_atlas_info AtlasInfo;
};

}
