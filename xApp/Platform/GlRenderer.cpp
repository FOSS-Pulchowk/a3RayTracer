#include "GlRenderer.h"
#include "GL/Glad.h"
#include "GL/GLDebug.h"

#define XMAXRENDERABLES2D 1000
#define XMAXRENDERABLEFONT 1000

namespace x {

	const renderer2d CreateBatchRenderer2D(shader_program program)
	{
		renderer2d r = {};
		xGL(glGenVertexArrays(1, &r.VertexBufferObject));
		xGL(glGenBuffers(1, &r.VertexArrayBuffer));
		xGL(glGenBuffers(1, &r.ElementArrayBuffer));

		xGL(glBindVertexArray(r.VertexBufferObject));
		xGL(glBindBuffer(GL_ARRAY_BUFFER, r.VertexArrayBuffer));
		xGL(glBufferData(GL_ARRAY_BUFFER, sizeof(x_v2d) * XMAXRENDERABLES2D * 4, null, GL_DYNAMIC_DRAW));
		xGL(glVertexAttribPointer(x_v2d::POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(x_v2d), (void*)(xOffsetOf(x_v2d, x_v2d::position))));
		xGL(glVertexAttribPointer(x_v2d::COLOR, 3, GL_FLOAT, GL_FALSE, sizeof(x_v2d), (void*)(xOffsetOf(x_v2d, x_v2d::color))));
		xGL(glVertexAttribPointer(x_v2d::TEXCOORDS, 2, GL_FLOAT, GL_FALSE, sizeof(x_v2d), (void*)(xOffsetOf(x_v2d, x_v2d::texCoords))));

		xGL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r.ElementArrayBuffer));
		xGL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * XMAXRENDERABLES2D * 6, null, GL_DYNAMIC_DRAW));

		xGL(glEnableVertexAttribArray(x_v2d::POSITION));
		xGL(glEnableVertexAttribArray(x_v2d::COLOR));
		xGL(glEnableVertexAttribArray(x_v2d::TEXCOORDS));
		xGL(glBindVertexArray(0));

		r.ShaderProgram = program;

		return r;
	}

	const renderer_font CreateFontRenderer(shader_program program)
	{
		renderer_font r = {};

		xGL(glGenVertexArrays(1, &r.VertexBufferObject));
		xGL(glGenBuffers(1, &r.VertexArrayBuffer));
		xGL(glBindVertexArray(r.VertexBufferObject));
		xGL(glBindBuffer(GL_ARRAY_BUFFER, r.VertexArrayBuffer));
		xGL(glBufferData(GL_ARRAY_BUFFER, sizeof(x_vfont) * XMAXRENDERABLEFONT, null, GL_DYNAMIC_DRAW));
		xGL(glVertexAttribPointer(x_vfont::POSITEXCORDS, 4, GL_FLOAT, GL_FALSE, sizeof(x_vfont), (void*)(xOffsetOf(x_vfont, x_vfont::positionTexCoords))));
		xGL(glEnableVertexAttribArray(x_vfont::POSITEXCORDS));
		xGL(glBindVertexArray(0));

		r.ShaderProgram = program;

		return r;
	}

}