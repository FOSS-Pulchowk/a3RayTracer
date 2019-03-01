#pragma once
#include "Common/Core.h"
#include "Utility/AssetData.h"
#include "Math/Math.h"
#include "Utility/Algorithm.h"

//
// DECLARATIONS
//

namespace a3 {

	a3::image CreateImageBuffer(i32 w, i32 h);
	void FillImageBuffer(a3::image* img, const rect& r, const v3&  color, f32 alpha = 1.0f);
	void FillImageBuffer(a3::image* img, const v3&  color, f32 alpha = 1.0f);
	void FillImageBuffer(a3::image* img, const rect& r, const v4&  color);
	void FillImageBuffer(a3::image* img, const v4&  color);
	void ClearImageBuffer(a3::image* img);
	// NOTE(Zero): Take alpha into consideration if `alpha` is true
	void CopyImageBuffer(a3::image* dest, a3::image* src, const rect& destRect, const rect& srcRect, b32 alpha = false);
	void CopyImageBuffer(a3::image* dest, a3::image* src, const rect& destRect, b32 alpha = false);

	void SetPixel(a3::image* img, i32 x, i32 y, u32 color);
	void SetRangedPixel(a3::image* img, i32 x, i32 y, u32 color);
	void SetPixelColor(a3::image* img, f32 x, f32 y, const v4&  color);
	void SetPixelColor(a3::image* img, f32 x, f32 y, const v3&  color, f32 alpha = 1.0f);
	u32 GetPixel(a3::image* img, i32 x, i32 y);
	v4  GetPixelColorNormal(a3::image*, i32 x, i32 y);
	u32 SamplePixel(a3::image* img, f32 u, f32 v);
	u32 SamplePixel(a3::image* img, v2 uv);
	v4  SamplePixelColor(a3::image* img, f32 u, f32 v);
	v4  SamplePixelColor(a3::image* img, v2 uv);

