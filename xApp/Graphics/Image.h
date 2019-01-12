#include "Common/Core.h"
#include "Platform/Platform.h"

namespace x {

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

image* LoadPNGImage(memory_arena& arena, s8 file);
ttf* LoadTTFont(memory_arena& arena, s8 file, f32 scale);

}