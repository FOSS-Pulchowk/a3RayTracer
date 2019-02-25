#pragma once
#include "Common/Core.h"
#include "Utility/AssetData.h"
#include "Math/Math.h"

//
// DECLARATIONS
//

namespace a3 {

	a3::image CreateImageBuffer(i32 w, i32 h);
	void FillImageBuffer(a3::image* img, v3 color, rect r);
	void FillImageBuffer(a3::image* img, v3 color);
	void ClearImageBuffer(a3::image* img);
	// NOTE(Zero): Take alpha into consideration if `alpha` is true
	void CopyImageBuffer(a3::image* dest, a3::image* src, rect destRect, rect srcRect, b32 alpha = false);
	void CopyImageBuffer(a3::image* dest, a3::image* src, rect destRect, b32 alpha = false);
	typedef void(*RasterizeFontCallback)(void* userData, i32 w, i32 h, u8* buffer, i32 xOffset, i32 yOffset);
	void ResterizeFontsToBuffer(font_atlas_info* i, void* buffer, i32 length, f32 scale, void* drawBuffer, RasterizeFontCallback callback, void* userData);

}


//
// DEFINATIONS
//

#ifdef A3_IMPLEMENT_RASTERIZER2D
#include "Utility/STBLibs.h"
#include "Platform/Platform.h"
#include "Math/Color.h"

namespace a3 {

	// NOTE(Zero): Supporting only RGBA pixel format

	a3::image CreateImageBuffer(i32 w, i32 h)
	{
		a3::image result;
		result.Pixels = a3New u8[w*h*4];
		result.Width = w;
		result.Height = h;
		result.Channels = 4;
		return result;
	}

	void FillImageBuffer(a3::image * img, v3 color, rect r)
	{
		u32 hexCol = a3Normalv3ToRGBA(color, 0xffffffff);
		u32* pixel = (u32*)img->Pixels;
		for (i32 y = r.y; y < (r.y + r.h); ++y)
		{
			for (i32 x = r.x; x < (r.x + r.w); ++x)
			{
				pixel[x + y * img->Width] = hexCol;
			}
		}
	}

	void FillImageBuffer(a3::image* img, v3 color)
	{
		a3::FillImageBuffer(img, color, rect{ 0, 0, img->Width, img->Height });
	}

	void ClearImageBuffer(a3::image * img)
	{
		a3::MemorySet(img->Pixels, 0, img->Width * img->Height * img->Channels);
	}

	void CopyImageBuffer(a3::image * dest, a3::image * src, rect destRect, rect srcRect, b32 alpha)
	{
		i32 mx = destRect.x + destRect.w;
		i32 my = destRect.y + destRect.h;
		u32* destPixels = (u32*)dest->Pixels;
		u32* srcPixels = (u32*)src->Pixels;

		if (alpha)
		{

		}
		else
		{
			for (i32 y = destRect.y; y < my; ++y)
			{
				for (i32 x = destRect.x; x < mx; ++x)
				{
					i32 sx = (i32)((f32)x / (f32)dest->Width * srcRect.w) + srcRect.x;
					i32 sy = (i32)((f32)y / (f32)dest->Height * srcRect.h) + srcRect.y;
					destPixels[x + y * dest->Width] = srcPixels[sx + sy * src->Width];
				}
			}
		}
	}

	void CopyImageBuffer(a3::image * dest, a3::image * src, rect destRect, b32 alpha)
	{
		a3::CopyImageBuffer(dest, src, destRect, rect{ 0, 0, src->Width, src->Height }, alpha);
	}

	void a3::ResterizeFontsToBuffer(font_atlas_info* i, void * buffer, i32 length, f32 scale, void * drawBuffer, RasterizeFontCallback callback, void* userData)
	{
		stbtt_fontinfo info;
		stbtt_InitFont(&info, (u8*)buffer, stbtt_GetFontOffsetForIndex((u8*)buffer, 0));
		i32 w, h;
		a3_CalculateFontBitmapMaxDimension(info, scale, &w, &h);
		f32 pscale = stbtt_ScaleForPixelHeight(&info, scale);
		i->ScalingFactor = pscale;
		i->Info = info;
		i->HeightInPixels = scale;
		i32 atlasWidth = w * A3MAXLOADGLYPHX * A3MAXLOADGLYPHY;
		i32 atlasHeight = h;

		i32 widthAdvance = 0;
		u8* flippedYBuffer = a3New u8[atlasWidth * atlasHeight];
		for (i32 blockY = 0; blockY < A3MAXLOADGLYPHY; ++blockY)
		{
			for (i32 blockX = 0; blockX < A3MAXLOADGLYPHX; ++blockX)
			{
				i32 index = blockY * A3MAXLOADGLYPHX + blockX;
				a3::character* c = &i->Characters[index];
				c->GlyphIndex = stbtt_FindGlyphIndex(&i->Info, index);
				i32 leftSizeBearing; // NOTE(Zero): Ignored
				i32 advance;
				stbtt_GetGlyphHMetrics(&i->Info, c->GlyphIndex, &advance, &leftSizeBearing);
				c->Advance = (f32)advance * pscale;
				i32 bw = w;
				i32 bh = h;
				if (stbtt_IsGlyphEmpty(&i->Info, c->GlyphIndex))
				{
					c->HasBitmap = false;
				}
				else
				{
					c->HasBitmap = true;
					i32 x0, x1, y0, y1;
					stbtt_GetGlyphBitmapBox(&i->Info, c->GlyphIndex, pscale, pscale, &x0, &y0, &x1, &y1);
					i32 bw = x1 - x0; i32 bh = y1 - y0;

					// NOTE(Zero): Here stride is equal to width because OpenGL wants packed pixels
					i32 stride = bw;
					stbtt_MakeGlyphBitmap(&i->Info, (u8*)drawBuffer, bw, bh, stride, pscale, pscale, c->GlyphIndex);
					c->OffsetX = x0;
					c->OffsetY = -y1;
					c->Width = w;
					c->Height = h;
					c->NormalX0 = (f32)widthAdvance / (f32)atlasWidth;
					// NOTE (Zero):
					// Here 1 is reduced to no dispaly a single column of the texture
					// This does not effect the final dislay because we actually increase
					// the max width for each font bitmap in `a3_CalculateFontBitmapMaxDimension` 
					c->NormalX1 = (f32)(widthAdvance + w - 1) / (f32)atlasWidth;
					c->NormalY0 = 0.0f;
					c->NormalY1 = 1.0f;
					a3::ReverseRectCopy(flippedYBuffer, drawBuffer, bw, bh);
					callback(userData, bw, bh, flippedYBuffer, widthAdvance, 0);
					widthAdvance += w;
				}
			}
		}
		a3Delete[] flippedYBuffer;
	}


}

#endif