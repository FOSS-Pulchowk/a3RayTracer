#pragma once
#include "Common/Core.h"
#include "Math/Math.h"
#include "Graphics/Image.h"

struct x_v2d
{
	enum
	{
		POSITION = 0, COLOR, TEXCOORDS
	};
	v3 position;
	v3 color;
	v2 texCoords;
};

struct x_vfont
{
	enum
	{
		POSITEXCORDS = 0
	};
	v4 positionTexCoords;
};

namespace a3 {

	typedef u32 shader_program;

	struct renderer2d
	{
		u32 VertexBufferObject;
		u32 VertexArrayBuffer;
		u32 ElementArrayBuffer;
		shader_program ShaderProgram;
	};

	struct renderer_font
	{
		u32 VertexBufferObject;
		u32 VertexArrayBuffer;
		shader_program ShaderProgram;
		m4x4 Projection;
	};

	const renderer2d CreateBatchRenderer2D(shader_program program);
	const renderer_font CreateFontRenderer(shader_program program);

	void RenderFont(const renderer_font& renderer, s8 string, const gl_textures& texts, v2 position, v3 color, f32 scale);

}