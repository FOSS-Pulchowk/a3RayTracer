#pragma once
#include "Common/Core.h"
#include "Math/Math.h"
#include "Graphics/Image.h"

struct x_vfont
{
	enum
	{
		POSITEXCORDS = 0
	};
	v4 positionTexCoords;
};

struct a3_renderer;

namespace a3 {

	struct basic2drenderer
	{
	private:
		u32 m_VertexArrayObject;
		u32 m_VertexArrayBuffer;
		u32 m_ElementArrayBuffer;
		u32 m_ShaderProgram;
		u32 m_Projection;
		u32 m_TextureDiffuse;
		basic2drenderer(){}
	public:
		void SetRegion(f32 left, f32 right, f32 bottom, f32 top);
		void Render(v3 position, v2 dimension, v3 color[4], u32 texture);

		friend struct a3_renderer;
	};

	struct renderer_font
	{
		u32 VertexBufferObject;
		u32 VertexArrayBuffer;
		u32 ShaderProgram;
		m4x4 Projection;
	};

	const renderer_font CreateFontRenderer(u32 program);

	void RenderFont(const renderer_font& renderer, s8 string, const gl_textures& texts, v2 position, v3 color, f32 scale);
}

struct a3_renderer
{
	a3::basic2drenderer Create2DRenderer() const;
};
namespace a3 {
	extern const a3_renderer Renderer;
}