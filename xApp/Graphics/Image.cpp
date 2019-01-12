#include "Image.h"
#include "Platform/Platform.h"

#include <iostream>

// TODO(Zero): Remove dependencies
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG // We will only be implementing PNG when we remove STB image
#include "External/STBImage.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "External/STBTrueType.h"

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "External/STBImageWrite.h"

x::image* x::LoadPNGImage(memory_arena& arena, s8 file)
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

x::ttf* x::LoadTTFont(memory_arena& stack, s8 fileName, f32 scale)
{
	x::file_content file = x::Platform.LoadFileContent(fileName);
	if(file.Buffer)
	{
		stbtt_fontinfo fontInfo;
		stbtt_InitFont(&fontInfo, (u8*)file.Buffer, stbtt_GetFontOffsetForIndex((u8*)file.Buffer, 0));
		//u8* bitmap = stbtt_GetCodepointBitmap(&fontInfo, 0, stbtt_ScaleForPixelHeight(&fontInfo, 100), 'A', &width, &height, &xoffset, &yoffset);
		//xAssert(bitmap);
		//stbtt_FreeBitmap(bitmap, 0);

		x::ttf* result = xPush(stack, x::ttf);
		result->Pixels = stack.Current;
		result->Width = 0;
		result->Height = (u32)ceilf(scale);

		i32 width, height, xoffset, yoffset;
		for (i32 codepoint = 1; codepoint < xArrayCount(result->Glyphs); ++codepoint)
		{
			u8* bitmap = stbtt_GetCodepointBitmap(&fontInfo, 0, stbtt_ScaleForPixelHeight(&fontInfo, scale), codepoint, &width, &height, &xoffset, &yoffset);
			if (!bitmap)
			{
				result->Glyphs[codepoint].Height = 0;
				result->Glyphs[codepoint].Width = 0;
				result->Glyphs[codepoint].XOffset = 0;
				result->Glyphs[codepoint].YOffset = 0;
				result->Glyphs[codepoint].Position = 0;
				continue;
			}
			u8* current = xPushArray(stack, u8, width * height);
			result->Width += width;
			u8* destRow = current + width * height - width;
			u8* source = bitmap;
			i32 y = 0;
			for (; y < height; ++y)
			{
				u8* dest = destRow;
				for (i32 x = 0; x < width; ++x)
				{
					*dest++ = *source++;
				}
				destRow -= width;
			}
			destRow = current + width * result->Height - width;
			xAssert(result->Height > height);
			for (; y < result->Height; ++y)
			{
				u8* dest = destRow;
				for (i32 x = 0; x < width; ++x)
				{
					*dest++ = 0;
				}
				destRow -= width;
			}
			result->Glyphs[codepoint-1].Width = width;
			result->Glyphs[codepoint-1].Height = height;
			result->Glyphs[codepoint-1].XOffset = xoffset;
			result->Glyphs[codepoint-1].YOffset = yoffset;
			result->Glyphs[codepoint-1].Position = result->Width - width;
			stbtt_FreeBitmap(bitmap, 0);
		}


#if 0
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

		result->Pixels = xPushArray(stack, u8, width * height);
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

		result->Width = width;
		result->Height = height;
		result->XOffset = xoffset;
		result->YOffset = yoffset;
		x::Platform.FreeFileContent(file);
#endif

		stbi_write_png("output.png", result->Width, result->Height, 1, result->Pixels, 1);

		return result;
	}
	return 0;
}