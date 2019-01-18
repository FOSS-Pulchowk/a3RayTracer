#include "GlRenderer.h"
#include "GL/Glad.h"
#include "GL/GLDebug.h"
#include "Platform.h"

#define XMAXRENDERABLES2D 1000
#define XMAXRENDERABLEFONT 1000

struct a3_current_bound
{
	u32 VertexArrayObject;
	u32 VertexArrayBuffer;
	u32 ElementArrayBuffer;
	u32 ShaderProgram;
};

static a3_current_bound s_CurrentBound;

inline void InternalBindVertexArrayObject(u32 target)
{
	//if (target == s_CurrentBound.VertexArrayObject) return;
	a3GL(glBindVertexArray(target));
	s_CurrentBound.VertexArrayObject = target;
}

inline void InternalBindVertexArrayBuffer(u32 target)
{
	//if (target == s_CurrentBound.VertexArrayBuffer) return;
	a3GL(glBindBuffer(GL_ARRAY_BUFFER, target));
	s_CurrentBound.VertexArrayBuffer = target;
}

inline void InternalBindElementArrayBuffer(u32 target)
{
	//if (target == s_CurrentBound.ElementArrayBuffer) return;
	a3GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, target));
	s_CurrentBound.ElementArrayBuffer = target;
}

inline void InternalBindProgram(u32 target)
{
	//if (target == s_CurrentBound.ShaderProgram) return;
	a3GL(glUseProgram(target));
	s_CurrentBound.ShaderProgram = target;
}

inline u32 GLCompileShader(GLenum type, s8 source)
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

inline u32 GLCreateShaderProgramFromBuffer(s8 vSource, s8 fSource)
{
	u32 vShader = GLCompileShader(GL_VERTEX_SHADER, vSource);
	u32 fShader = GLCompileShader(GL_FRAGMENT_SHADER, fSource);

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
		a3GL(a3LogWarn("Shaders could not be linked!\n"));
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
	v3 texCoords;
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

namespace a3 {
	const a3_renderer Renderer;
}

a3::basic2drenderer a3_renderer::Create2DRenderer() const
{
	a3::basic2drenderer r;
	a3GL(glGenVertexArrays(1, &r.m_VertexArrayObject));
	a3GL(glGenBuffers(1, &r.m_VertexArrayBuffer));
	a3GL(glGenBuffers(1, &r.m_ElementArrayBuffer));
	InternalBindVertexArrayObject(r.m_VertexArrayObject);
	InternalBindVertexArrayBuffer(r.m_VertexArrayBuffer);
	InternalBindElementArrayBuffer(r.m_ElementArrayBuffer);

	a3GL(glBufferData(GL_ARRAY_BUFFER, sizeof(a3_vertex2d) * 4, null, GL_DYNAMIC_DRAW));
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

	a3::file_content vertex = a3::Platform.LoadFileContent("Platform/GLSL/Basic2DVertexShader.glsl");
	a3::file_content fragment = a3::Platform.LoadFileContent("Platform/GLSL/Basic2DFragmentShader.glsl");
	a3Assert(vertex.Buffer);
	a3Assert(fragment.Buffer);
	r.m_ShaderProgram = GLCreateShaderProgramFromBuffer((s8)vertex.Buffer, (s8)fragment.Buffer);
	a3::Platform.FreeFileContent(vertex);
	a3::Platform.FreeFileContent(fragment);
	InternalBindProgram(r.m_ShaderProgram);
	a3GL(r.m_Projection = glGetUniformLocation(r.m_ShaderProgram, "u_Projection"));
	a3GL(r.m_TextureDiffuse = glGetUniformLocation(r.m_ShaderProgram, "u_Diffuse"));

	return r;
}

a3::font_renderer a3_renderer::CreateFontRenderer() const
{
	a3::font_renderer r;
	a3GL(glGenVertexArrays(1, &r.m_VertexArrayObject));
	a3GL(glGenBuffers(1, &r.m_VertexArrayBuffer));
	a3GL(glGenBuffers(1, &r.m_ElementArrayBuffer));
	InternalBindVertexArrayObject(r.m_VertexArrayObject);
	InternalBindVertexArrayBuffer(r.m_VertexArrayBuffer);
	InternalBindElementArrayBuffer(r.m_ElementArrayBuffer);

	a3GL(glBufferData(GL_ARRAY_BUFFER, sizeof(a3_vertex_font) * A3_VERTICES_FONT_MAX, null, GL_DYNAMIC_DRAW));
	a3GL(glVertexAttribPointer(a3_vertex_font::POSTEXCOORDS, 4, GL_FLOAT, GL_FALSE, sizeof(a3_vertex_font), (void*)(a3OffsetOf(a3_vertex_font, a3_vertex_font::posTexCoords))));
	a3GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * A3_INDICES_FONT_MAX, null, GL_DYNAMIC_DRAW));
	a3GL(glEnableVertexAttribArray(a3_vertex_font::POSTEXCOORDS));

	a3::file_content vertex = a3::Platform.LoadFileContent("Platform/GLSL/FontVertexShader.glsl");
	a3::file_content fragment = a3::Platform.LoadFileContent("Platform/GLSL/FontFragmentShader.glsl");
	a3Assert(vertex.Buffer);
	a3Assert(fragment.Buffer);
	r.m_ShaderProgram = GLCreateShaderProgramFromBuffer((s8)vertex.Buffer, (s8)fragment.Buffer);
	a3::Platform.FreeFileContent(vertex);
	a3::Platform.FreeFileContent(fragment);
	InternalBindProgram(r.m_ShaderProgram);
	a3GL(r.m_Projection = glGetUniformLocation(r.m_ShaderProgram, "u_Projection"));
	a3GL(r.m_Color = glGetUniformLocation(r.m_ShaderProgram, "u_Color"));
	a3GL(r.m_FontAtlas = glGetUniformLocation(r.m_ShaderProgram, "u_FontAtlas"));

	return r;
}

