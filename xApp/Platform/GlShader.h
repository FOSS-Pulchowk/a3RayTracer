#pragma once
#include "Common/Core.h"
#include "Platform.h"
#include "GL/Glad.h"
#include "GL/GLDebug.h"

inline u32 GLCompileShader(GLenum type, s8 source)
{
	xGL(u32 shader = glCreateShader(type));
	xGL(glShaderSource(shader, 1, &source, 0));
	xGL(glCompileShader(shader));
	i32 result;
	xGL(glGetShaderiv(shader, GL_COMPILE_STATUS, &result));
	if (result != GL_TRUE)
	{
		i32 len;
		utf8 errMsg[1024];
		xGL(glGetShaderInfoLog(shader, 1024, &len, errMsg));
		xGL(xLogWarn("Shader source could not be compiled!\n"));
		xLogWarn("Shader Compilation Error: '{s}'\n", errMsg);
		xGL(glDeleteShader(shader));
		return 0;
	}
	return shader;
}

inline u32 GLCreateShaderProgramFromFile(s8 vSource, s8 fSource)
{
	u32 vShader = GLCompileShader(GL_VERTEX_SHADER, vSource);
	u32 fShader = GLCompileShader(GL_FRAGMENT_SHADER, fSource);

	xGL(u32 program = glCreateProgram());
	xGL(glAttachShader(program, vShader));
	xGL(glAttachShader(program, fShader));
	xGL(glLinkProgram(program));
	xGL(glDeleteShader(vShader));
	xGL(glDeleteShader(fShader));
	i32 result;
	xGL(glGetProgramiv(program, GL_LINK_STATUS, &result));
	if (result != GL_TRUE)
	{
		i32 len;
		utf8 errMsg[1024];
		xGL(glGetProgramInfoLog(program, 1024, &len, errMsg));
		xGL(xLogWarn("Shaders could not be linked!\n"));
		xGL(glDeleteProgram(program));
		return 0;
	}
	xGL(glUseProgram(0));
	return program;
}