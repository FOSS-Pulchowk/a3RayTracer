#pragma once
#include "Common/Core.h"
#include "GlRenderer.h"
#include "GLSL/GLSLShaders.h"
#include "AssetManager.h"
#include "Assets.h"

namespace a3 {

	struct ui_context
	{
	private:
		f32 m_MouseX;
		f32 m_MouseY;
		b32 m_MouseUp;
		b32 m_MouseDown;
		b32 m_MouseDragging;
		i32 m_Active;
		i32 m_Hot;
		batch2d_renderer m_Batch2DRenderer;
		font_renderer m_FontRenderer;
	public:
		ui_context(f32 left, f32 right, f32 bottom, f32 top) :
			m_Batch2DRenderer(a3::Renderer.CreateBatch2DRenderer(Shaders::GLBatch2DVertex, Shaders::GLBatch2DFragment)),
			m_FontRenderer(a3::Renderer.CreateFontRenderer(Shaders::GLFontVertex, Shaders::GLFontFragment))
		{
			m_MouseX = 0.0f;
			m_MouseY = 0.0f;
			m_Active = -1;
			m_Hot = -1;
			m_MouseDragging = false;
			m_FontRenderer.SetRegion(left, right, bottom, top);
			a3::Asset.LoadFontTextureAtlasFromFile(a3::asset_id::UIFont, "Resources/HackRegular.ttf", 30.0f);
			m_FontRenderer.SetFont(a3::Asset.Get<a3::font_texture>(a3::asset_id::UIFont));
			m_Batch2DRenderer.SetRegion(left, right, bottom, top);
			a3::Asset.LoadTexture2DFromFile(a3::asset_id::UITexture, "Resources/UIAtlas.png", GL_LINEAR, GL_CLAMP_TO_EDGE);
			m_Batch2DRenderer.SetTexture(a3::Asset.Get<a3::texture>(a3::asset_id::UITexture));
		}

		void UpdateIO(a3_input_system& io)
		{
			m_MouseX = (f32)io.MouseX;
			m_MouseY = (f32)io.MouseY;
			m_MouseUp = (io.Buttons[a3::ButtonLeft] == a3::ButtonUp);
			m_MouseDown = (io.Buttons[a3::ButtonLeft] == a3::ButtonDown);
		}

		b32 Button(i32 uid, v2 position, v2 dimension, v3 color, s8 desc)
		{
			a3Assert(uid != -1);
			i32 result = false;
			if (m_Active == uid)
			{
				if (m_MouseUp)
				{
					if (m_Hot == uid)
					{
						result = true;
					}
					m_Active = -1;
				}
			}
			else if (m_Hot == uid)
			{
				if (m_MouseDown && !m_MouseDragging)
				{
					m_Active = uid;
				}
			}

			if (m_MouseDown && m_Hot == -1)
			{
				m_MouseDragging = true;
			}
			else if (m_MouseUp && m_MouseDragging)
			{
				a3LogTrace("Mouse dragging reset");
				m_MouseDragging = false;
			}

			if (m_MouseX > position.x && m_MouseX < (position.x + dimension.x) &&
				m_MouseY > position.y && m_MouseY < (position.y + dimension.y))
			{
				if (m_Active == -1)
				{
					m_Hot = uid;
				}
			}
			else if (m_Hot == uid)
			{
				m_Hot = -1;
			}

			v3 acolor[4];
			v3 finalColor;
			if (m_Active == uid)
			{
				finalColor = { 0.0f, 0.0f, 1.0f };
				acolor[0] = finalColor;
				acolor[1] = finalColor;
				acolor[2] = finalColor;
				acolor[3] = finalColor;
			}
			else if (m_Hot == uid)
			{
				finalColor = { 0.0f, 1.0f, 0.0f };
				acolor[0] = finalColor;
				acolor[1] = finalColor;
				acolor[2] = finalColor;
				acolor[3] = finalColor;
			}
			else
			{
				finalColor = color;
				acolor[0] = finalColor;
				acolor[1] = finalColor;
				acolor[2] = finalColor;
				acolor[3] = finalColor;
			}

			v4 texDimension = { 0.0f, 0.35f, 1.0f, 0.57f };
			m_Batch2DRenderer.BeginFrame();
			m_Batch2DRenderer.Push(position, dimension, acolor, texDimension);
			m_Batch2DRenderer.EndFrame();
			position.x += 20.0f;
			position.y += 20.0f;
			m_FontRenderer.Render(desc, position, dimension.y * 0.8f, { 1.0f, 1.0f, 1.0f });

			return result;
		}
	};

}