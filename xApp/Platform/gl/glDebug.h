#pragma once
#include "Common/Core.h"
#include "glad.h"

#if defined(XDEBUG) || defined(XINTERNAL)
inline s8 glGetErrorString(u32 errorCode)
{
	switch(errorCode)
	{
		case GL_INVALID_ENUM:                  return "Invalid enum";
		case GL_INVALID_VALUE:                 return "Invalid value";
		case GL_INVALID_OPERATION:             return "Invalid operation";
		case GL_STACK_OVERFLOW:                return "Stack overflow";
		case GL_STACK_UNDERFLOW:               return "Stack underflow";
		case GL_OUT_OF_MEMORY:                 return "Out of memory";
		case GL_INVALID_FRAMEBUFFER_OPERATION: return "Invalid frame buffer operation";
		default: return "Unknow OpenGL error";
	}
}
#endif

#if defined(XDEBUG) || defined(XINTERNAL)
#define xGL(x) \
x;\
{\
GLenum error = glGetError();\
while (error != GL_NO_ERROR)\
{\
	xLogError("OpenGL Call[{u}]: {s}, File: {s}, Line: {i}", error, glGetErrorString(error), __FILE__, __LINE__);\
	xAssert(error == GL_NO_ERROR);\
	error = glGetError();\
}\
}
#else
#define xGL(x) x
#endif