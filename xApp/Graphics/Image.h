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

struct font
{
	i32 glyph;
	i32 xMin;
	i32 xMax;
	i32 yMin;
	i32 yMax;
	i32 advance;
	i32 leftSideBearing;
	f32 scalingFactor;
	image bitmap;
};

image* LoadPNGImage(memory_arena& arena, s8 file);
b32 WritePNGImage(s8 file, i32 width, i32 height, i32 channels, i32 bytesPerPixel, void* pixels);
font* LoadTTFont(memory_arena& arena, s8 file, f32 scale);

}