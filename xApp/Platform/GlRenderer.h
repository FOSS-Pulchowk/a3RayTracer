#pragma once
#include "Common/Core.h"
#include "Math/Math.h"
#include "Graphics/Image.h"

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

	struct font_renderer
	{
	private:
		u32 m_VertexArrayObject;
		u32 m_VertexArrayBuffer;
		u32 m_ElementArrayBuffer;
		u32 m_ShaderProgram;
		u32 m_Projection;
		u32 m_FontAtlas;
		u32 m_Color;
		font_renderer(){}
	public:
		void SetRegion(f32 left, f32 right, f32 bottom, f32 top);
		void Render(s8 font, v2 position, f32 scale, v3 color, u32 texture, const a3::fonts& f);

		friend struct a3_renderer;
	};
}

struct a3_renderer
{
	a3::basic2drenderer Create2DRenderer() const;
	a3::font_renderer CreateFontRenderer() const;
};

namespace a3 {
	extern const a3_renderer Renderer;
}