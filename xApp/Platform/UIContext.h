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

		v3 m_UIColor;
		v3 m_ActiveUIColor;
		v3 m_HotUIColor;
		v3 m_UIFontColor;

	private:
		b32 IsInteracted(i32 uid, v2 position, v2 dimension)
		{
			a3Assert(uid != -1);
			b32 result = false;
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
			return result;
		}

		void RenderUI(i32 uid, v2 position, v2 dimension, v4 texDimension)
		{
			v3 finalColor;
			if (m_Active == uid)
			{
				finalColor = m_ActiveUIColor;
				m_Batch2DRenderer.SetSpotLightIntensity(5.0f);
			}
			else if (m_Hot == uid)
			{
				finalColor = m_HotUIColor;
				m_Batch2DRenderer.SetSpotLightIntensity(5.0f);
			}
			else
			{
				finalColor = m_UIColor;
				m_Batch2DRenderer.SetSpotLightIntensity(2.0f);
			}

			m_Batch2DRenderer.BeginFrame();
			m_Batch2DRenderer.SetSpotLightPosition(v2{ m_Input.mouseX, m_Input.mouseY });
			m_Batch2DRenderer.Push(position, dimension, finalColor, texDimension);
			m_Batch2DRenderer.EndFrame();
		}

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
			SetColor(a3::color::Aqua, a3::color::Aqua, a3::color::Blurple, a3::color::WhiteSmoke);
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
			b32 result = IsInteracted(uid, position, dimension);
			v4 texDimension = { 0.0f, 0.35f, 1.0f, 0.57f };
			RenderUI(uid, position, dimension, texDimension);
			position += v2{ 20.0f, 20.0f };
			v2 fontRegionDim = dimension;
			fontRegionDim.y *= 0.5f;
			m_FontRenderer.Render(desc, position, position + fontRegionDim, fontRegionDim.y, m_UIFontColor);
			return result;
		}

		b32 Checkbox(i32 uid, v2 position, v2 dimension, b32 checked, s8 desc)
		{
			b32 result = IsInteracted(uid, position, dimension);
			v4 texDimension;
			if (checked)
				texDimension = { 0.0f, 0.56f, 1.0f, 0.79f };
			else
				texDimension = { 0.0f, 0.77f, 1.0f, 1.0f };
			RenderUI(uid, position, dimension, texDimension);
			position += (v2{ 20.0f, 20.0f } +v2{ 0.15f * dimension.x, 0.0f });
			v2 fontRegionDim = dimension;
			fontRegionDim.y *= 0.5f;
			m_FontRenderer.Render(desc, position, position + fontRegionDim, fontRegionDim.y, m_UIFontColor);
			return result;
		}

		void SetColor(v3 color, v3 hot, v3 active, v3 font)
		{
			m_UIColor = color;
			m_HotUIColor = hot;
			m_ActiveUIColor = active;
			m_UIFontColor = font;
			m_Batch2DRenderer.SetSpotLightColor(hot);
		}
	};

}