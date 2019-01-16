#include "Common/Core.h"
#include "Platform/Platform.h"

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
	i32 Advance;
	i32 LeftSideBearing;
	b32 HasBitmap;
	image Bitmap;
};

struct fonts
{
	character Characters[256];
	f32 ScalingFactor;
};

image* LoadPNGImage(memory_arena& arena, s8 file);
b32 WritePNGImage(s8 file, i32 width, i32 height, i32 channels, i32 bytesPerPixel, void* pixels);
fonts* LoadTTFont(memory_arena& arena, s8 file, f32 scale);

}