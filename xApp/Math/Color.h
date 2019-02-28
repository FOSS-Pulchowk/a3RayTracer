#pragma once
#include "Common/Core.h"

// NOTE(Zero):
/*
	Color are based on the color names by w3schools + there are some additionals
	Link:: https://www.w3schools.com/colors/colors_names.asp
*/


// NOTE(Zero): This macros return color values in Little Endian Order
#define a3GetRChannel(hex) (((hex) & 0x000000ff) >> 0)
#define a3GetGChannel(hex) (((hex) & 0x0000ff00) >> 8)
#define a3GetBChannel(hex) (((hex) & 0x00ff0000) >> 16)
#define a3GetAChannel(hex) (((hex) & 0xff000000) >> 24)

#define a3RChannelToNormal(hex) ((f32)(a3GetRChannel(hex)) / 255.0f)
#define a3GChannelToNormal(hex) ((f32)(a3GetGChannel(hex)) / 255.0f)
#define a3BChannelToNormal(hex) ((f32)(a3GetBChannel(hex)) / 255.0f)
#define a3AChannelToNormal(hex) ((f32)(a3GetAChannel(hex)) / 255.0f)

#define a3MakeRGBv3(rgb) v3{ a3RChannelToNormal(rgb), a3GChannelToNormal(rgb), a3BChannelToNormal(rgb) }
#define a3MakeRGBAv4(rgba) v4{ a3RChannelToNormal(rgba), a3GChannelToNormal(rgba), a3BChannelToNormal(rgba), a3AChannelToNormal(rgba) }

#define a3NormalToChannel8(v) ((u8)((v) * 255.0f))
#define a3NormalToChannel a3NormalToChannel8
#define a3NormalToChannel32(v) ((u32)((v) * 255.0f))

#define a3Normalv3ToRGB(c) ((a3NormalToChannel32(c.r) << 0) | (a3NormalToChannel32(c.g) << 8) | (a3NormalToChannel32(c.b) << 16))
#define a3Normalv4ToRGBA(c) ((a3NormalToChannel32(c.a) << 24) | a3Normalv3ToRGB(c.rgb))
#define a3Normalv3ToRGBA(c, alpha) (((alpha) << 24) | (a3Normalv3ToRGB(c)))

namespace a3 { namespace color {

	// NOTE(Zero): These colors are in Network-Byte-Order or are in Big Endian

