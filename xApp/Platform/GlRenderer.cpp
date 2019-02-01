#include "Common/Core.h"
#include "GlRenderer.h"
#include "GL/Glad.h"
#include "GL/GLDebug.h"

// TODO(Zero):
// Retain texture binds
// Make the rendering system more efficient by pushing and drawing maximum sprites in single draw call

struct a3_current_bound
{
	u32 VertexArrayObject;
	u32 VertexArrayBuffer;
	u32 ElementArrayBuffer;
	u32 ShaderProgram;

	b32 VertexArrayBufferIsMapped;
	b32 ElementArrayBufferIsMapped;
	void* MappedVertexArrayPointer;
	u32* MappedElementArrayPointer;

	i32 MaxTextureUnits; // 80 is gaurenteed, use more if available
	const i32 FontTextureAtlasSlot = 4;
	const i32 UITextureSlot = 5;
};

static a3_current_bound s_CurrentBound;

inline void a3_MapVertexPointer()
{
	if (s_CurrentBound.VertexArrayBufferIsMapped) return;
	a3GL(s_CurrentBound.MappedVertexArrayPointer = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
	s_CurrentBound.VertexArrayBufferIsMapped = true;
}

inline void a3_MapElementPointer()
{
	if (s_CurrentBound.ElementArrayBufferIsMapped) return;
	a3GL(s_CurrentBound.MappedElementArrayPointer = (u32*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));
	s_CurrentBound.ElementArrayBufferIsMapped = true;
}

inline void a3_UnmapVertexPointer()
{
	a3GL(glUnmapBuffer(GL_ARRAY_BUFFER));
	s_CurrentBound.VertexArrayBufferIsMapped = false;
}

inline void a3_UnmapElementPointer()
{
	a3GL(glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER));
	s_CurrentBound.ElementArrayBufferIsMapped = false;
}

#define a3GetMappedVertexPointer(type) (type*)s_CurrentBound.MappedVertexArrayPointer
#define a3GetMappedElementPointer() s_CurrentBound.MappedElementArrayPointer


inline void a3_BindVertexArrayObject(u32 target)
{
	if (target == s_CurrentBound.VertexArrayObject) return;
	a3GL(glBindVertexArray(target));
	s_CurrentBound.VertexArrayObject = target;
}

inline void a3_BindVertexArrayBuffer(u32 target)
{
	if (target == s_CurrentBound.VertexArrayBuffer) return;
	if (s_CurrentBound.VertexArrayBufferIsMapped)
	{
		// NOTE(Zero): Pointer is not cleared to zero
		a3_UnmapVertexPointer();
	}
	a3GL(glBindBuffer(GL_ARRAY_BUFFER, target));
	s_CurrentBound.VertexArrayBuffer = target;
}

inline void a3_BindElementArrayBuffer(u32 target)
{
	if (target == s_CurrentBound.ElementArrayBuffer) return;
	if (s_CurrentBound.ElementArrayBufferIsMapped)
	{
		// NOTE(Zero): Pointer is not cleared to zero
		a3_UnmapElementPointer();
	}
	a3GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, target));
	s_CurrentBound.ElementArrayBuffer = target;
}

inline void a3_BindProgram(u32 target)
{
	if (target == s_CurrentBound.ShaderProgram) return;
	a3GL(glUseProgram(target));
	s_CurrentBound.ShaderProgram = target;
}

inline u32 a3_CompileShader(GLenum type, s8 source)
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

inline u32 a3_CreateShaderProgramFromBuffer(s8 vSource, s8 fSource)
{
	u32 vShader = a3_CompileShader(GL_VERTEX_SHADER, vSource);
	u32 fShader = a3_CompileShader(GL_FRAGMENT_SHADER, fSource);

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
		a3GL(a3LogWarn("Shaders could not be linked: {s}", errMsg));
		a3GL(glDeleteProgram(program));
		return 0;
	}
	a3GL(glUseProgram(0));
	return program;
}

struct a3_vertex2d
{
	v3 position;
	v3 color;
	v2 texCoords;
	enum { POSITION = 0, COLOR, TEXCOORDS };
};

