#include "Common/Core.h"
#include "Platform/Platform.h"
#include "Math/Math.h"
#include "Graphics/Image.h"

a3::image CreateImageBuffer(i32 w, i32 h, i32 n)
{
	a3::image result;
	result.Width = w;
	result.Height = h;
	result.Channels = n;
	result.Pixels = new u8[w*h*n];
	return result;
}

void PutPixel(a3::image& img, i32 x, i32 y, u32 color)
{
	if (x >= img.Width || y >= img.Height)
	{
		a3TriggerBreakPoint();
		return;
	}
	((u32*)img.Pixels)[x+y*img.Width] = color;
}

void DrawRect(a3::image& img, i32 x, i32 y, i32 w, i32 h, u32 color)
{
	for (i32 yy = y; yy < h; ++yy)
	{
		for (i32 xx = x; xx < w; ++xx)
		{
			PutPixel(img, xx, yy, color);
		}
	}
}

int main()
{
	a3::image imgBuffer = CreateImageBuffer(100, 100, 4);
	DrawRect(imgBuffer, 0, 0, 100, 100, 0xffff0000); // AARRGGBB
	a3::WritePNGImage("test_img.png", imgBuffer.Width, imgBuffer.Height, imgBuffer.Channels, 4, imgBuffer.Pixels);
	return 0;
}
