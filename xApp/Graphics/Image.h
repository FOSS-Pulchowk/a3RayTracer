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

struct ttf_glyph
{
	i32 Width;
	i32 Height;
	i32 XOffset;
	i32 YOffset;
	i32 Position;
};

struct ttf
{
	u8* Pixels;
	i32 Width;
	i32 Height;
};

struct fonts
{
	u8 c;
	i32 width;
	i32 height;
	u8* pixels;
};

image* LoadPNGImage(memory_arena& arena, s8 file);
fonts* LoadTTFont(memory_arena& arena, s8 file, f32 scale);

}