#pragma once
#include "Common/Core.h"
#include "Platform.h"
#include "GL/Glad.h"
#include "GL/GLDebug.h"

inline u32 GLCompileShader(GLenum type, s8 source)
{
	a3GL(u32 shader = glCreateShader(type));
	a3GL(glShaderSource(shader, 1, &source, 0));
	a3GL(glCompileShader(shader));
	i32 result;
	a3GL(glGetShaderiv(shader, GL_COMPILE_STATUS, &result));
	if (result != GL_TRUE)
	{
		i32 len;
		utf8 errMsg[1024];
		a3GL(glGetShaderInfoLog(shader, 1024, &len, errMsg));
		a3GL(a3LogWarn("Shader source could not be compiled!\n"));
		a3LogWarn("Shader Compilation Error: '{s}'\n", errMsg);
		a3GL(glDeleteShader(shader));
		return 0;
	}
	return shader;
}

inline u32 GLCreateShaderProgramFromFile(s8 vSource, s8 fSource)
{
	u32 vShader = GLCompileShader(GL_VERTEX_SHADER, vSource);
	u32 fShader = GLCompileShader(GL_FRAGMENT_SHADER, fSource);

	a3GL(u32 program = glCreateProgram());
	a3GL(glAttachShader(program, vShader));
	a3GL(glAttachShader(program, fShader));
	a3GL(glLinkProgram(program));
	a3GL(glDeleteShader(vShader));
	a3GL(glDeleteShader(fShader));
	i32 result;
	a3GL(glGetProgramiv(program, GL_LINK_STATUS, &result));
	if (result != GL_TRUE)
	{
		i32 len;
		utf8 errMsg[1024];
		a3GL(glGetProgramInfoLog(program, 1024, &len, errMsg));
		a3GL(a3LogWarn("Shaders could not be linked!\n"));
		a3GL(glDeleteProgram(program));
		return 0;
	}
	a3GL(glUseProgram(0));
	return program;
}