#define A3_FONT_RENDER_MAX 1000
#define A3_VERTICES_FONT_MAX A3_FONT_RENDER_MAX * 4
#define A3_INDICES_FONT_MAX A3_FONT_RENDER_MAX * 6
struct a3_vertex_font
{
	v4 posTexCoords;
	enum { POSTEXCOORDS = 0 };
};

#define A3_UI_RENDER_MAX 500
#define A3_VERTICES_UI_MAX A3_FONT_RENDER_MAX * 4
#define A3_INDICES_UI_MAX A3_FONT_RENDER_MAX * 6
struct a3_vertex_ui
{
	v2 position;
	v3 color;
	v2 texCoords;
	enum { POSITION = 0, COLOR, TEXCOORDS };
};

namespace a3 {
	const a3_renderer Renderer;
}

inline void a3_GenerateBuffers(u32* o, u32* v, u32* e)
{
	a3GL(glGenVertexArrays(1, o));
	a3GL(glGenBuffers(1, v));
	a3GL(glGenBuffers(1, e));
}

inline void a3_GenerateAndBind(u32* o, u32* v, u32* e)
{
	a3_GenerateBuffers(o, v, e);
	a3_BindVertexArrayObject(*o);
	a3_BindVertexArrayBuffer(*v);
	a3_BindElementArrayBuffer(*e);
}

void a3_renderer::Initialize() const
{
	a3GL(glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &s_CurrentBound.MaxTextureUnits));
}

a3::basic2d_renderer a3_renderer::Create2DRenderer(s8 vSource, s8 fSource) const
{
	a3::basic2d_renderer r;
	a3_GenerateAndBind(&r.m_VertexArrayObject, &r.m_VertexArrayBuffer, &r.m_ElementArrayBuffer);

	a3GL(glBufferData(GL_ARRAY_BUFFER, sizeof(a3_vertex2d) * 4, A3NULL, GL_DYNAMIC_DRAW));
	a3GL(glVertexAttribPointer(a3_vertex2d::POSITION, 3, GL_FLOAT,
		GL_FALSE, sizeof(a3_vertex2d),
		(void*)(a3OffsetOf(a3_vertex2d, a3_vertex2d::position))));

	a3GL(glVertexAttribPointer(a3_vertex2d::COLOR, 3, GL_FLOAT,
		GL_FALSE, sizeof(a3_vertex2d),
		(void*)(a3OffsetOf(a3_vertex2d, a3_vertex2d::color))));

	a3GL(glVertexAttribPointer(a3_vertex2d::TEXCOORDS, 2, GL_FLOAT,
		GL_FALSE, sizeof(a3_vertex2d),
		(void*)(a3OffsetOf(a3_vertex2d, a3_vertex2d::texCoords))));

	u32 indices[] = {
		0, 1, 2, 0, 2, 3
	};

	a3GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * 6, indices, GL_STATIC_DRAW));
	a3GL(glEnableVertexAttribArray(a3_vertex2d::POSITION));
	a3GL(glEnableVertexAttribArray(a3_vertex2d::COLOR));
	a3GL(glEnableVertexAttribArray(a3_vertex2d::TEXCOORDS));

	r.m_ShaderProgram = a3_CreateShaderProgramFromBuffer(vSource, fSource);
	a3_BindProgram(r.m_ShaderProgram);
	a3GL(r.m_uProjection = glGetUniformLocation(r.m_ShaderProgram, "u_Projection"));
	a3GL(r.m_uTextureDiffuse = glGetUniformLocation(r.m_ShaderProgram, "u_Diffuse"));

	return r;
}

a3::font_renderer a3_renderer::CreateFontRenderer(s8 vSource, s8 fSource) const
{
	a3::font_renderer r;
	a3_GenerateAndBind(&r.m_VertexArrayObject, &r.m_VertexArrayBuffer, &r.m_ElementArrayBuffer);

	a3GL(glBufferData(GL_ARRAY_BUFFER, sizeof(a3_vertex_font) * A3_VERTICES_FONT_MAX, A3NULL, GL_DYNAMIC_DRAW));
	a3GL(glVertexAttribPointer(a3_vertex_font::POSTEXCOORDS, 4, GL_FLOAT, GL_FALSE, sizeof(a3_vertex_font), (void*)(a3OffsetOf(a3_vertex_font, a3_vertex_font::posTexCoords))));
	a3GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * A3_INDICES_FONT_MAX, A3NULL, GL_DYNAMIC_DRAW));
	a3GL(glEnableVertexAttribArray(a3_vertex_font::POSTEXCOORDS));

	r.m_ShaderProgram = a3_CreateShaderProgramFromBuffer(vSource, fSource);
	a3_BindProgram(r.m_ShaderProgram);
	a3GL(r.m_uProjection = glGetUniformLocation(r.m_ShaderProgram, "u_Projection"));
	a3GL(r.m_uColor = glGetUniformLocation(r.m_ShaderProgram, "u_Color"));
	a3GL(r.m_uFontAtlas = glGetUniformLocation(r.m_ShaderProgram, "u_FontAtlas"));
	a3GL(glUniform1i(r.m_uFontAtlas, s_CurrentBound.FontTextureAtlasSlot));

	return r;
}

