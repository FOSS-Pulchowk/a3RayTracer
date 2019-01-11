#include "Image.h"

#include <iostream>

// TODO(Zero): Remove dependencies
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG // We will only be implementing PNG when we remove STB image
#include "External/STBImage.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "External/STBTrueType.h"

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

x::ttfont* x::LoadTTFont(memory_arena& stack, s8 fileName)
{
	i32 width, height, xoffset, yoffset;
	file_read_info file = ReadEntireFile(fileName);
	if(file.Buffer)
	{
		stbtt_fontinfo fontInfo;

		stbtt_InitFont(&fontInfo, (u8*)file.Buffer, stbtt_GetFontOffsetForIndex((u8*)file.Buffer, 0));
		u8* bitmap = stbtt_GetCodepointBitmap(&fontInfo, 0, stbtt_ScaleForPixelHeight(&fontInfo, 100), 'A', &width, &height, &xoffset, &yoffset);
		xAssert(bitmap);

		x::ttfont* result = xPush(stack, x::ttfont);

#if 1
		result->Pixels = xPushArray(stack, u8, width * height * 4);
		u8* destRow = (u8*)((u32*)result->Pixels + width * height - width);
		u8* source = bitmap;
		for(i32 y = 0; y < height; ++y)
		{
			u32* dest = (u32*)destRow;
			for(i32 x = 0; x < width; ++x)
			{
				*dest++ = (u32)*source << 24;
				source++;
			}
			destRow -= width * 4;
		}
#else
		result->Pixels = xPushArray(arena, u8, width * height);
		u8* destRow = result->Pixels + width * height - width;
		u8* source = bitmap;
		for (i32 y = 0; y < height; ++y)
		{
			u8* dest = destRow;
			for (i32 x = 0; x < width; ++x)
			{
				*dest++ = *source++;
			}
			destRow -= width;
		}
#endif

		stbtt_FreeBitmap(bitmap, 0);
		result->Width = width;
		result->Height = height;
		result->XOffset = xoffset;
		result->YOffset = yoffset;
		FreeFileContent(file);

		return result;
	}
	return 0;
}