#pragma once
#include "GLResources.h"
#include "Utility/Resource.h"

// TODO(Zero):
// Make this proper, this is only temporary code, don't wry guys ;)
namespace a3 {
	enum asset_id
	{
		UIFont,
		UITexture,
		UserFont,
		DebugFont
	};

	struct font_texture
	{
		a3::texture Texture;
		a3::font_atlas_info AtlasInfo;
	};

}