a3::batch2d_renderer a3_renderer::CreateBatch2DRenderer(s8 vSource, s8 fSource) const
{
	a3::batch2d_renderer r;
	a3_GenerateAndBind(&r.m_VertexArrayObject, &r.m_VertexArrayBuffer, &r.m_ElementArrayBuffer);

	a3GL(glBufferData(GL_ARRAY_BUFFER, sizeof(a3_vertex_ui) * A3_VERTICES_UI_MAX, A3NULL, GL_DYNAMIC_DRAW));
	a3GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * A3_INDICES_UI_MAX, A3NULL, GL_DYNAMIC_DRAW));
	a3GL(glVertexAttribPointer(a3_vertex_ui::POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(a3_vertex_ui), (void*)(a3OffsetOf(a3_vertex_ui, a3_vertex_ui::position))));
	a3GL(glVertexAttribPointer(a3_vertex_ui::COLOR, 3, GL_FLOAT, GL_FALSE, sizeof(a3_vertex_ui), (void*)(a3OffsetOf(a3_vertex_ui, a3_vertex_ui::color))));
	a3GL(glVertexAttribPointer(a3_vertex_ui::TEXCOORDS, 2, GL_FLOAT, GL_FALSE, sizeof(a3_vertex_ui), (void*)(a3OffsetOf(a3_vertex_ui, a3_vertex_ui::texCoords))));
	a3GL(glEnableVertexAttribArray(a3_vertex_ui::POSITION));
	a3GL(glEnableVertexAttribArray(a3_vertex_ui::COLOR));
	a3GL(glEnableVertexAttribArray(a3_vertex_ui::TEXCOORDS));

	r.m_ShaderProgram = a3_CreateShaderProgramFromBuffer(vSource, fSource);
	a3_BindProgram(r.m_ShaderProgram);
	a3GL(r.m_uProjection = glGetUniformLocation(r.m_ShaderProgram, "u_Projection"));
	a3GL(r.m_uTextureAtlas = glGetUniformLocation(r.m_ShaderProgram, "u_TextureAtlas"));
	a3GL(r.m_uSpotLightPosition = glGetUniformLocation(r.m_ShaderProgram, "u_SpotLightPosition"));
	a3GL(r.m_uSpotLightColor = glGetUniformLocation(r.m_ShaderProgram, "u_SpotLightColor"));
	a3GL(r.m_uSpotLightIntensity = glGetUniformLocation(r.m_ShaderProgram, "u_SpotLightIntensity"));
	a3GL(glUniform1i(r.m_uTextureAtlas, s_CurrentBound.UITextureSlot));
	r.m_Count = 0;

	return r;
}

namespace a3 {

	void basic2d_renderer::SetRegion(f32 left, f32 right, f32 bottom, f32 top)
	{
		SetRegion(m4x4::OrthographicR(left, right, bottom, top, -1.0f, 1.0f));
	}

	void basic2d_renderer::SetRegion(const m4x4& p)
	{
		a3_BindProgram(m_ShaderProgram);
		a3GL(glUniformMatrix4fv(m_uProjection, 1, GL_FALSE, p.elements));
	}

	void basic2d_renderer::BeginFrame()
	{
		a3_BindVertexArrayObject(m_VertexArrayObject);
		a3_BindVertexArrayBuffer(m_VertexArrayBuffer);
		a3_BindProgram(m_ShaderProgram);
	}

