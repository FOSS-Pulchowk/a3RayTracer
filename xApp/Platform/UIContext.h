#pragma once
#include "Common/Core.h"
#include "GlRenderer.h"
#include "GLSL/GLSLShaders.h"

namespace a3 {

	struct ui_context
	{
	private:
		f32 m_MouseX;
		f32 m_MouseY;
		b32 m_MouseUp;
		b32 m_MouseDown;
		i32 m_Active;
		i32 m_Hot;
		batch2d_renderer m_Batch2DRenderer;
		font_renderer m_FontRenderer;
	public:
		ui_context():
			m_Batch2DRenderer(a3::Renderer.CreateBatch2DRenderer(Shaders::GLBatch2DVertex, Shaders::GLBatch2DFragment)),
			m_FontRenderer(a3::Renderer.CreateFontRenderer(Shaders::GLFontVertex, Shaders::GLFontFragment))
		{
			m_MouseX = 0.0f;
			m_MouseY = 0.0f;
			m_Active = -1;
			m_Hot = -1;
		}

		void UpdateIO(a3_input_system& io)
		{
			m_MouseX = (f32)io.MouseX;
			m_MouseY = (f32)io.MouseY;
			m_MouseUp = (io.Buttons[a3::ButtonLeft] == a3::ButtonUp);
			m_MouseDown = (io.Buttons[a3::ButtonLeft] == a3::ButtonDown);
		}

		b32 Button(i32 uid, v2 position, v2 dimension, v3 color)
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
						m_Active = -1;
					}
				}
			}
			else if (m_Hot == uid)
			{
				if (m_MouseDown) m_Active = uid;
			}

			if (m_Active != -1)
			{
				if (m_MouseX > position.x && m_MouseX < (position.x + dimension.x))
				{
					if (m_MouseY > position.y && m_MouseY < (position.y + dimension.y))
					{
						m_Hot = uid;
					}
				}
			}

			return result;
		}
	};

}