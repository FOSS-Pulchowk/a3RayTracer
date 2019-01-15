#include "Image.h"
#include "Platform/Platform.h"
#include "STBImplementation.h"

#define a3AspectRatio (16.0f / 9.0f)
#define a3AspectHeight(x) ((x) / a3AspectRatio)
#define a3AspectWidth(x) (a3AspectRatio * (x))

static inline void InternalSTBWriteCallback(void* context, void* data, i32 size)
{
	a3::file_content* fc = (a3::file_content*)context;
	fc->Buffer = new u8[size];
	if (fc->Buffer)
	{
		fc->Size = size;
		a3::MemoryCopy(fc->Buffer, data, size);
	}
}

a3::image* a3::LoadPNGImage(memory_arena& arena, s8 file)
{
	stbi_set_flip_vertically_on_load(1);
	i32 x, y, n;
	a3::file_content fc = Platform.LoadFileContent(file);
	if (!fc.Buffer) return 0;
	a3Assert(fc.Size < (u64)max_i32);
	u8* pixels = stbi_load_from_memory((u8*)fc.Buffer, (i32)fc.Size, &x, &y, &n, 4);
	Platform.FreeFileContent(fc);

	if (!pixels) return null;

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

b32 a3::WritePNGImage(s8 file, i32 width, i32 height, i32 channels, i32 bytesPerPixel,  void* pixels)
{
	stbi_flip_vertically_on_write(1);
	a3::file_content fc = {};
	// NOTE(Zero): Stride for images should align to multiple of 4 or 8
	// This is done for SSE optimizations and such
	// Stride reference : https://en.wikipedia.org/wiki/Stride_of_an_array
	// Here the stride is for pitch of the image and not for the pixel
	// Byte alignment didn't work for the single channel image
	// `bytesPerPixel` is only use by images with 3 channel, it's not mistake
	i32 stride;
	if (channels == 1)
		stride = width;
	else if (channels == 3) // TODO(Zero): Test for images with 3 channels
		stride = 4 * ((width * bytesPerPixel + 3) / 4);
	else if (channels == 4)
		stride = width;
	else
		return false; // NOTE(Zero): Ouput only for 1, 3 and 4 channel images

	if (stbi_write_png_to_func(InternalSTBWriteCallback, &fc, width, height, channels, pixels, stride))
	{
		// HACK(Zero): Should we directly replace the file?
		b32 res = a3::Platform.WriteFileContent(file, fc);
		if (!res)
			res = a3::Platform.ReplaceFileContent(file, fc);
		delete[] fc.Buffer;
		return res;
	}
	return false;
}

a3::font* a3::LoadTTFont(memory_arena& stack, s8 fileName, f32 scale)
{
	a3::file_content file = a3::Platform.LoadFileContent(fileName);
	if(file.Buffer)
	{
		stbtt_fontinfo info;
		stbtt_InitFont(&info, (u8*)file.Buffer, stbtt_GetFontOffsetForIndex((u8*)file.Buffer, 0));

		a3::font* f = a3Push(stack, a3::font);
		f->glyph = stbtt_FindGlyphIndex(&info, 'B');
		stbtt_GetGlyphBox(&info, f->glyph, &f->xMin, &f->yMin, &f->xMax, &f->yMax);
		stbtt_GetGlyphHMetrics(&info, f->glyph, &f->advance, &f->leftSideBearing);
		f->scalingFactor = stbtt_ScaleForPixelHeight(&info, 50);
		f32 sx = f->scalingFactor * 16.0f / 9.0f;
		i32 x0, x1, y0, y1;
		stbtt_GetGlyphBitmapBox(&info, f->glyph, sx, f->scalingFactor, &x0, &y0, &x1, &y1);
		i32 w = x1 - x0;
		i32 h = y1 - y0;
		u8* bitmap = a3PushArray(stack, u8, w*h);

		// TODO(Zero): 
		// bitmap output will be upside down
		// should we invert the texture co-odrdinates or the image pixels?
		// load all fonts here, not just single font
		// handle the cases when fonts don't have bitmap such as newline and space

		// NOTE(Zero): Here stride is equal to width because OpenGL wants packed pixels
		i32 stride = w;
		stbtt_MakeGlyphBitmap(&info, bitmap, w, h, stride, sx, f->scalingFactor, f->glyph);
		f->bitmap.Width = w;
		f->bitmap.Height = h;
		f->bitmap.Pixels = bitmap;
		f->bitmap.Channels = 1;
		return f;
	}
	return 0;
}