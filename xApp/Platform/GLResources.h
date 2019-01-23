#pragma once
#include "Common/Core.h"
#include "gl/glad.h"
#include "Platform.h"
#include "gl/glDebug.h"

namespace a3 {

	using texture = u32;

	inline texture GLMakeTexture(GLenum type, GLenum filter, GLenum wrap)
	{
		texture texture;
		a3GL(glGenTextures(1, &texture));
		a3GL(glBindTexture(type, texture));
		a3GL(glTexParameteri(type, GL_TEXTURE_MIN_FILTER, filter));
		a3GL(glTexParameteri(type, GL_TEXTURE_MAG_FILTER, filter));
		a3GL(glTexParameteri(type, GL_TEXTURE_WRAP_S, wrap));
		a3GL(glTexParameteri(type, GL_TEXTURE_WRAP_T, wrap));
		return texture;
	}

	inline texture GLMakeTextureFromBuffer(GLenum type, GLenum filter, GLenum wrap, void* buffer, i32 w, i32 h, i32 n)
	{
		texture result = GLMakeTexture(type, filter, wrap);
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
			a3GL(glDeleteTextures(1, &result));
			return 0;
		}
		}
		a3GL(glTexImage2D(type, 0, internal, w, h, 0, format, GL_UNSIGNED_BYTE, buffer));
		if (n == 1) a3GL(glPixelStorei(GL_UNPACK_ALIGNMENT, 4));
		return result;
	}

}