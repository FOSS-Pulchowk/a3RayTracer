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
		struct user_input
		{
			f32 mouseX;
			f32 mouseY;
			b32 mouseUp;
			b32 mouseDown;
			b32 mouseDragging;
		};
		user_input m_Input;

		i32 m_Active;
		i32 m_Hot;
		f32 m_Width;
		f32 m_Height;

		batch2d_renderer m_Batch2DRenderer;
		font_renderer m_FontRenderer;

		static v3 s_UIColor;
		static v3 s_ActiveUIColor;
		static v3 s_HotUIColor;
		static v3 s_UIFontColor;
	public:
		ui_context(f32 width, f32 height) :
			m_Batch2DRenderer(a3::Renderer.CreateBatch2DRenderer(Shaders::GLBatch2DVertex, Shaders::GLBatch2DFragment)),
			m_FontRenderer(a3::Renderer.CreateFontRenderer(Shaders::GLFontVertex, Shaders::GLFontFragment))
		{
			m_Width = width;
			m_Height = height;
			m_Active = -1;
			m_Hot = -1;
			m_Input.mouseDragging = false;
			m_FontRenderer.SetRegion(0.0f, m_Width, 0.0f, m_Height);
			a3::Asset.LoadFontTextureAtlasFromFile(a3::asset_id::UIFont, "Resources/HackRegular.ttf", 30.0f);
			m_FontRenderer.SetFont(a3::Asset.Get<a3::font_texture>(a3::asset_id::UIFont));
			m_Batch2DRenderer.SetRegion(0.0f, m_Width, 0.0f, m_Height);
			a3::Asset.LoadTexture2DFromFile(a3::asset_id::UITexture, "Resources/UIAtlas.png", GL_LINEAR, GL_CLAMP_TO_EDGE);
			m_Batch2DRenderer.SetTexture(a3::Asset.Get<a3::texture>(a3::asset_id::UITexture));
		}

		void UpdateIO(const input_info& input)
		{
			m_Input.mouseX = input.MouseX * m_Width;
			m_Input.mouseY = input.MouseY * m_Height;
			m_Input.mouseUp = input.Buttons[a3::ButtonLeft] == a3::ButtonUp;
			m_Input.mouseDown = input.Buttons[a3::ButtonLeft] == a3::ButtonDown;
		}

		b32 Button(i32 uid, v2 position, v2 dimension, s8 desc)
		{
			a3Assert(uid != -1);
			i32 result = false;
			if (m_Active == uid)
			{
				if (m_Input.mouseUp)
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
				if (m_Input.mouseDown && !m_Input.mouseDragging)
				{
					m_Active = uid;
				}
			}

			if (m_Input.mouseDown && m_Hot == -1)
			{
				m_Input.mouseDragging = true;
			}
			else if (m_Input.mouseUp && m_Input.mouseDragging)
			{
				m_Input.mouseDragging = false;
			}

			if (m_Input.mouseX > position.x && m_Input.mouseX < (position.x + dimension.x) &&
				m_Input.mouseY > position.y && m_Input.mouseY < (position.y + dimension.y))
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
				finalColor = s_ActiveUIColor;
				acolor[0] = finalColor;
				acolor[1] = finalColor;
				acolor[2] = finalColor;
				acolor[3] = finalColor;
			}
			else if (m_Hot == uid)
			{
				finalColor = s_HotUIColor;
				acolor[0] = finalColor;
				acolor[1] = finalColor;
				acolor[2] = finalColor;
				acolor[3] = finalColor;
			}
			else
			{
				finalColor = s_UIColor;
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
			v2 fontRegionDim = dimension;
			fontRegionDim.y *= 0.5f;
			m_FontRenderer.Render(desc, position, position + fontRegionDim, fontRegionDim.y, s_UIFontColor);
			return result;
		}

		inline static void SetColor(v3 color, v3 hot, v3 active, v3 font)
		{
			s_UIColor = color;
			s_HotUIColor = hot;
			s_ActiveUIColor = active;
			s_UIFontColor = font;
		}
	};

	v3 ui_context::s_UIColor = a3::color::DarkNotBlack;
	v3 ui_context::s_HotUIColor = a3::color::NotQuiteBlack;
	v3 ui_context::s_ActiveUIColor = a3::color::Blurple;
	v3 ui_context::s_UIFontColor = a3::color::WhiteSmoke;

}