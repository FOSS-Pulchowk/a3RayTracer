#pragma once
#include "Common/Core.h"

// NOTE(Zero):
/*
	Color are based on the color names by w3schools + there are some additionals
	Link:: https://www.w3schools.com/colors/colors_names.asp
*/


// NOTE(Zero): These macros take color in Big Endian order or in Netork Order
#define a3GetRChannel(hex) ((hex) >> 16)
#define a3GetGChannel(hex) (((hex) & 0x00ff00) >> 8)
#define a3GetBChannel(hex) (((hex) & 0x0000ff))

#define a3RChannelToNormal(hex) ((f32)(a3GetRChannel(hex)) / 255.0f)
#define a3GChannelToNormal(hex) ((f32)(a3GetGChannel(hex)) / 255.0f)
#define a3BChannelToNormal(hex) ((f32)(a3GetBChannel(hex)) / 255.0f)

#define a3MakeRGBv3(rgb) v3{ a3RChannelToNormal(rgb), a3GChannelToNormal(rgb), a3BChannelToNormal(rgb) }

#define a3NormalToChannel8(v) ((u8)((v) * 255.0f))
#define a3NormalToChannel a3NormalToChannel8
#define a3NormalToChannel32(v) ((u32)((v) * 255.0f))

// NOTE(Zero): This macros return color values in Little Endian Order
#define a3Normalv3ToRGB(c) ((a3NormalToChannel32(c.r) << 0) | (a3NormalToChannel32(c.g) << 8) | (a3NormalToChannel32(c.b) << 16))
#define a3Normalv4ToRGBA(c) ((a3NormalToChannel32(c.a) << 24) | a3Normalv3ToRGB(c.rgb))
#define a3Normalv3ToRGBA(c, alpha) ((alpha << 24) | a3Normalv3ToRGB(c))

namespace a3 { namespace color {

	// NOTE(Zero): These colors are in Network-Byte-Order or are in Big Endian

