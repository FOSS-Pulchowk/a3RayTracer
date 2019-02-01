#pragma once
#include "Common/Core.h"
#include "Math/Math.h"
#include "Assets.h"

struct a3_renderer;

namespace a3 {

	struct basic2d_renderer
	{
	private:
		u32 m_VertexArrayObject;
		u32 m_VertexArrayBuffer;
		u32 m_ElementArrayBuffer;
		u32 m_ShaderProgram;
		u32 m_uProjection;
		u32 m_uTextureDiffuse;
		basic2d_renderer(){}
	public:
		void SetRegion(f32 left, f32 right, f32 bottom, f32 top);
		void SetRegion(const m4x4& p);
		void BeginFrame();
		// TODO(Zero): Implement a Push function, EndFrame should only draw the sprites
		void EndFrame(v3 position, v2 dimension, v3 color[4], a3::texture* texture);

		friend struct a3_renderer;
	};

	struct font_renderer
	{
	private:
		u32 m_VertexArrayObject;
		u32 m_VertexArrayBuffer;
		u32 m_ElementArrayBuffer;
		u32 m_ShaderProgram;
		u32 m_uProjection;
		u32 m_uFontAtlas;
		u32 m_uColor;
		a3::font_texture* m_FontTexture;
		
		font_renderer(){}
	public:
		void SetRegion(f32 left, f32 right, f32 bottom, f32 top);
		void SetRegion(const m4x4& p);
		void SetFont(a3::font_texture* ft);
		void Render(s8 font, v2 position, f32 height, v3 color);
		void Render(s8 font, v2 c1, v2 c2, f32 height, v3 color);

		friend struct a3_renderer;
	};

	struct batch2d_renderer
	{
	private:
		u32 m_VertexArrayObject;
		u32 m_VertexArrayBuffer;
		u32 m_ElementArrayBuffer;
		u32 m_ShaderProgram;
		u32 m_uProjection;
		u32 m_uTextureAtlas;
		u32 m_uSpotLightPosition;
		u32 m_uSpotLightColor;
		u32 m_uSpotLightIntensity;
		u32 m_Count;
		texture* m_Texture;
	public:
		void SetRegion(f32 left, f32 right, f32 bottom, f32 top);
		void SetRegion(const m4x4& p);
		void SetTexture(a3::texture* texture);
		void SetSpotLightIntensity(f32 intensity);
		void SetSpotLightColor(v3 color);
		void SetSpotLightPosition(v2 position);
		void SetSpotLightMaterial(v3 color, f32 intensity);
		void Push(v2 position, v2 dimension, v3 color, v4 texDimension);
		void BeginFrame();
		void EndFrame();

		friend struct a3_renderer;
	};
}

struct a3_renderer
{
	void Initialize() const;
	a3::basic2d_renderer Create2DRenderer(s8 vSource, s8 fSource) const;
	a3::font_renderer CreateFontRenderer(s8 vSource, s8 fSource) const;
	a3::batch2d_renderer CreateBatch2DRenderer(s8 vSource, s8 fSource) const;
};

namespace a3 {
	extern const a3_renderer Renderer;
}