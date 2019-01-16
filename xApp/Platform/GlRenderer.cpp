#include "GlRenderer.h"
#include "GL/Glad.h"
#include "GL/GLDebug.h"

#define XMAXRENDERABLES2D 1000
#define XMAXRENDERABLEFONT 1000

namespace a3 {

	const renderer2d CreateBatchRenderer2D(shader_program program)
	{
		renderer2d r = {};
		a3GL(glGenVertexArrays(1, &r.VertexBufferObject));
		a3GL(glGenBuffers(1, &r.VertexArrayBuffer));
		a3GL(glGenBuffers(1, &r.ElementArrayBuffer));

		a3GL(glBindVertexArray(r.VertexBufferObject));
		a3GL(glBindBuffer(GL_ARRAY_BUFFER, r.VertexArrayBuffer));
		a3GL(glBufferData(GL_ARRAY_BUFFER, sizeof(x_v2d) * XMAXRENDERABLES2D * 4, null, GL_DYNAMIC_DRAW));
		a3GL(glVertexAttribPointer(x_v2d::POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(x_v2d), (void*)(a3OffsetOf(x_v2d, x_v2d::position))));
		a3GL(glVertexAttribPointer(x_v2d::COLOR, 3, GL_FLOAT, GL_FALSE, sizeof(x_v2d), (void*)(a3OffsetOf(x_v2d, x_v2d::color))));
		a3GL(glVertexAttribPointer(x_v2d::TEXCOORDS, 2, GL_FLOAT, GL_FALSE, sizeof(x_v2d), (void*)(a3OffsetOf(x_v2d, x_v2d::texCoords))));

		a3GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r.ElementArrayBuffer));
		a3GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * XMAXRENDERABLES2D * 6, null, GL_DYNAMIC_DRAW));

		a3GL(glEnableVertexAttribArray(x_v2d::POSITION));
		a3GL(glEnableVertexAttribArray(x_v2d::COLOR));
		a3GL(glEnableVertexAttribArray(x_v2d::TEXCOORDS));
		a3GL(glBindVertexArray(0));

		r.ShaderProgram = program;

		return r;
	}

	const renderer_font CreateFontRenderer(shader_program program)
	{
		renderer_font r = {};

		a3GL(glGenVertexArrays(1, &r.VertexBufferObject));
		a3GL(glGenBuffers(1, &r.VertexArrayBuffer));
		a3GL(glBindVertexArray(r.VertexBufferObject));
		a3GL(glBindBuffer(GL_ARRAY_BUFFER, r.VertexArrayBuffer));
		a3GL(glBufferData(GL_ARRAY_BUFFER, sizeof(x_vfont) * XMAXRENDERABLEFONT, null, GL_DYNAMIC_DRAW));
		a3GL(glVertexAttribPointer(x_vfont::POSITEXCORDS, 4, GL_FLOAT, GL_FALSE, sizeof(x_vfont), (void*)(a3OffsetOf(x_vfont, x_vfont::positionTexCoords))));
		a3GL(glEnableVertexAttribArray(x_vfont::POSITEXCORDS));
		a3GL(glBindVertexArray(0));

		r.ShaderProgram = program;

		return r;
	}

	void RenderFont(const renderer_font& renderer, s8 string, const gl_textures& texts, v2 position, v3 color, f32 scale)
	{
		a3GL(glBindVertexArray(renderer.VertexBufferObject));
		a3GL(glBindBuffer(GL_ARRAY_BUFFER, renderer.VertexArrayBuffer));
		a3GL(glUseProgram(renderer.ShaderProgram));
		// TODO(Zero): Get all locations when loading shader
		a3GL(u32 loc = glGetUniformLocation(renderer.ShaderProgram, "u_Projection"));
		a3GL(glUniformMatrix4fv(loc, 1, GL_FALSE, renderer.Projection.elements));
		a3GL(loc = glGetUniformLocation(renderer.ShaderProgram, "u_Color"));
		a3GL(glUniform3fv(loc, 1, color.values));
		a3GL(loc = glGetUniformLocation(renderer.ShaderProgram, "u_Texture"));
		a3GL(glUniform1i(loc, 0));
		a3GL(glActiveTexture(GL_TEXTURE0));

		f32 startX = position.x;
		for (i32 ch = 0; string[ch] != 0; ++ch)
		{
			const a3::character& c = texts.font->Characters[string[ch]];
			if (c.HasBitmap)
			{
				a3GL(glBindTexture(GL_TEXTURE_2D, texts.textures[string[ch]]));

				f32 w = c.Bitmap.Width * scale;
				f32 h = c.Bitmap.Height * scale;
				f32 x = startX + c.OffsetX * scale;
				f32 y = position.y + c.OffsetY * scale;

				a3GL(x_vfont* vertices = (x_vfont*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
				vertices[0].positionTexCoords = { x, y, 0.0f, 0.0f };
				vertices[1].positionTexCoords = { x, y + h, 0.0f, 1.0f };
				vertices[2].positionTexCoords = { x + w, y + h, 1.0f, 1.0f };
				vertices[3].positionTexCoords = { x, y, 0.0f, 0.0f };
				vertices[4].positionTexCoords = { x + w, y + h, 1.0f, 1.0f };
				vertices[5].positionTexCoords = { x + w, y, 1.0f, 0.0f };
				a3GL(glUnmapBuffer(GL_ARRAY_BUFFER));
				a3GL(glDrawArrays(GL_TRIANGLES, 0, 6));
			}
			startX += c.Advance * scale;
			if(string[ch+1])
				startX += (a3::GetTTFontKernalAdvance(*texts.font, c.GlyphIndex, (&c + 1)->GlyphIndex) * scale);
		}
	}

}