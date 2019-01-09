#include "Image.h"

// TODO(Zero): Remove dependencies
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG // We will only be implementing PNG when we remove STB image
#include "External/STBImage.h"

x::image* x::LoadImage(memory_arena& arena, s8 file)
{
	int x, y, n;
	stbi_set_flip_vertically_on_load(1);
	u8* pixels = stbi_load(file, &x, &y, &n, 4);
	if(!pixels) return null;

	x::image* img = xPush(arena, x::image);
	img->Width = x;
	img->Height = y;
	img->Channels = n;
	img->Pixels = xPushArray(arena, u8, x * y * n);

	for(i32 i = 0; i < x*y*n; ++i)
		img->Pixels[i] = pixels[i];
	stbi_image_free(pixels);

	return img;
}