	constexpr v3 AliceBlue = a3MakeRGBv3(a3SwapEndian32(0xF0F8FF00));
	constexpr v3 AntiqueWhite = a3MakeRGBv3(a3SwapEndian32(0xFAEBD700));
	constexpr v3 Aqua = a3MakeRGBv3(a3SwapEndian32(0x00FFFF00));
	constexpr v3 Aquamarine = a3MakeRGBv3(a3SwapEndian32(0x7FFFD400));
	constexpr v3 Azure = a3MakeRGBv3(a3SwapEndian32(0xF0FFFF00));
	constexpr v3 Beige = a3MakeRGBv3(a3SwapEndian32(0xF5F5DC00));
	constexpr v3 Bisque = a3MakeRGBv3(a3SwapEndian32(0xFFE4C400));
	constexpr v3 Black = a3MakeRGBv3(a3SwapEndian32(0x00000000));
	constexpr v3 BlanchedAlmond = a3MakeRGBv3(a3SwapEndian32(0xFFEBCD00));
	constexpr v3 Blue = a3MakeRGBv3(a3SwapEndian32(0x0000FF00));
	constexpr v3 BlueViolet = a3MakeRGBv3(a3SwapEndian32(0x8A2BE200));
	constexpr v3 Blurple = a3MakeRGBv3(a3SwapEndian32(0x7289DA00));
	constexpr v3 Brown = a3MakeRGBv3(a3SwapEndian32(0xA52A2A00));
	constexpr v3 BurlyWood = a3MakeRGBv3(a3SwapEndian32(0xDEB88700));
	constexpr v3 CadetBlue = a3MakeRGBv3(a3SwapEndian32(0x5F9EA000));
	constexpr v3 Chartreuse = a3MakeRGBv3(a3SwapEndian32(0x7FFF0000));
	constexpr v3 Chocolate = a3MakeRGBv3(a3SwapEndian32(0xD2691E00));
	constexpr v3 Coral = a3MakeRGBv3(a3SwapEndian32(0xFF7F5000));
	constexpr v3 CornflowerBlue = a3MakeRGBv3(a3SwapEndian32(0x6495ED00));
	constexpr v3 Cornsilk = a3MakeRGBv3(a3SwapEndian32(0xFFF8DC00));
	constexpr v3 Crimson = a3MakeRGBv3(a3SwapEndian32(0xDC143C00));
	constexpr v3 Cyan = a3MakeRGBv3(a3SwapEndian32(0x00FFFF00));
	constexpr v3 DarkBlue = a3MakeRGBv3(a3SwapEndian32(0x00008B00));
	constexpr v3 DarkCyan = a3MakeRGBv3(a3SwapEndian32(0x008B8B00));
	constexpr v3 DarkGoldenRod = a3MakeRGBv3(a3SwapEndian32(0xB8860B00));
	constexpr v3 DarkGray = a3MakeRGBv3(a3SwapEndian32(0xA9A9A900));
	constexpr v3 DarkGreen = a3MakeRGBv3(a3SwapEndian32(0x00640000));
	constexpr v3 DarkGrey = a3MakeRGBv3(a3SwapEndian32(0xA9A9A900));
	constexpr v3 DarkNotBlack = a3MakeRGBv3(a3SwapEndian32(0x2C2F3300));
	constexpr v3 DarkKhaki = a3MakeRGBv3(a3SwapEndian32(0xBDB76B00));
	constexpr v3 DarkMagenta = a3MakeRGBv3(a3SwapEndian32(0x8B008B00));
	constexpr v3 DarkOliveGreen = a3MakeRGBv3(a3SwapEndian32(0x556B2F00));
	constexpr v3 DarkOrange = a3MakeRGBv3(a3SwapEndian32(0xFF8C0000));
	constexpr v3 DarkOrchid = a3MakeRGBv3(a3SwapEndian32(0x9932CC00));
	constexpr v3 DarkRed = a3MakeRGBv3(a3SwapEndian32(0x8B000000));
	constexpr v3 DarkSalmon = a3MakeRGBv3(a3SwapEndian32(0xE9967A00));
	constexpr v3 DarkSeaGreen = a3MakeRGBv3(a3SwapEndian32(0x8FBC8F00));
	constexpr v3 DarkSlateBlue = a3MakeRGBv3(a3SwapEndian32(0x483D8B00));
	constexpr v3 DarkSlateGray = a3MakeRGBv3(a3SwapEndian32(0x2F4F4F00));
	constexpr v3 DarkSlateGrey = a3MakeRGBv3(a3SwapEndian32(0x2F4F4F00));
	constexpr v3 DarkTurquoise = a3MakeRGBv3(a3SwapEndian32(0x00CED100));
	constexpr v3 DarkViolet = a3MakeRGBv3(a3SwapEndian32(0x9400D300));
	constexpr v3 DeepPink = a3MakeRGBv3(a3SwapEndian32(0xFF149300));
	constexpr v3 DeepSkyBlue = a3MakeRGBv3(a3SwapEndian32(0x00BFFF00));
	constexpr v3 DimGray = a3MakeRGBv3(a3SwapEndian32(0x69696900));
	constexpr v3 DimGrey = a3MakeRGBv3(a3SwapEndian32(0x69696900));
	constexpr v3 DodgerBlue = a3MakeRGBv3(a3SwapEndian32(0x1E90FF00));
	constexpr v3 FireBrick = a3MakeRGBv3(a3SwapEndian32(0xB2222200));
	constexpr v3 FloralWhite = a3MakeRGBv3(a3SwapEndian32(0xFFFAF000));
	constexpr v3 ForestGreen = a3MakeRGBv3(a3SwapEndian32(0x228B2200));
	constexpr v3 Fuchsia = a3MakeRGBv3(a3SwapEndian32(0xFF00FF00));
	constexpr v3 Gainsboro = a3MakeRGBv3(a3SwapEndian32(0xDCDCDC00));
	constexpr v3 GhostWhite = a3MakeRGBv3(a3SwapEndian32(0xF8F8FF00));
	constexpr v3 Gold = a3MakeRGBv3(a3SwapEndian32(0xFFD70000));
	constexpr v3 GoldenRod = a3MakeRGBv3(a3SwapEndian32(0xDAA52000));
	constexpr v3 Gray = a3MakeRGBv3(a3SwapEndian32(0x80808000));
	constexpr v3 Green = a3MakeRGBv3(a3SwapEndian32(0x00800000));
	constexpr v3 GreenYellow = a3MakeRGBv3(a3SwapEndian32(0xADFF2F00));
	constexpr v3 Grey = a3MakeRGBv3(a3SwapEndian32(0x80808000));
	constexpr v3 Greyple = a3MakeRGBv3(a3SwapEndian32(0x99AAB500));
	constexpr v3 HoneyDew = a3MakeRGBv3(a3SwapEndian32(0xF0FFF000));
	constexpr v3 HotPink = a3MakeRGBv3(a3SwapEndian32(0xFF69B400));
	constexpr v3 IndianRed = a3MakeRGBv3(a3SwapEndian32(0xCD5C5C00));
	constexpr v3 Indigo = a3MakeRGBv3(a3SwapEndian32(0x4B008200));
	constexpr v3 Ivory = a3MakeRGBv3(a3SwapEndian32(0xFFFFF000));
	constexpr v3 Khaki = a3MakeRGBv3(a3SwapEndian32(0xF0E68C00));
	constexpr v3 Lavender = a3MakeRGBv3(a3SwapEndian32(0xE6E6FA00));
	constexpr v3 LavenderBlush = a3MakeRGBv3(a3SwapEndian32(0xFFF0F500));
	constexpr v3 LawnGreen = a3MakeRGBv3(a3SwapEndian32(0x7CFC0000));
	constexpr v3 LemonChiffon = a3MakeRGBv3(a3SwapEndian32(0xFFFACD00));
	constexpr v3 LightBlue = a3MakeRGBv3(a3SwapEndian32(0xADD8E600));
	constexpr v3 LightCoral = a3MakeRGBv3(a3SwapEndian32(0xF0808000));
	constexpr v3 LightCyan = a3MakeRGBv3(a3SwapEndian32(0xE0FFFF00));
	constexpr v3 LightGoldenRodYellow = a3MakeRGBv3(a3SwapEndian32(0xFAFAD200));
	constexpr v3 LightGray = a3MakeRGBv3(a3SwapEndian32(0xD3D3D300));
	constexpr v3 LightGreen = a3MakeRGBv3(a3SwapEndian32(0x90EE9000));
	constexpr v3 LightGrey = a3MakeRGBv3(a3SwapEndian32(0xD3D3D300));
	constexpr v3 LightPink = a3MakeRGBv3(a3SwapEndian32(0xFFB6C100));
	constexpr v3 LightSalmon = a3MakeRGBv3(a3SwapEndian32(0xFFA07A00));
	constexpr v3 LightSeaGreen = a3MakeRGBv3(a3SwapEndian32(0x20B2AA00));
	constexpr v3 LightSkyBlue = a3MakeRGBv3(a3SwapEndian32(0x87CEFA00));
	constexpr v3 LightSlateGray = a3MakeRGBv3(a3SwapEndian32(0x77889900));
	constexpr v3 LightSlateGrey = a3MakeRGBv3(a3SwapEndian32(0x77889900));
	constexpr v3 LightSteelBlue = a3MakeRGBv3(a3SwapEndian32(0xB0C4DE00));
	constexpr v3 LightYellow = a3MakeRGBv3(a3SwapEndian32(0xFFFFE000));
	constexpr v3 Lime = a3MakeRGBv3(a3SwapEndian32(0x00FF0000));
	constexpr v3 LimeGreen = a3MakeRGBv3(a3SwapEndian32(0x32CD3200));
	constexpr v3 Linen = a3MakeRGBv3(a3SwapEndian32(0xFAF0E600));
	constexpr v3 Magenta = a3MakeRGBv3(a3SwapEndian32(0xFF00FF00));
	constexpr v3 Maroon = a3MakeRGBv3(a3SwapEndian32(0x80000000));
	constexpr v3 MediumAquaMarine = a3MakeRGBv3(a3SwapEndian32(0x66CDAA00));
	constexpr v3 MediumBlue = a3MakeRGBv3(a3SwapEndian32(0x0000CD00));
	constexpr v3 MediumOrchid = a3MakeRGBv3(a3SwapEndian32(0xBA55D300));
	constexpr v3 MediumPurple = a3MakeRGBv3(a3SwapEndian32(0x9370DB00));
	constexpr v3 MediumSeaGreen = a3MakeRGBv3(a3SwapEndian32(0x3CB37100));
	constexpr v3 MediumSlateBlue = a3MakeRGBv3(a3SwapEndian32(0x7B68EE00));
	constexpr v3 MediumSpringGreen = a3MakeRGBv3(a3SwapEndian32(0x00FA9A00));
	constexpr v3 MediumTurquoise = a3MakeRGBv3(a3SwapEndian32(0x48D1CC00));
	constexpr v3 MediumVioletRed = a3MakeRGBv3(a3SwapEndian32(0xC7158500));
	constexpr v3 MidnightBlue = a3MakeRGBv3(a3SwapEndian32(0x19197000));
	constexpr v3 MintCream = a3MakeRGBv3(a3SwapEndian32(0xF5FFFA00));
	constexpr v3 MistyRose = a3MakeRGBv3(a3SwapEndian32(0xFFE4E100));
	constexpr v3 Moccasin = a3MakeRGBv3(a3SwapEndian32(0xFFE4B500));
	constexpr v3 NavajoWhite = a3MakeRGBv3(a3SwapEndian32(0xFFDEAD00));
	constexpr v3 Navy = a3MakeRGBv3(a3SwapEndian32(0x00008000));
	constexpr v3 NotQuiteBlack = a3MakeRGBv3(a3SwapEndian32(0x23272A00));
	constexpr v3 OldLace = a3MakeRGBv3(a3SwapEndian32(0xFDF5E600));
	constexpr v3 Olive = a3MakeRGBv3(a3SwapEndian32(0x80800000));
	constexpr v3 OliveDrab = a3MakeRGBv3(a3SwapEndian32(0x6B8E2300));
	constexpr v3 Orange = a3MakeRGBv3(a3SwapEndian32(0xFFA50000));
	constexpr v3 OrangeRed = a3MakeRGBv3(a3SwapEndian32(0xFF450000));
	constexpr v3 Orchid = a3MakeRGBv3(a3SwapEndian32(0xDA70D600));
	constexpr v3 PaleGoldenRod = a3MakeRGBv3(a3SwapEndian32(0xEEE8AA00));
	constexpr v3 PaleGreen = a3MakeRGBv3(a3SwapEndian32(0x98FB9800));
	constexpr v3 PaleTurquoise = a3MakeRGBv3(a3SwapEndian32(0xAFEEEE00));
	constexpr v3 PaleVioletRed = a3MakeRGBv3(a3SwapEndian32(0xDB709300));
	constexpr v3 PapayaWhip = a3MakeRGBv3(a3SwapEndian32(0xFFEFD500));
	constexpr v3 PeachPuff = a3MakeRGBv3(a3SwapEndian32(0xFFDAB900));
	constexpr v3 Peru = a3MakeRGBv3(a3SwapEndian32(0xCD853F00));
	constexpr v3 Pink = a3MakeRGBv3(a3SwapEndian32(0xFFC0CB00));
	constexpr v3 Plum = a3MakeRGBv3(a3SwapEndian32(0xDDA0DD00));
	constexpr v3 PowderBlue = a3MakeRGBv3(a3SwapEndian32(0xB0E0E600));
	constexpr v3 Purple = a3MakeRGBv3(a3SwapEndian32(0x80008000));
	constexpr v3 RebeccaPurple = a3MakeRGBv3(a3SwapEndian32(0x66339900));
	constexpr v3 Red = a3MakeRGBv3(a3SwapEndian32(0xFF000000));
	constexpr v3 RosyBrown = a3MakeRGBv3(a3SwapEndian32(0xBC8F8F00));
	constexpr v3 RoyalBlue = a3MakeRGBv3(a3SwapEndian32(0x4169E100));
	constexpr v3 SaddleBrown = a3MakeRGBv3(a3SwapEndian32(0x8B451300));
	constexpr v3 Salmon = a3MakeRGBv3(a3SwapEndian32(0xFA807200));
	constexpr v3 SandyBrown = a3MakeRGBv3(a3SwapEndian32(0xF4A46000));
	constexpr v3 SeaGreen = a3MakeRGBv3(a3SwapEndian32(0x2E8B5700));
	constexpr v3 SeaShell = a3MakeRGBv3(a3SwapEndian32(0xFFF5EE00));
	constexpr v3 Sienna = a3MakeRGBv3(a3SwapEndian32(0xA0522D00));
	constexpr v3 Silver = a3MakeRGBv3(a3SwapEndian32(0xC0C0C000));
	constexpr v3 SkyBlue = a3MakeRGBv3(a3SwapEndian32(0x87CEEB00));
	constexpr v3 SlateBlue = a3MakeRGBv3(a3SwapEndian32(0x6A5ACD00));
	constexpr v3 SlateGray = a3MakeRGBv3(a3SwapEndian32(0x70809000));
	constexpr v3 SlateGrey = a3MakeRGBv3(a3SwapEndian32(0x70809000));
	constexpr v3 Snow = a3MakeRGBv3(a3SwapEndian32(0xFFFAFA00));
	constexpr v3 SpringGreen = a3MakeRGBv3(a3SwapEndian32(0x00FF7F00));
	constexpr v3 SteelBlue = a3MakeRGBv3(a3SwapEndian32(0x4682B400));
	constexpr v3 Tan = a3MakeRGBv3(a3SwapEndian32(0xD2B48C00));
	constexpr v3 Teal = a3MakeRGBv3(a3SwapEndian32(0x00808000));
	constexpr v3 Thistle = a3MakeRGBv3(a3SwapEndian32(0xD8BFD800));
	constexpr v3 Tomato = a3MakeRGBv3(a3SwapEndian32(0xFF634700));
	constexpr v3 Turquoise = a3MakeRGBv3(a3SwapEndian32(0x40E0D000));
	constexpr v3 Violet = a3MakeRGBv3(a3SwapEndian32(0xEE82EE00));
	constexpr v3 Wheat = a3MakeRGBv3(a3SwapEndian32(0xF5DEB300));
	constexpr v3 White = a3MakeRGBv3(a3SwapEndian32(0xFFFFFF00));
	constexpr v3 WhiteSmoke = a3MakeRGBv3(a3SwapEndian32(0xF5F5F500));
	constexpr v3 Yellow = a3MakeRGBv3(a3SwapEndian32(0xFFFF0000));
	constexpr v3 YellowGreen = a3MakeRGBv3(a3SwapEndian32(0x9ACD3200));

} }

//
// DECLARATION
//

namespace a3 {

	inline v3 MixColor(v3 c1, v3 c2);
	inline v4 MixColor(v4 c1, v4 c2);
	inline v4 BlendColor(v4 c1, v4 c2, f32 blend = 0.5f);
	inline v3 BlendColor(v3 c1, v3 c2, f32 blend = 0.5f);
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

inline v4 a3::MixColor(v4 c1, v4 c2)
{
	v4 result;
	result.r = c1.r * c2.r;
	result.g = c1.g * c2.g;
	result.b = c1.b * c2.b;
	result.a = c1.a * c2.a;
	return result;
}

inline v4 a3::BlendColor(v4 c1, v4 c2, f32 blend)
{
	return Lerp(c1, c2, blend);
}

inline v3 a3::BlendColor(v3 c1, v3 c2, f32 blend)
{
	return (1.0f - blend) * c1 + blend * c2;
}

inline v3 a3::GrayScaleColor(v3 c)
{
	v3 result;
	result.r = c.r * 0.2627f;
	result.g = c.g * 0.6780f;
	result.b = c.b * 0.0593f;
	return result;
}