	void basic2d_renderer::EndFrame(v3 position, v2 dimension, v3 color[4], a3::texture* texture)
	{
		a3GL(glActiveTexture(GL_TEXTURE0));
		a3GL(glBindTexture(GL_TEXTURE_2D, *texture));
		a3GL(glUniform1i(m_uTextureDiffuse, 0));
		a3_vertex2d v[4];
		v[0].position = position;
		v[1].position = position;
		v[2].position = position;
		v[3].position = position;
		v[1].position.x += dimension.x;
		v[2].position.xy += dimension;
		v[3].position.y += dimension.y;
		v[0].color = color[0];
		v[1].color = color[1];
		v[2].color = color[2];
		v[3].color = color[3];
		v[0].texCoords = { 0.0f, 0.0f };
		v[1].texCoords = { 1.0f, 0.0f };
		v[2].texCoords = { 1.0f, 1.0f };
		v[3].texCoords = { 0.0f, 1.0f };
		a3GL(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(a3_vertex2d) * 4, v));
		a3GL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, A3NULL));
	}

	void font_renderer::SetRegion(f32 left, f32 right, f32 bottom, f32 top)
	{
		SetRegion(m4x4::OrthographicR(left, right, bottom, top, -1.0f, 1.0f));
	}

	void font_renderer::SetRegion(const m4x4& p)
	{
		a3_BindProgram(m_ShaderProgram);
		a3GL(glUniformMatrix4fv(m_uProjection, 1, GL_FALSE, p.elements));
	}

	void font_renderer::SetFont(a3::font_texture * ft)
	{
		m_FontTexture = ft;
	}

	void font_renderer::Render(s8 font, v2 position, f32 height, v3 color)
	{
		a3_BindVertexArrayObject(m_VertexArrayObject);
		a3_BindVertexArrayBuffer(m_VertexArrayBuffer);
		a3_BindElementArrayBuffer(m_ElementArrayBuffer);
		a3_BindProgram(m_ShaderProgram);
		a3GL(glActiveTexture(GL_TEXTURE0 + s_CurrentBound.FontTextureAtlasSlot));
		a3GL(glBindTexture(GL_TEXTURE_2D, m_FontTexture->Texture));
		a3GL(glUniform3fv(m_uColor, 1, color.values));

		u8* t = (u8*)font;
		f32 hBegin = position.x;
		a3_MapVertexPointer();
		a3_MapElementPointer();
		a3_vertex_font* vertices = a3GetMappedVertexPointer(a3_vertex_font);
		u32* indices = a3GetMappedElementPointer();
		f32 scale = height / m_FontTexture->AtlasInfo.HeightInPixels;

		u32 counter = 0;
		for ( ; ; ++t)
		{
			// Flush
			if (counter == A3_FONT_RENDER_MAX || *t == 0)
			{
				a3_UnmapVertexPointer();
				a3_UnmapElementPointer();
				a3GL(glDrawElements(GL_TRIANGLES, counter * 6, GL_UNSIGNED_INT, A3NULL));
				if (*t == 0) break;
				counter = 0;
				a3_MapVertexPointer();
				a3_MapElementPointer();
				a3_vertex_font* vertices = a3GetMappedVertexPointer(a3_vertex_font);
				u32* indices = a3GetMappedElementPointer();
			}

			const a3::character& c = m_FontTexture->AtlasInfo.Characters[*t];
			if (c.HasBitmap)
			{
				f32 x = hBegin + c.OffsetX * scale;
				f32 y = position.y + c.OffsetY * scale;
				f32 w = c.Width * scale;
				f32 h = c.Height * scale;

				f32 tx0 = c.NormalX0;
				f32 tx1 = c.NormalX1;
				f32 ty0 = c.NormalY0;
				f32 ty1 = c.NormalY1;

				vertices[counter * 4 + 0].posTexCoords = { x, y, tx0, ty0 };
				vertices[counter * 4 + 1].posTexCoords = { x + w, y, tx1, ty0 };
				vertices[counter * 4 + 2].posTexCoords = { x + w, y + h, tx1, ty1 };
				vertices[counter * 4 + 3].posTexCoords = { x, y + h, tx0, ty1 };

				indices[counter * 6 + 0] = counter * 4 + 0;
				indices[counter * 6 + 1] = counter * 4 + 1;
				indices[counter * 6 + 2] = counter * 4 + 2;
				indices[counter * 6 + 3] = counter * 4 + 0;
				indices[counter * 6 + 4] = counter * 4 + 2;
				indices[counter * 6 + 5] = counter * 4 + 3;

				counter++;
			}
			hBegin += c.Advance * scale;
			if (*(t + 1))
				hBegin += (a3::GetTTFontKernalAdvance(m_FontTexture->AtlasInfo.Info, m_FontTexture->AtlasInfo.ScalingFactor, c.GlyphIndex, (m_FontTexture->AtlasInfo.Characters[*(t + 1)]).GlyphIndex) * scale);
		}
	}

	void font_renderer::Render(s8 font, v2 c1, v2 c2, f32 height, v3 color)
	{
		a3_BindVertexArrayObject(m_VertexArrayObject);
		a3_BindVertexArrayBuffer(m_VertexArrayBuffer);
		a3_BindElementArrayBuffer(m_ElementArrayBuffer);
		a3_BindProgram(m_ShaderProgram);
		a3GL(glActiveTexture(GL_TEXTURE0 + s_CurrentBound.FontTextureAtlasSlot));
		a3GL(glBindTexture(GL_TEXTURE_2D, m_FontTexture->Texture));
		a3GL(glUniform3fv(m_uColor, 1, color.values));

		u8* t = (u8*)font;
		f32 hBegin = c1.x;
		f32 vBegin = c2.y - height;
		a3_MapVertexPointer();
		a3_MapElementPointer();
		a3_vertex_font* vertices = a3GetMappedVertexPointer(a3_vertex_font);
		u32* indices = a3GetMappedElementPointer();
		f32 scale = height / m_FontTexture->AtlasInfo.HeightInPixels;

		u32 counter = 0;
		for (; ; ++t)
		{
			// Flush
			if (counter == A3_FONT_RENDER_MAX || *t == 0 || vBegin < c1.y)
			{
				a3_UnmapVertexPointer();
				a3_UnmapElementPointer();
				a3GL(glDrawElements(GL_TRIANGLES, counter * 6, GL_UNSIGNED_INT, A3NULL));
				if (*t == 0 || vBegin < c1.y) break;
				counter = 0;
				a3_MapVertexPointer();
				a3_MapElementPointer();
				a3_vertex_font* vertices = a3GetMappedVertexPointer(a3_vertex_font);
				u32* indices = a3GetMappedElementPointer();
			}

			const a3::character& c = m_FontTexture->AtlasInfo.Characters[*t];
			if (c.HasBitmap)
			{
				f32 x = hBegin + c.OffsetX * scale;
				f32 y = vBegin + c.OffsetY * scale;
				f32 w = c.Width * scale;
				f32 h = c.Height * scale;

				f32 tx0 = c.NormalX0;
				f32 tx1 = c.NormalX1;
				f32 ty0 = c.NormalY0;
				f32 ty1 = c.NormalY1;

				vertices[counter * 4 + 0].posTexCoords = { x, y, tx0, ty0 };
				vertices[counter * 4 + 1].posTexCoords = { x + w, y, tx1, ty0 };
				vertices[counter * 4 + 2].posTexCoords = { x + w, y + h, tx1, ty1 };
				vertices[counter * 4 + 3].posTexCoords = { x, y + h, tx0, ty1 };

				indices[counter * 6 + 0] = counter * 4 + 0;
				indices[counter * 6 + 1] = counter * 4 + 1;
				indices[counter * 6 + 2] = counter * 4 + 2;
				indices[counter * 6 + 3] = counter * 4 + 0;
				indices[counter * 6 + 4] = counter * 4 + 2;
				indices[counter * 6 + 5] = counter * 4 + 3;

				counter++;
			}
			hBegin += c.Advance * scale;
			if (*(t + 1))
				hBegin += (a3::GetTTFontKernalAdvance(m_FontTexture->AtlasInfo.Info, m_FontTexture->AtlasInfo.ScalingFactor, c.GlyphIndex, (m_FontTexture->AtlasInfo.Characters[*(t + 1)]).GlyphIndex) * scale);
			if (hBegin > c2.x)
			{
				hBegin = c1.x;
				vBegin -= height;
			}
		}
	}

	void batch2d_renderer::SetRegion(f32 left, f32 right, f32 bottom, f32 top)
	{
		SetRegion(m4x4::OrthographicR(left, right, bottom, top, -1.0f, 1.0f));
	}

	void batch2d_renderer::SetRegion(const m4x4& p)
	{
		a3_BindProgram(m_ShaderProgram);
		a3GL(glUniformMatrix4fv(m_uProjection, 1, GL_FALSE, p.elements));
	}

	void batch2d_renderer::SetTexture(a3::texture * tex)
	{
		m_Texture = tex;
	}

	void batch2d_renderer::SetSpotLightIntensity(f32 intensity)
	{
		a3_BindProgram(m_ShaderProgram);
		a3GL(glUniform1f(m_uSpotLightIntensity, intensity));
	}

	void batch2d_renderer::SetSpotLightColor(v3 color)
	{
		a3_BindProgram(m_ShaderProgram);
		a3GL(glUniform3fv(m_uSpotLightColor, 1, color.values));
	}

	void batch2d_renderer::SetSpotLightPosition(v2 position)
	{
		a3_BindProgram(m_ShaderProgram);
		a3GL(glUniform2fv(m_uSpotLightPosition, 1, position.values));
	}

	void batch2d_renderer::SetSpotLightMaterial(v3 color, f32 intensity)
	{
		a3_BindProgram(m_ShaderProgram);
		a3GL(glUniform3fv(m_uSpotLightColor, 1, color.values));
		a3GL(glUniform1f(m_uSpotLightIntensity, intensity));
	}

	void batch2d_renderer::Push(v2 position, v2 dimension, v3 color, v4 texDimension)
	{
		if (m_Count == A3_UI_RENDER_MAX)
		{
			EndFrame();
			BeginFrame();
		}
		a3_vertex_ui* v = a3GetMappedVertexPointer(a3_vertex_ui);
		u32* i = a3GetMappedElementPointer();

		f32 tx0 = texDimension.x;
		f32 ty0 = texDimension.y;
		f32 tx1 = texDimension.z;
		f32 ty1 = texDimension.w;

		v[m_Count * 4 + 0].position = position;
		v[m_Count * 4 + 1].position = position;
		v[m_Count * 4 + 2].position = position;
		v[m_Count * 4 + 3].position = position;
		v[m_Count * 4 + 1].position.x += dimension.x;
		v[m_Count * 4 + 2].position += dimension;
		v[m_Count * 4 + 3].position.y += dimension.y;
		v[m_Count * 4 + 0].color = color;
		v[m_Count * 4 + 1].color = color;
		v[m_Count * 4 + 2].color = color;
		v[m_Count * 4 + 3].color = color;
		v[m_Count * 4 + 0].texCoords = { tx0, ty0 };
		v[m_Count * 4 + 1].texCoords = { tx1, ty0 };
		v[m_Count * 4 + 2].texCoords = { tx1, ty1 };
		v[m_Count * 4 + 3].texCoords = { tx0, ty1 };

		i[m_Count * 4 + 0] = m_Count * 4 + 0;
		i[m_Count * 4 + 1] = m_Count * 4 + 1;
		i[m_Count * 4 + 2] = m_Count * 4 + 2;
		i[m_Count * 4 + 3] = m_Count * 4 + 0;
		i[m_Count * 4 + 4] = m_Count * 4 + 2;
		i[m_Count * 4 + 5] = m_Count * 4 + 3;

		m_Count++;
	}

	void batch2d_renderer::BeginFrame()
	{
		a3_BindVertexArrayObject(m_VertexArrayObject);
		a3_BindVertexArrayBuffer(m_VertexArrayBuffer);
		a3_BindElementArrayBuffer(m_ElementArrayBuffer);
		a3_MapVertexPointer();
		a3_MapElementPointer();
	}

	void batch2d_renderer::EndFrame()
	{
		a3_UnmapVertexPointer();
		a3_UnmapElementPointer();
		a3_BindProgram(m_ShaderProgram);
		a3GL(glActiveTexture(GL_TEXTURE0 + s_CurrentBound.UITextureSlot));
		a3GL(glBindTexture(GL_TEXTURE_2D, *m_Texture));
		a3GL(glDrawElements(GL_TRIANGLES, m_Count * 6, GL_UNSIGNED_INT, A3NULL));
		m_Count = 0;
	}

}