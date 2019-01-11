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

struct ttfont
{
	u8* Pixels;
	i32 Width;
	i32 Height;
	i32 XOffset;
	i32 YOffset;
};

image* LoadImage(memory_arena& arena, s8 file);
ttfont* LoadTTFont(memory_arena& arena, s8 file);

}