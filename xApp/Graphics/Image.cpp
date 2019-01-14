#include "Image.h"
#include "Platform/Platform.h"
#include "STBImplementation.h"

#define a3AspectRatio (16.0f / 9.0f)
#define a3AspectHeight(x) ((x) / a3AspectRatio)
#define a3AspectWidth(x) (a3AspectRatio * (x))

a3::image* a3::LoadPNGImage(memory_arena& arena, s8 file)
{
	i32 x, y, n;
	stbi_set_flip_vertically_on_load(1);
	a3::file_content fc = Platform.LoadFileContent(file);
	if (!fc.Buffer) return 0;
	a3Assert(fc.Size < (u64)max_i32);
	u8* pixels = stbi_load_from_memory((u8*)fc.Buffer, (i32)fc.Size, &x, &y, &n, 4);
	Platform.FreeFileContent(fc);
	if (!pixels)
	{
		a3LogWarn("{s} Image not loaded {s}", file, stbi_failure_reason());
		return null;
	}

	a3::image* img = a3Push(arena, a3::image);
	img->Width = x;
	img->Height = y;
	img->Channels = n;
	img->Pixels = a3PushArray(arena, u8, x * y * n);

	for(i32 i = 0; i < x*y*n; ++i)
		img->Pixels[i] = pixels[i];
	stbi_image_free(pixels);

	return img;
}

a3::fonts* a3::LoadTTFont(memory_arena& stack, s8 fileName, f32 scale)
{
	a3::file_content file = a3::Platform.LoadFileContent(fileName);
	if(file.Buffer)
	{
		stbtt_fontinfo fontInfo;
		stbtt_InitFont(&fontInfo, (u8*)file.Buffer, stbtt_GetFontOffsetForIndex((u8*)file.Buffer, 0));

		f32 scaleY = stbtt_ScaleForPixelHeight(&fontInfo, scale);
		f32 scaleX = a3AspectWidth(scaleY);

		fonts* glyphs= a3PushArray(stack, fonts, 255 - 32 + 1);
		MemorySet(glyphs, 0, 255 - 32 + 1);

		char output[] = "Platform/etc/img_x.png";
		i32 atlasWidth = 0;
		i32 atlasHeight = (i32)a3Ceilf(scaleY);
		for (i32 glyphIndex = 32; glyphIndex < 255; ++glyphIndex)
		{
			u32 cp = glyphIndex - 32;
			i32 x0, y0, x1, y1;
			i32 glyphCode = stbtt_FindGlyphIndex(&fontInfo, glyphIndex);
			stbtt_GetGlyphBitmapBox(&fontInfo, glyphCode, scaleX, scaleY, &x0, &y0, &x1, &y1);
			i32 w = x1 - x0;
			i32 h = y1 - y0;
			glyphs[cp].width = w;
			glyphs[cp].height = h;
			glyphs[cp].pixels = new u8[w*h];
			glyphs[cp].c = (u8)glyphIndex;
			stbtt_MakeGlyphBitmap(&fontInfo, glyphs[cp].pixels, w, h, sizeof(u8), scaleX, scaleY, glyphCode);
			atlasWidth += glyphs[cp].width;
			output[17] = glyphIndex;
			stbi_write_png(output, w, h, 1, glyphs[cp].pixels, 1);
		}
		return glyphs;
		//a3::ttf* ret = a3Push(stack, a3::ttf);
		//u8* result = a3PushArray(stack, u8, w * h);
		//stbtt_MakeCodepointBitmap(&fontInfo, result, w, h, 1, 1, 1, 'A');
		//ret->Width = w;
		//ret->Height = h;
		//ret->Pixels = result;
		//return ret;

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