namespace a3 {

	void basic2drenderer::SetRegion(f32 left, f32 right, f32 bottom, f32 top)
	{
		InternalBindProgram(m_ShaderProgram);
		a3GL(glUniformMatrix4fv(m_Projection, 1, GL_FALSE, m4x4::OrthographicR(left, right, bottom, top, -1.0f, 1.0f).elements));
	}

	void basic2drenderer::Render(v3 position, v2 dimension, v3 color[4], u32 texture)
	{
		InternalBindVertexArrayObject(m_VertexArrayObject);
		InternalBindVertexArrayBuffer(m_VertexArrayBuffer);
		InternalBindProgram(m_ShaderProgram);
		a3GL(glActiveTexture(GL_TEXTURE0));
		a3GL(glBindTexture(GL_TEXTURE_2D, texture));
		a3GL(glUniform1i(m_TextureDiffuse, 0));
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
		a3GL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, null));
	}

	void font_renderer::SetRegion(f32 left, f32 right, f32 bottom, f32 top)
	{
		a3GL(glUniformMatrix4fv(m_Projection, 1, GL_FALSE, m4x4::OrthographicR(left, right, bottom, top, -1.0f, 1.0f).elements));
	}

	void font_renderer::Render(s8 font, v2 position, f32 scale, v3 color, u32 texture, const a3::fonts & f)
	{
		InternalBindVertexArrayObject(m_VertexArrayObject);
		InternalBindVertexArrayBuffer(m_VertexArrayBuffer);
		InternalBindProgram(m_ShaderProgram);
		a3GL(glActiveTexture(GL_TEXTURE0));
		a3GL(glBindTexture(GL_TEXTURE_2D, texture));
		a3GL(glUniform1i(m_FontAtlas, 0));
		a3GL(glUniform3fv(m_Color, 1, color.values));

		u8* t = (u8*)font;
		f32 hBegin = position.x;
		a3GL(a3_vertex_font* vertices = (a3_vertex_font*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
		a3GL(u32* indices = (u32*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));

		u32 counter = 0;
		for ( ; ; ++t)
		{
			// Flush
			if (counter == A3_FONT_RENDER_MAX || *t == 0)
			{
				a3GL(glUnmapBuffer(GL_ARRAY_BUFFER));
				a3GL(glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER));
				a3GL(glDrawElements(GL_TRIANGLES, counter * 6, GL_UNSIGNED_INT, null));
				if (*t == 0) break;
				counter = 0;
				a3GL(vertices = (a3_vertex_font*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
				a3GL(indices = (u32*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));
			}

			const a3::character& c = f.Characters[*t];
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
				hBegin += (a3::GetTTFontKernalAdvance(f, c.GlyphIndex, (f.Characters[*(t + 1)]).GlyphIndex) * scale);
		}
	}

}