	void DrawLine(a3::image* img, v2 start, v2 end, const v3&  color);
	void DrawLineStrip(a3::image* img, v2* lines, i32 n, const v3&  color);
	void DrawPolygon(a3::image* img, v2* points, i32 n, const v3&  color);
	void DrawTriangle(a3::image* img, v2 p0, v2 p1, v2 p2, const v3&  color);
	// NOTE(Zero): Anti-Clockwise order works better, I guess
	void FillTriangle(a3::image* img, v2 p0, v2 p1, v2 p2, const v3&  fillColor);

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
		result.Pixels = a3New u8[w*h * 4];
		result.Width = w;
		result.Height = h;
		result.Channels = 4;
		return result;
	}

	void FillImageBuffer(a3::image * img, const rect& r, const v3& color, f32 alpha)
	{
		a3Assert(r.x >= 0 && r.y >= 0);
		a3Assert(r.x + r.w <= img->Width && r.y + r.h <= img->Height);
		u32 hexCol = a3Normalv3ToRGBA(color, a3NormalToChannel32(alpha));
		u32* pixel = (u32*)img->Pixels;
		for (i32 y = r.y; y < (r.y + r.h); ++y)
		{
			for (i32 x = r.x; x < (r.x + r.w); ++x)
			{
				pixel[x + y * img->Width] = hexCol;
			}
		}
	}

	void FillImageBuffer(a3::image* img, const v3& color, f32 alpha)
	{
		a3::FillImageBuffer(img, rect{ 0, 0, img->Width, img->Height }, color, alpha);
	}

	void FillImageBuffer(a3::image * img, const rect& r, const v4&  color)
	{
		a3::FillImageBuffer(img, r, color.rgb, color.a);
	}

	void FillImageBuffer(a3::image * img, const v4&  color)
	{
		a3::FillImageBuffer(img, color.rgb, color.a);
	}

	void ClearImageBuffer(a3::image * img)
	{
		a3::MemorySet(img->Pixels, 0, img->Width * img->Height * img->Channels);
	}

	void CopyImageBuffer(a3::image * dest, a3::image * src, const rect& destRect, const rect& srcRect, b32 alpha)
	{
		a3Assert(destRect.x >= 0 && destRect.y >= 0);
		a3Assert(destRect.x + destRect.w <= dest->Width && destRect.y + destRect.h <= dest->Height);
		a3Assert(srcRect.x >= 0 && srcRect.y >= 0);
		a3Assert(srcRect.x + srcRect.w <= src->Width && srcRect.y + srcRect.h <= src->Height);

		i32 mx = destRect.x + destRect.w;
		i32 my = destRect.y + destRect.h;
		u32* destPixels = (u32*)dest->Pixels;
		u32* srcPixels = (u32*)src->Pixels;

		if (alpha)
		{
			for (i32 y = destRect.y; y < my; ++y)
			{
				for (i32 x = destRect.x; x < mx; ++x)
				{
					i32 sx = (i32)((f32)(x - destRect.x) / (f32)destRect.w * srcRect.w) + srcRect.x;
					i32 sy = (i32)((f32)(y - destRect.y) / (f32)destRect.h * srcRect.h) + srcRect.y;
					const v4&  srcColor = a3MakeRGBAv4(srcPixels[sx + sy * src->Width]);
					v4 destColor = a3MakeRGBAv4(destPixels[x + y * dest->Width]);
					destColor = srcColor.a * srcColor + (1.0f - srcColor.a) * destColor;
					destPixels[x + y * dest->Width] = a3Normalv4ToRGBA(destColor);
				}
			}
		}
		else
		{
			for (i32 y = destRect.y; y < my; ++y)
			{
				for (i32 x = destRect.x; x < mx; ++x)
				{
					i32 sx = (i32)((f32)(x - destRect.x) / (f32)destRect.w * srcRect.w) + srcRect.x;
					i32 sy = (i32)((f32)(y - destRect.y) / (f32)destRect.h * srcRect.h) + srcRect.y;
					destPixels[x + y * dest->Width] = srcPixels[sx + sy * src->Width];
				}
			}
		}
	}

	void CopyImageBuffer(a3::image * dest, a3::image * src, const rect& destRect, b32 alpha)
	{
		a3::CopyImageBuffer(dest, src, destRect, rect{ 0, 0, src->Width, src->Height }, alpha);
	}

	void SetPixel(a3::image * img, i32 x, i32 y, u32 color)
	{
		a3Assert(x >= 0 && x < img->Width);
		a3Assert(y >= 0 && y < img->Height);
		((u32*)img->Pixels)[x + y * img->Width] = color;
	}

	void SetRangedPixel(a3::image * img, i32 x, i32 y, u32 color)
	{
		i32 index = x + y * img->Width;
		if(index < img->Width * img->Height)
			((u32*)img->Pixels)[index] = color;
	}

	void SetPixelColor(a3::image * img, f32 x, f32 y, const v4& color)
	{
		i32 px = (i32)x;
		i32 py = (i32)y;

		f32 bx = (x - (f32)px);
		f32 by = (y - (f32)py);
		bx = (bx > 0.5f) ? (1.0f - bx) : bx;
		by = (by > 0.5f) ? (1.0f - by) : by;
		f32 blend = 4.0f * bx * by;
		u32 hc = a3::GetPixel(img, px, py);
		a3::SetPixel(img, px, py, a3Normalv4ToRGBA(a3::BlendColor(color, a3MakeRGBAv4(hc), blend)));
	}

	void SetPixelColor(a3::image * img, f32 x, f32 y, const v3& color, f32 alpha)
	{
		v4 c;
		c.rgb = color;
		c.a = alpha;
		SetPixelColor(img, x, y, c);
	}

	u32 GetPixel(a3::image * img, i32 x, i32 y)
	{
		a3Assert(x >= 0 && x < img->Width);
		a3Assert(y >= 0 && y < img->Height);
		return ((u32*)img->Pixels)[x + y * img->Width];
	}

	v4 GetPixelColorNormal(a3::image * img, i32 x, i32 y)
	{
		return a3MakeRGBAv4(a3::GetPixel(img, x, y));
	}

	u32 SamplePixel(a3::image* img, f32 u, f32 v)
	{
		i32 x = (i32)(u * (f32)img->Width + 0.5f);
		i32 y = (i32)(v * (f32)img->Height + 0.5f);
		return a3::GetPixel(img, x, y);
	}

	u32 SamplePixel(a3::image* img, v2 uv)
	{
		return a3::SamplePixel(img, uv.u, uv.v);
	}

	v4 SamplePixelColor(a3::image* img, f32 u, f32 v)
	{
		return a3MakeRGBAv4(a3::SamplePixel(img, u, v));
	}

	v4 SamplePixelColor(a3::image* img, v2 uv)
	{
		return a3::SamplePixelColor(img, uv.u, uv.v);
	}

	void DrawLine(a3::image * img, v2 start, v2 end, const v3& color)
	{

		if (start.x < 0.0f) start.x = 0.0f;
		if (start.y < 0.0f) start.y = 0.0f;
		if (end.x < 0.0f) end.x = 0.0f;
		if (end.y < 0.0f) end.y = 0.0f;

		if (start.x > (f32)img->Width) start.x = (f32)img->Width;
		if (start.y > (f32)img->Height) start.y = (f32)img->Height;
		if (end.x > (f32)img->Width) end.x = (f32)img->Width;
		if (end.y > (f32)img->Height) end.x = (f32)img->Height;

		v2 d = end - start;
		i32 step;
		if (FAbsf(d.x) >= FAbsf(d.y))
			step = (i32)FAbsf(d.x);
		else
			step = (i32)FAbsf(d.y);
		d *= (1.0f / (f32)step);
		v2 s = start;
		for (i32 i = 1; i <= step; ++i)
		{
			a3::SetPixelColor(img, s.x, s.y, color);
			s += d;
		}
	}

	void DrawLineStrip(a3::image * img, v2 * lines, i32 n, const v3& color)
	{
		for (i32 i = 0; i < n - 1; ++i)
		{
			DrawLine(img, lines[i], lines[i + 1], color);
		}
	}

	void DrawPolygon(a3::image * img, v2 * points, i32 n, const v3& color)
	{
		for (i32 i = 0; i < n; ++i)
		{
			DrawLine(img, points[i], points[(i + 1) % n], color);
		}
	}

	void DrawTriangle(a3::image * img, v2 p0, v2 p1, v2 p2, const v3&  color)
	{
		DrawLine(img, p0, p1, color);
		DrawLine(img, p1, p2, color);
		DrawLine(img, p2, p0, color);
	}

	void FillTriangle(a3::image * img, v2 p0, v2 p1, v2 p2, const v3& fillColor)
	{
		auto processTopFlatPart = [&img, &fillColor](v2* p0, v2* p1, v2* p2)
		{
			if (p2->x > p1->x) a3::Swap(&p2->x, &p1->x);

			f32 m0 = (p0->x - p2->x) / (p0->y - p2->y);
			f32 m1 = (p0->x - p1->x) / (p0->y - p1->y);
			i32 vstep = 2 * (i32)(p1->y - p0->y + 0.5f);

			f32 y = p0->y;
			for (i32 i = 0; i < vstep; ++i)
			{
				f32 x0 = m0 * (y - p2->y) + p2->x;
				f32 x1 = m1 * (y - p1->y) + p1->x;
				// NOTE(Zero):
				// Here 1 is added to the step to balance the 0.5f reduction in x
				// 0.5f is reduced from x because we sample from the center of the pixel
				i32 hstep = 2 * (i32)(x1 - x0 + 0.5f);
				f32 x = x0 - 0.5f;
				for (i32 j = 0; j < hstep; ++j)
				{
					a3::SetPixelColor(img, x, y, fillColor);
					x += 0.5f;
				}
				y += 0.5f;
			}
		};

		auto processBottomFlatPart = [&img, &fillColor](v2* p0, v2* p1, v2* p2)
		{
			if (p2->x > p1->x) a3::Swap(&p2->x, &p1->x);

			f32 m0 = (p0->x - p2->x) / (p0->y - p2->y);
			f32 m1 = (p0->x - p1->x) / (p0->y - p1->y);
			i32 vstep = 2 * (i32)(p0->y - p1->y + 0.5f);

			f32 y = p1->y;
			for (i32 i = 0; i < vstep; ++i)
			{
				f32 x0 = m0 * (y - p2->y) + p2->x;
				f32 x1 = m1 * (y - p1->y) + p1->x;
				// NOTE(Zero):
				// Here 1 is added to the step to balance the 0.5f reduction in x
				// 0.5f is reduced from x because we sample from the center of the pixel
				i32 hstep = 2 * (i32)(x1 - x0 + 0.5f);
				f32 x = x0 - 0.5f;
				for (i32 j = 0; j < hstep; ++j)
				{
					a3::SetPixelColor(img, x, y, fillColor);
					x += 0.5f;
				}
				y += 0.5f;
			}
		};

		auto rasterizeTriangle = [&img, &processTopFlatPart, &processBottomFlatPart](v2* p0, v2* p1, v2* p2)
		{
			if (p0->y > p1->y) a3::Swap(&p0, &p1);
			if (p0->y > p2->y) a3::Swap(&p0, &p2);
			if (p1->y > p2->y) a3::Swap(&p1, &p2);

			if ((p1->y - p2->y) == 0.0f) // Top
			{
				processTopFlatPart(p0, p1, p2);
			}
			else if ((p1->y - p0->y) == 0.0f) // Bottom
			{
				processBottomFlatPart(p2, p1, p0);
			}
			else // Split into 2 flat parts
			{
				f32 blend = (p1->y - p0->y) / (p2->y - p0->y);
				v2 pm = (1.0f - blend) * *p0 + blend * *p2;
				processTopFlatPart(p0, p1, &pm); // top-flat
				processBottomFlatPart(p2, p1, &pm); // bottom-flat
			}
		};

		i32 numOfTriangles = 1;
		v2 fTriangles[6 * 3] = {};
		b32 rasterize[6] = { 1,1,1,1,1,1 };
		fTriangles[0] = p0;
		fTriangles[1] = p1;
		fTriangles[2] = p2;

		auto clipTriangleX = [&fTriangles, &numOfTriangles, &rasterize](b32(*check)(f32, f32), f32 assign)
		{
			i32 finalNumOfTriangles = numOfTriangles;
			for (i32 i = 0; i < numOfTriangles; ++i)
			{
				i32 nInsides = 0;
				v2* insides[] = { 0, 0, 0 };
				v2* outsides[] = { 0,0,0 };
				for (i32 n = 0; n < 3; ++n)
				{
					if (check(fTriangles[i * 3 + n].x, assign))
						insides[nInsides++] = &fTriangles[i * 3 + n];
					else
						outsides[n - nInsides] = &fTriangles[i * 3 + n];
				}
				switch (nInsides)
				{
				case 0:
				{
					rasterize[i] = false;
				} break;
				case 1:
				{
					f32 m0 = (insides[0]->y - outsides[0]->y) / (insides[0]->x - outsides[0]->x);
					f32 m1 = (insides[0]->y - outsides[1]->y) / (insides[0]->x - outsides[1]->x);
					v2 p0, p1, p2;
					p0.x = assign;
					p0.y = m0 * (assign - insides[0]->x) + insides[0]->y;
					p1 = *insides[0];
					p2.x = assign;
					p2.y = m1 * (assign - insides[0]->x) + insides[0]->y;
					fTriangles[i * 3 + 0] = p0;
					fTriangles[i * 3 + 1] = p1;
					fTriangles[i * 3 + 2] = p2;
				} break;
				case 2:
				{
					f32 m0 = (insides[0]->y - outsides[0]->y) / (insides[0]->x - outsides[0]->x);
					f32 m1 = (insides[1]->y - outsides[0]->y) / (insides[1]->x - outsides[0]->x);
					v2 p0, p1, p2, p3;
					p0.x = assign;
					p0.y = m0 * (assign - outsides[0]->x) + outsides[0]->y;
					p1 = *insides[0];
					p2 = *insides[1];
					p3.x = assign;
					p3.y = m1 * (assign - outsides[0]->x) + outsides[0]->y;
					fTriangles[i * 3 + 0] = p0;
					fTriangles[i * 3 + 1] = p1;
					fTriangles[i * 3 + 2] = p3;
					fTriangles[3 * finalNumOfTriangles + 0] = p1;
					fTriangles[3 * finalNumOfTriangles + 1] = p2;
					fTriangles[3 * finalNumOfTriangles + 2] = p3;
					finalNumOfTriangles++;
				} break;
				default:
				{
					a3Assert(nInsides == 3);
				}
				}
			}
			numOfTriangles = finalNumOfTriangles;
		};

		auto clipTriangleY = [&fTriangles, &numOfTriangles, &rasterize](b32(*check)(f32, f32), f32 assign)
		{
			i32 finalNumOfTriangles = numOfTriangles;
			for (i32 i = 0; i < numOfTriangles; ++i)
			{
				i32 nInsides = 0;
				v2* insides[] = { 0, 0, 0 };
				v2* outsides[] = { 0,0,0 };
				for (i32 n = 0; n < 3; ++n)
				{
					if (check(fTriangles[i * 3 + n].y, assign))
						insides[nInsides++] = &fTriangles[i * 3 + n];
					else
						outsides[n - nInsides] = &fTriangles[i * 3 + n];
				}
				switch (nInsides)
				{
				case 0:
				{
					rasterize[i] = false;
				} break;
				case 1:
				{
					f32 m0 = (insides[0]->x - outsides[0]->x) / (insides[0]->y - outsides[0]->y);
					f32 m1 = (insides[0]->x - outsides[1]->x) / (insides[0]->y - outsides[1]->y);
					v2 p0, p1, p2;
					p0.y = assign;
					p0.x = m0 * (assign - insides[0]->y) + insides[0]->x;
					p1 = *insides[0];
					p2.y = assign;
					p2.x = m1 * (assign - insides[0]->x) + insides[0]->x;
					fTriangles[i * 3 + 0] = p0;
					fTriangles[i * 3 + 1] = p1;
					fTriangles[i * 3 + 2] = p2;
				} break;
				case 2:
				{
					f32 m0 = (insides[0]->x - outsides[0]->x) / (insides[0]->y - outsides[0]->y);
					f32 m1 = (insides[1]->x - outsides[0]->x) / (insides[1]->y - outsides[0]->y);
					v2 p0, p1, p2, p3;
					p0.y = assign;
					p0.x = m0 * (assign - outsides[0]->y) + outsides[0]->x;
					p1 = *insides[0];
					p2 = *insides[1];
					p3.y = assign;
					p3.x = m1 * (assign - outsides[0]->y) + outsides[0]->x;
					fTriangles[i * 3 + 0] = p0;
					fTriangles[i * 3 + 1] = p1;
					fTriangles[i * 3 + 2] = p3;
					fTriangles[3 * finalNumOfTriangles + 0] = p1;
					fTriangles[3 * finalNumOfTriangles + 1] = p2;
					fTriangles[3 * finalNumOfTriangles + 2] = p3;
					finalNumOfTriangles++;
				} break;
				default:
				{
					a3Assert(nInsides == 3);
				}
				}
			}
			numOfTriangles = finalNumOfTriangles;
		};

		clipTriangleX([](f32 x, f32 p) -> b32 { return x >= p; }, 0.0f);
		clipTriangleY([](f32 y, f32 p) -> b32 { return y <= p; }, (f32)(img->Height));
		clipTriangleX([](f32 x, f32 p) -> b32 { return x <= p; }, (f32)(img->Width));
		clipTriangleY([](f32 y, f32 p) -> b32 { return y >= p; }, 0.0f);

		for (i32 i = 0; i < numOfTriangles; ++i)
		{
			if (rasterize[i])
			{
				i32 n = i * 3;
				rasterizeTriangle(&fTriangles[n + 0], &fTriangles[n + 1], &fTriangles[n + 2]);
			}
		}
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