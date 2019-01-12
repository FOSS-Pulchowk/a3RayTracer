#include "Image.h"
#include "Platform/Platform.h"

// TODO(Zero): Remove dependencies
#define STBI_NO_STDIO
#define STBI_NO_GIF
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
	i32 x, y, n;
	stbi_set_flip_vertically_on_load(1);
	x::file_content fc = Platform.LoadFileContent(file);
	if (!fc.Buffer) return 0;
	// TODO(Zero): Be careful need to assert here for the size
	u8* pixels = stbi_load_from_memory((u8*)fc.Buffer, (i32)fc.Size, &x, &y, &n, 4);
	Platform.FreeFileContent(fc);
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

		i32 x0, y0, x1, y1;
		f32 scale_y = stbtt_ScaleForPixelHeight(&fontInfo, scale);
		stbtt_GetCodepointBitmapBox(&fontInfo, 'A', 1, 1, &x0, &y0, &x1, &y1);
		i32 w = x1 - x0;
		i32 h = y1 - y0;
		x::ttf* ret = xPush(stack, x::ttf);
		u8* result = xPushArray(stack, u8, w * h);
		stbtt_MakeCodepointBitmap(&fontInfo, result, w, h, 1, 1, 1, 'A');
		ret->Width = w;
		ret->Height = h;
		ret->Pixels = result;
		stbi_write_png("output.png", ret->Width, ret->Height, 1, ret->Pixels, 1);
		return ret;

		//u8* bitmap = stbtt_GetCodepointBitmap(&fontInfo, 0, stbtt_ScaleForPixelHeight(&fontInfo, 100), 'A', &width, &height, &xoffset, &yoffset);
		//xAssert(bitmap);
		//stbtt_FreeBitmap(bitmap, 0);
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


		//return result;
	}
	return 0;
}