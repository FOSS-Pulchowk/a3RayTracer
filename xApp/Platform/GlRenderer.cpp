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

}