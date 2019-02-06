#pragma once
#include "Common/Core.h"
#include "gl/glad.h"
#include "Platform.h"
#include "gl/glDebug.h"

//
// DECLARATIONS
//

namespace a3 {

	struct texture
	{
		u32 Id;
		i32 Width;
		i32 Height;
	};

	inline texture GLMakeTexture(GLenum type, GLenum filter, GLenum wrap);
	inline texture GLMakeTexture2DFromBuffer(GLenum filter, GLenum wrap, void* buffer, i32 w, i32 h, i32 n);
	inline void GLSubImageTexture2D(texture * tex, i32 xOffset, i32 yOffset, i32 w, i32 h, i32 n, u8 * pixels);
}


//
// IMPLEMENTATIONS
//

inline a3::texture a3::GLMakeTexture(GLenum type, GLenum filter, GLenum wrap)
{
	texture texture;
	a3GL(glGenTextures(1, &texture.Id));
	a3GL(glBindTexture(type, texture.Id));
	a3GL(glTexParameteri(type, GL_TEXTURE_MIN_FILTER, filter));
	a3GL(glTexParameteri(type, GL_TEXTURE_MAG_FILTER, filter));
	a3GL(glTexParameteri(type, GL_TEXTURE_WRAP_S, wrap));
	a3GL(glTexParameteri(type, GL_TEXTURE_WRAP_T, wrap));
	return texture;
}

inline a3::texture a3::GLMakeTexture2DFromBuffer(GLenum filter, GLenum wrap, void* buffer, i32 w, i32 h, i32 n)
{
	texture result = GLMakeTexture(GL_TEXTURE_2D, filter, wrap);
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

void a3::GLSubImageTexture2D(texture * tex, i32 xOffset, i32 yOffset, i32 w, i32 h, i32 n, u8 * pixels)
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
	a3GL(glBindTexture(GL_TEXTURE_2D, tex->Id));
	a3GL(glTexSubImage2D(GL_TEXTURE_2D, 0, xOffset, yOffset, w, h, format, GL_UNSIGNED_BYTE, pixels));
}
