#pragma once
#include "Common/Core.h"
#include "Platform/Platform.h"
#include "Glad.h"

#if defined(A3DEBUG) || defined(A3INTERNAL)
inline s8 a3_GLGetErrorString(u32 errorCode)
{
	switch(errorCode)
	{
		case GL_INVALID_ENUM:                  return "Invalid enum";
		case GL_INVALID_VALUE:                 return "Invalid value";
		case GL_INVALID_OPERATION:             return "Invalid operation";
		case GL_OUT_OF_MEMORY:                 return "Out of memory";
		case GL_INVALID_FRAMEBUFFER_OPERATION: return "Invalid frame buffer operation";
		default: return "Unknow OpenGL error";
	}
}
#endif

#if defined(A3DEBUG) || defined(A3INTERNAL)
#define a3GL(x) \
x;\
{\
GLenum error = glGetError();\
while (error != GL_NO_ERROR)\
{\
	a3LogError("OpenGL Call[{u}]: {s}", error, a3_GLGetErrorString(error));\
	a3Assert(error == GL_NO_ERROR);\
	error = glGetError();\
}\
}
#else
#define a3GL(x) x
#endif