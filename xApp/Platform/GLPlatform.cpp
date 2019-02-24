#pragma once
#include "Common/Core.h"
#include "HardwarePlatform.h"

#include "gl/glad.h"
#include "gl/glDebug.h"

namespace a3 {
	const a3_hardware_platform GPU;
}

a3::image_texture a3_hardware_platform::CreateTexture(a3::texture_type type, a3::filter filter, a3::wrap wrap) const
{
	a3::image_texture texture;
	a3GL(glGenTextures(1, &texture.Id));
	a3Assert(type == a3::texture_type::TextureType2D);
	GLenum gtype = GL_TEXTURE_2D;
	a3GL(glBindTexture(gtype, texture.Id));
	if (filter == a3::FilterLinear)
	{
		a3GL(glTexParameteri(gtype, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		a3GL(glTexParameteri(gtype, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	}
	else
	{
		a3GL(glTexParameteri(gtype, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
		a3GL(glTexParameteri(gtype, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	}
	if (wrap == a3::WrapClampToEdge)
	{
		a3GL(glTexParameteri(gtype, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		a3GL(glTexParameteri(gtype, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	}
	else
	{
		a3GL(glTexParameteri(gtype, GL_TEXTURE_WRAP_S, GL_REPEAT));
		a3GL(glTexParameteri(gtype, GL_TEXTURE_WRAP_T, GL_REPEAT));
	}
	return texture;
}

a3::image_texture a3_hardware_platform::CreateTexture2DFromBuffer(a3::filter filter, a3::wrap wrap, void * buffer, i32 w, i32 h, i32 n) const
{
	a3::image_texture result = a3::GPU.CreateTexture(a3::TextureType2D, filter, wrap);
	GLenum format;
	GLenum internal;
	switch (n)
	{
	case 1:
	{
		a3GL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
		format = GL_RED;
		internal = GL_RED;
	} break;
	case 3:
	{
		format = GL_RGB;
		internal = GL_RGB8;
	} break;
	case 4:
	{
		format = GL_RGBA;
		internal = GL_RGBA8;
	} break;
	default:
	{
		a3TriggerBreakPoint();
		a3GL(glDeleteTextures(1, &result.Id));
		result.Id = 0;
		return result;
	}
	}
	a3GL(glTexImage2D(GL_TEXTURE_2D, 0, internal, w, h, 0, format, GL_UNSIGNED_BYTE, buffer));
	if (n == 1) a3GL(glPixelStorei(GL_UNPACK_ALIGNMENT, 4));
	result.Width = w;
	result.Height = h;
	return result;
}

void a3_hardware_platform::DrawSubImageTexture2D(a3::image_texture * texture, i32 xOffset, i32 yOffset, i32 w, i32 h, i32 n, u8 * pixels) const
{
	GLenum format;
	switch (n)
	{
	case 1:
	{
		a3GL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
		format = GL_RED;
	} break;
	case 3:
	{
		format = GL_RGB;
	} break;
	case 4:
	{
		format = GL_RGBA;
	} break;
	default:
	{
		a3TriggerBreakPoint();
	}
	}
	// TODO(Zero):
	// Do not bind the thing everytime to for subimaging
	a3GL(glBindTexture(GL_TEXTURE_2D, texture->Id));
	a3GL(glTexSubImage2D(GL_TEXTURE_2D, 0, xOffset, yOffset, w, h, format, GL_UNSIGNED_BYTE, pixels));
}
