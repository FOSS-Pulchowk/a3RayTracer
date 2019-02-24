#pragma once
#include "Utility/AssetData.h"

struct a3_hardware_platform
{
	a3::image_texture CreateTexture(a3::texture_type type, a3::filter filter, a3::wrap wrap) const;
	a3::image_texture CreateTexture2DFromBuffer(a3::filter filter, a3::wrap wrap, void* buffer, i32 w, i32 h, i32 n) const;
	void DrawSubImageTexture2D(a3::image_texture* texture, i32 xOffset, i32 yOffset, i32 w, i32 h, i32 n, u8 * pixels) const;
};

namespace a3 {

	extern const a3_hardware_platform GPU;

}