	constexpr v3 AliceBlue = a3MakeRGBv3(0xF0F8FF);
	constexpr v3 AntiqueWhite = a3MakeRGBv3(0xFAEBD7);
	constexpr v3 Aqua = a3MakeRGBv3(0x00FFFF);
	constexpr v3 Aquamarine = a3MakeRGBv3(0x7FFFD4);
	constexpr v3 Azure = a3MakeRGBv3(0xF0FFFF);
	constexpr v3 Beige = a3MakeRGBv3(0xF5F5DC);
	constexpr v3 Bisque = a3MakeRGBv3(0xFFE4C4);
	constexpr v3 Black = a3MakeRGBv3(0x000000);
	constexpr v3 BlanchedAlmond = a3MakeRGBv3(0xFFEBCD);
	constexpr v3 Blue = a3MakeRGBv3(0x0000FF);
	constexpr v3 BlueViolet = a3MakeRGBv3(0x8A2BE2);
	constexpr v3 Blurple = a3MakeRGBv3(0x7289DA);
	constexpr v3 Brown = a3MakeRGBv3(0xA52A2A);
	constexpr v3 BurlyWood = a3MakeRGBv3(0xDEB887);
	constexpr v3 CadetBlue = a3MakeRGBv3(0x5F9EA0);
	constexpr v3 Chartreuse = a3MakeRGBv3(0x7FFF00);
	constexpr v3 Chocolate = a3MakeRGBv3(0xD2691E);
	constexpr v3 Coral = a3MakeRGBv3(0xFF7F50);
	constexpr v3 CornflowerBlue = a3MakeRGBv3(0x6495ED);
	constexpr v3 Cornsilk = a3MakeRGBv3(0xFFF8DC);
	constexpr v3 Crimson = a3MakeRGBv3(0xDC143C);
	constexpr v3 Cyan = a3MakeRGBv3(0x00FFFF);
	constexpr v3 DarkBlue = a3MakeRGBv3(0x00008B);
	constexpr v3 DarkCyan = a3MakeRGBv3(0x008B8B);
	constexpr v3 DarkGoldenRod = a3MakeRGBv3(0xB8860B);
	constexpr v3 DarkGray = a3MakeRGBv3(0xA9A9A9);
	constexpr v3 DarkGreen = a3MakeRGBv3(0x006400);
	constexpr v3 DarkGrey = a3MakeRGBv3(0xA9A9A9);
	constexpr v3 DarkNotBlack = a3MakeRGBv3(0x2C2F33);
	constexpr v3 DarkKhaki = a3MakeRGBv3(0xBDB76B);
	constexpr v3 DarkMagenta = a3MakeRGBv3(0x8B008B);
	constexpr v3 DarkOliveGreen = a3MakeRGBv3(0x556B2F);
	constexpr v3 DarkOrange = a3MakeRGBv3(0xFF8C00);
	constexpr v3 DarkOrchid = a3MakeRGBv3(0x9932CC);
	constexpr v3 DarkRed = a3MakeRGBv3(0x8B0000);
	constexpr v3 DarkSalmon = a3MakeRGBv3(0xE9967A);
	constexpr v3 DarkSeaGreen = a3MakeRGBv3(0x8FBC8F);
	constexpr v3 DarkSlateBlue = a3MakeRGBv3(0x483D8B);
	constexpr v3 DarkSlateGray = a3MakeRGBv3(0x2F4F4F);
	constexpr v3 DarkSlateGrey = a3MakeRGBv3(0x2F4F4F);
	constexpr v3 DarkTurquoise = a3MakeRGBv3(0x00CED1);
	constexpr v3 DarkViolet = a3MakeRGBv3(0x9400D3);
	constexpr v3 DeepPink = a3MakeRGBv3(0xFF1493);
	constexpr v3 DeepSkyBlue = a3MakeRGBv3(0x00BFFF);
	constexpr v3 DimGray = a3MakeRGBv3(0x696969);
	constexpr v3 DimGrey = a3MakeRGBv3(0x696969);
	constexpr v3 DodgerBlue = a3MakeRGBv3(0x1E90FF);
	constexpr v3 FireBrick = a3MakeRGBv3(0xB22222);
	constexpr v3 FloralWhite = a3MakeRGBv3(0xFFFAF0);
	constexpr v3 ForestGreen = a3MakeRGBv3(0x228B22);
	constexpr v3 Fuchsia = a3MakeRGBv3(0xFF00FF);
	constexpr v3 Gainsboro = a3MakeRGBv3(0xDCDCDC);
	constexpr v3 GhostWhite = a3MakeRGBv3(0xF8F8FF);
	constexpr v3 Gold = a3MakeRGBv3(0xFFD700);
	constexpr v3 GoldenRod = a3MakeRGBv3(0xDAA520);
	constexpr v3 Gray = a3MakeRGBv3(0x808080);
	constexpr v3 Green = a3MakeRGBv3(0x008000);
	constexpr v3 GreenYellow = a3MakeRGBv3(0xADFF2F);
	constexpr v3 Grey = a3MakeRGBv3(0x808080);
	constexpr v3 Greyple = a3MakeRGBv3(0x99AAB5);
	constexpr v3 HoneyDew = a3MakeRGBv3(0xF0FFF0);
	constexpr v3 HotPink = a3MakeRGBv3(0xFF69B4);
	constexpr v3 IndianRed = a3MakeRGBv3(0xCD5C5C);
	constexpr v3 Indigo = a3MakeRGBv3(0x4B0082);
	constexpr v3 Ivory = a3MakeRGBv3(0xFFFFF0);
	constexpr v3 Khaki = a3MakeRGBv3(0xF0E68C);
	constexpr v3 Lavender = a3MakeRGBv3(0xE6E6FA);
	constexpr v3 LavenderBlush = a3MakeRGBv3(0xFFF0F5);
	constexpr v3 LawnGreen = a3MakeRGBv3(0x7CFC00);
	constexpr v3 LemonChiffon = a3MakeRGBv3(0xFFFACD);
	constexpr v3 LightBlue = a3MakeRGBv3(0xADD8E6);
	constexpr v3 LightCoral = a3MakeRGBv3(0xF08080);
	constexpr v3 LightCyan = a3MakeRGBv3(0xE0FFFF);
	constexpr v3 LightGoldenRodYellow = a3MakeRGBv3(0xFAFAD2);
	constexpr v3 LightGray = a3MakeRGBv3(0xD3D3D3);
	constexpr v3 LightGreen = a3MakeRGBv3(0x90EE90);
	constexpr v3 LightGrey = a3MakeRGBv3(0xD3D3D3);
	constexpr v3 LightPink = a3MakeRGBv3(0xFFB6C1);
	constexpr v3 LightSalmon = a3MakeRGBv3(0xFFA07A);
	constexpr v3 LightSeaGreen = a3MakeRGBv3(0x20B2AA);
	constexpr v3 LightSkyBlue = a3MakeRGBv3(0x87CEFA);
	constexpr v3 LightSlateGray = a3MakeRGBv3(0x778899);
	constexpr v3 LightSlateGrey = a3MakeRGBv3(0x778899);
	constexpr v3 LightSteelBlue = a3MakeRGBv3(0xB0C4DE);
	constexpr v3 LightYellow = a3MakeRGBv3(0xFFFFE0);
	constexpr v3 Lime = a3MakeRGBv3(0x00FF00);
	constexpr v3 LimeGreen = a3MakeRGBv3(0x32CD32);
	constexpr v3 Linen = a3MakeRGBv3(0xFAF0E6);
	constexpr v3 Magenta = a3MakeRGBv3(0xFF00FF);
	constexpr v3 Maroon = a3MakeRGBv3(0x800000);
	constexpr v3 MediumAquaMarine = a3MakeRGBv3(0x66CDAA);
	constexpr v3 MediumBlue = a3MakeRGBv3(0x0000CD);
	constexpr v3 MediumOrchid = a3MakeRGBv3(0xBA55D3);
	constexpr v3 MediumPurple = a3MakeRGBv3(0x9370DB);
	constexpr v3 MediumSeaGreen = a3MakeRGBv3(0x3CB371);
	constexpr v3 MediumSlateBlue = a3MakeRGBv3(0x7B68EE);
	constexpr v3 MediumSpringGreen = a3MakeRGBv3(0x00FA9A);
	constexpr v3 MediumTurquoise = a3MakeRGBv3(0x48D1CC);
	constexpr v3 MediumVioletRed = a3MakeRGBv3(0xC71585);
	constexpr v3 MidnightBlue = a3MakeRGBv3(0x191970);
	constexpr v3 MintCream = a3MakeRGBv3(0xF5FFFA);
	constexpr v3 MistyRose = a3MakeRGBv3(0xFFE4E1);
	constexpr v3 Moccasin = a3MakeRGBv3(0xFFE4B5);
	constexpr v3 NavajoWhite = a3MakeRGBv3(0xFFDEAD);
	constexpr v3 Navy = a3MakeRGBv3(0x000080);
	constexpr v3 NotQuiteBlack = a3MakeRGBv3(0x23272A);
	constexpr v3 OldLace = a3MakeRGBv3(0xFDF5E6);
	constexpr v3 Olive = a3MakeRGBv3(0x808000);
	constexpr v3 OliveDrab = a3MakeRGBv3(0x6B8E23);
	constexpr v3 Orange = a3MakeRGBv3(0xFFA500);
	constexpr v3 OrangeRed = a3MakeRGBv3(0xFF4500);
	constexpr v3 Orchid = a3MakeRGBv3(0xDA70D6);
	constexpr v3 PaleGoldenRod = a3MakeRGBv3(0xEEE8AA);
	constexpr v3 PaleGreen = a3MakeRGBv3(0x98FB98);
	constexpr v3 PaleTurquoise = a3MakeRGBv3(0xAFEEEE);
	constexpr v3 PaleVioletRed = a3MakeRGBv3(0xDB7093);
	constexpr v3 PapayaWhip = a3MakeRGBv3(0xFFEFD5);
	constexpr v3 PeachPuff = a3MakeRGBv3(0xFFDAB9);
	constexpr v3 Peru = a3MakeRGBv3(0xCD853F);
	constexpr v3 Pink = a3MakeRGBv3(0xFFC0CB);
	constexpr v3 Plum = a3MakeRGBv3(0xDDA0DD);
	constexpr v3 PowderBlue = a3MakeRGBv3(0xB0E0E6);
	constexpr v3 Purple = a3MakeRGBv3(0x800080);
	constexpr v3 RebeccaPurple = a3MakeRGBv3(0x663399);
	constexpr v3 Red = a3MakeRGBv3(0xFF0000);
	constexpr v3 RosyBrown = a3MakeRGBv3(0xBC8F8F);
	constexpr v3 RoyalBlue = a3MakeRGBv3(0x4169E1);
	constexpr v3 SaddleBrown = a3MakeRGBv3(0x8B4513);
	constexpr v3 Salmon = a3MakeRGBv3(0xFA8072);
	constexpr v3 SandyBrown = a3MakeRGBv3(0xF4A460);
	constexpr v3 SeaGreen = a3MakeRGBv3(0x2E8B57);
	constexpr v3 SeaShell = a3MakeRGBv3(0xFFF5EE);
	constexpr v3 Sienna = a3MakeRGBv3(0xA0522D);
	constexpr v3 Silver = a3MakeRGBv3(0xC0C0C0);
	constexpr v3 SkyBlue = a3MakeRGBv3(0x87CEEB);
	constexpr v3 SlateBlue = a3MakeRGBv3(0x6A5ACD);
	constexpr v3 SlateGray = a3MakeRGBv3(0x708090);
	constexpr v3 SlateGrey = a3MakeRGBv3(0x708090);
	constexpr v3 Snow = a3MakeRGBv3(0xFFFAFA);
	constexpr v3 SpringGreen = a3MakeRGBv3(0x00FF7F);
	constexpr v3 SteelBlue = a3MakeRGBv3(0x4682B4);
	constexpr v3 Tan = a3MakeRGBv3(0xD2B48C);
	constexpr v3 Teal = a3MakeRGBv3(0x008080);
	constexpr v3 Thistle = a3MakeRGBv3(0xD8BFD8);
	constexpr v3 Tomato = a3MakeRGBv3(0xFF6347);
	constexpr v3 Turquoise = a3MakeRGBv3(0x40E0D0);
	constexpr v3 Violet = a3MakeRGBv3(0xEE82EE);
	constexpr v3 Wheat = a3MakeRGBv3(0xF5DEB3);
	constexpr v3 White = a3MakeRGBv3(0xFFFFFF);
	constexpr v3 WhiteSmoke = a3MakeRGBv3(0xF5F5F5);
	constexpr v3 Yellow = a3MakeRGBv3(0xFFFF00);
	constexpr v3 YellowGreen = a3MakeRGBv3(0x9ACD32);

} }

//
// DECLARATION
//

namespace a3 {

	inline v3 MixColor(v3 c1, v3 c2);
	inline v3 GrayScaleColor(v3 c);

}


//
// IMPLEMENTATION
//

inline v3 a3::MixColor(v3 c1, v3 c2)
{
	v3 result;
	result.r = c1.r * c2.r;
	result.g = c1.g * c2.g;
	result.b = c1.b * c2.b;
	return result;
}

inline v3 a3::GrayScaleColor(v3 c)
{
	v3 result;
	result.r = c.r * 0.2627f;
	result.g = c.g * 0.6780f;
	result.b = c.b * 0.0593f;
	return result;
}