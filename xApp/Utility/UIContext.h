#pragma once
#include "Common/Core.h"
#include "GlRenderer.h"
#include "GLSL/GLSLShaders.h"
#include "Utility/AssetManager.h"
#include "Math/Color.h"
#include "Utility/Memory.h"
#include "Utility/DString.h"

//
// DECLARATIONS
//

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
		v3 m_UIFontColor;
		v3 m_UIActiveFontColor;

		struct font_data
		{
			a3::dstring text;
			v2 position;
			v2 dimension;
			f32 height;
			v3 color;

			font_data(s8 text, v2 pos, v2 dim, f32 h, v3 col) :
				text(text), position(pos), dimension(dim), height(h), color(col)
			{
			}
		};
        
        a3::queue<a3::ui_context::font_data> m_Fontq;
        
        public:
		ui_context(f32 width, f32 height);
		inline void UpdateIO(const input_info& input);
        
        inline void BeginFrame(v2 position);
        inline void EndFrame();
        
		inline b32 Button(i32 uid, v2 position, v2 dimension, s8 desc);
		inline b32 Checkbox(i32 uid, v2 position, v2 dimension, b32 checked, s8 desc);
		inline void SetColor(v3 color, v3 hot, v3 active, v3 font);
        
        private:
		inline i32 IsInteracted(i32 uid, v2 position, v2 dimension);
		inline void RenderUI(i32 uid, v2 position, v2 dimension, v4 texDimension);
	};
    
}

//
// IMPLEMENTATION
//

a3::ui_context::ui_context(f32 width, f32 height) :
m_Batch2DRenderer(a3::Renderer.CreateBatch2DRenderer(shaders::GLBatch2DVertex, shaders::GLBatch2DFragment)),
m_FontRenderer(a3::Renderer.CreateFontRenderer(shaders::GLFontVertex, shaders::GLFontFragment)),
m_Fontq(a3Allocate(sizeof(a3::ui_context::font_data) * 25, void*), sizeof(a3::ui_context::font_data) * 25)
{
	m_Width = width;
	m_Height = height;
	m_Active = -1;
	m_Hot = -1;
	m_Input.mouseDragging = false;
	
	a3::Asset.LoadFontTextureAtlasFromFile(a3::asset_id::UIFont, "Resources/HackRegular.ttf", 30.0f);
	a3::Asset.LoadTexture2DFromFile(a3::asset_id::UITexture, "Resources/A3UI.png", a3::FilterLinear, a3::WrapClampToEdge);
	
	m_FontRenderer.SetRegion(0.0f, m_Width, 0.0f, m_Height);
	m_FontRenderer.SetFont(a3::Asset.Get<a3::font_texture>(a3::asset_id::UIFont));
	
	m_Batch2DRenderer.SetRegion(0.0f, m_Width, 0.0f, m_Height);
	m_Batch2DRenderer.SetTexture(a3::Asset.Get<a3::image_texture>(a3::asset_id::UITexture));
	SetColor(a3::color::NotQuiteBlack, a3::color::Blurple, a3::color::Grey, a3::color::White);
}

inline void a3::ui_context::UpdateIO(const input_info& input)
{
	m_Input.mouseX = input.MouseX * m_Width;
	m_Input.mouseY = input.MouseY * m_Height;
	m_Input.mouseUp = input.Buttons[a3::ButtonLeft] == a3::ButtonUp;
	m_Input.mouseDown = input.Buttons[a3::ButtonLeft] == a3::ButtonDown;
}

inline void a3::ui_context::BeginFrame(v2 position)
{
    m_Batch2DRenderer.BeginFrame();
    m_Fontq.Empty();
}

inline void a3::ui_context::EndFrame()
{
    m_Batch2DRenderer.EndFrame();
	a3::ui_context::font_data* data = m_Fontq.GetFront();
	for (u32 i = 0; i < m_Fontq.QueryFillCounts(); ++i)
	{
		m_FontRenderer.Render(data->text.Utf8Array(), data->position, data->dimension, data->height, data->color);
		data++;
	}
	m_Fontq.Empty();
}

inline b32 a3::ui_context::Button(i32 uid, v2 position, v2 dimension, s8 desc)
{
	b32 result = IsInteracted(uid, position, dimension);
	v4 texDimension = { 0.0f, 0.7f, 1.0f, 1.0f };
	RenderUI(uid, position, dimension, texDimension);
	position += 0.25f * dimension;
	v2 fontRegionDim = dimension;
	fontRegionDim.y *= 0.6f;
	m_Fontq.Emplace(desc, position, position + fontRegionDim, fontRegionDim.y, (m_Active != uid && m_Hot != uid) ? m_UIFontColor : m_UIActiveFontColor);
	//m_FontRenderer.Render(desc, position, position + fontRegionDim, fontRegionDim.y, (m_Active != uid && m_Hot != uid) ? m_UIFontColor : m_UIActiveFontColor);
	return result;
}

inline b32 a3::ui_context::Checkbox(i32 uid, v2 position, v2 dimension, b32 checked, s8 desc)
{
	b32 result = IsInteracted(uid, position, dimension);
	v4 texDimension;
	if (checked)
		texDimension = { 0.0f, 0.0f, 1.0f, 0.3f };
	else
		texDimension = { 0.0f, 0.34f, 1.0f, 0.64f };
	RenderUI(uid, position, dimension, texDimension);
	position += (0.25f * dimension + v2{ 0.1f * dimension.x, 0.0f });
	v2 fontRegionDim = dimension;
	fontRegionDim.y *= 0.6f;
	m_Fontq.Emplace(desc, position, position + fontRegionDim, fontRegionDim.y, (m_Active != uid && m_Hot != uid) ? m_UIFontColor : m_UIActiveFontColor);
	//m_FontRenderer.Render(desc, position, position + fontRegionDim, fontRegionDim.y, (m_Active != uid && m_Hot != uid) ? m_UIFontColor : m_UIActiveFontColor);
	return result;
}

inline void a3::ui_context::SetColor(v3 ui, v3 activeui, v3 font, v3 activefont)
{
	m_UIColor = ui;
	m_ActiveUIColor = activeui;
	m_UIFontColor = font;
	m_UIActiveFontColor = activefont;
}

inline b32 a3::ui_context::IsInteracted(i32 uid, v2 position, v2 dimension)
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

inline void a3::ui_context::RenderUI(i32 uid, v2 position, v2 dimension, v4 texDimension)
{
	v3 finalColor;
	if (m_Active == uid || m_Hot == uid)
	{
		finalColor = m_ActiveUIColor;
	}
	else
	{
		finalColor = m_UIColor;
	}
    
	//m_Batch2DRenderer.BeginFrame();
	m_Batch2DRenderer.Push(position, dimension, finalColor, texDimension);
	//m_Batch2DRenderer.EndFrame();
}
