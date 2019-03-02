#pragma once
#include "Common/Core.h"
#include "Math/Math.h"
#include "Platform/Platform.h"
#include "Utility/Algorithm.h"

//
// DECLARATIONS
//

namespace a3 {

	enum render_type
	{
		RenderTriangle,
		RenderShade,
		RenderShadeWithOutline,
		RenderMapTexture
	};

	struct swapchain
	{
	private:
		m4x4 m_Projection;
		m4x4 m_View;
		rect m_Viewport;
		mesh* m_Meshes;
		image* m_Texture;
		image* m_FrameBuffer;
		f32* m_DepthBuffer;
		b32 m_DrawNormals;

		struct polygon
		{
			v4 vertices[10];
			i32 numVertices;
		};
		void ClipPolygonOnWAxis(polygon* face);
		void ClipPolygonForAxis(polygon* face, i32 comp);
		void ClipPolygon(polygon* face);

	public:
		swapchain();
		void SetProjection(const m4x4& proj);
		void SetProjection(f32 fov, f32 aspectRatio, f32 cNear, f32 cFar);
		void SetView(const m4x4& view);
		void SetView(v3 from, v3 to);
		void SetCamera(const m4x4& camera);
		void SetViewport(i32 x, i32 y, i32 w, i32 h);
		void SetMesh(mesh* meshCube);
		void SetTexture(image* tex);
		void SetFrameBuffer(image* tex);
		void SetDrawNormals(b32 normals);
		void Clear();
		void Render(const m4x4& model, render_type type, const v3& shade = a3::color::White, const v3& outline = a3::color::Yellow);
	private:
		void TextureTriangle(i32 x, i32 y, v2 t1, f32 w1, i32 x2, i32 y2, v2 t2, f32 w2, i32 x3, i32 y3, v2 t3, f32 w3);
		void ShadeTriangle(i32 x, i32 y, f32 w1, i32 x2, i32 y2, f32 w2, i32 x3, i32 y3, f32 w3, const v3& shade);
	};

}


//
// IMPLEMENTATION
//


namespace a3 {

	void swapchain::ClipPolygonOnWAxis(polygon* face)
	{
		f32 wPlane = 0.00001f;

		v4* currentVertice;
		v4* previousVertice;

		i32 insideNumVertices = 0;
		v4 insideVertices[10];

		i32 previousDot;
		i32 currentDot;

		f32 intersectionFactor;
		v4 intersectionPoint;


		previousVertice = &face->vertices[face->numVertices - 1];
		previousDot = (previousVertice->w < wPlane) ? -1 : 1;
		currentVertice = &face->vertices[0];
		while (currentVertice != &face->vertices[face->numVertices])
		{
			currentDot = (currentVertice->w < wPlane) ? -1 : 1;

			if (previousDot * currentDot < 0)
			{
				intersectionFactor = (wPlane - previousVertice->w) / (previousVertice->w - currentVertice->w);

				intersectionPoint = *currentVertice;
				intersectionPoint -= *previousVertice;
				intersectionPoint *= intersectionFactor;
				intersectionPoint += *previousVertice;

				insideVertices[insideNumVertices] = intersectionPoint;
				insideNumVertices++;
			}

			if (currentDot > 0)
			{
				insideVertices[insideNumVertices] = *currentVertice;
				insideNumVertices++;
			}

			previousDot = currentDot;
			previousVertice = currentVertice;
			currentVertice++;
		}

		for (i32 i = 0; i < insideNumVertices; ++i)
			face->vertices[i] = insideVertices[i];
		face->numVertices = insideNumVertices;
		insideNumVertices = 0;
	}

	void swapchain::ClipPolygonForAxis(polygon* face, i32 comp)
	{
		v4* currentVertice;
		v4* previousVertice;

		i32 insideNumVertices = 0;
		v4 insideVertices[10];

		i32 previousDot;
		i32 currentDot;

		f32 intersectionFactor;
		v4 intersectionPoint;

		previousVertice = &face->vertices[face->numVertices - 1];
		previousDot = (previousVertice->values[comp] <= previousVertice->w) ? 1 : -1;
		currentVertice = &face->vertices[0];
		while (currentVertice != &face->vertices[face->numVertices])
		{
			currentDot = (currentVertice->values[comp] <= currentVertice->w) ? 1 : -1;

			if (previousDot * currentDot < 0)
			{
				intersectionFactor =
					(previousVertice->w - previousVertice->values[comp]) /
					((previousVertice->w - previousVertice->values[comp]) - (currentVertice->w - currentVertice->values[comp]));

				intersectionPoint = *currentVertice;
				intersectionPoint -= *previousVertice;
				intersectionPoint *= intersectionFactor;
				intersectionPoint += *previousVertice;

				insideVertices[insideNumVertices] = intersectionPoint;
				insideNumVertices++;
			}

			if (currentDot > 0)
			{
				insideVertices[insideNumVertices] = *currentVertice;
				insideNumVertices++;
			}

			previousDot = currentDot;
			previousVertice = currentVertice;
			currentVertice++;
		}

		for (i32 i = 0; i < insideNumVertices; ++i)
			face->vertices[i] = insideVertices[i];
		face->numVertices = insideNumVertices;
		insideNumVertices = 0;

		previousVertice = &face->vertices[face->numVertices - 1];
		previousDot = ((-previousVertice->values[comp]) <= previousVertice->w) ? 1 : -1;
		currentVertice = &face->vertices[0];
		while (currentVertice != &face->vertices[face->numVertices])
		{
			currentDot = (-(currentVertice->values[comp]) <= currentVertice->w) ? 1 : -1;

			if (previousDot * currentDot < 0)
			{
				intersectionFactor =
					(previousVertice->w + previousVertice->values[comp]) /
					((previousVertice->w + previousVertice->values[comp]) - (currentVertice->w + currentVertice->values[comp]));

				intersectionPoint = *currentVertice;
				intersectionPoint -= *previousVertice;
				intersectionPoint *= intersectionFactor;
				intersectionPoint += *previousVertice;

				insideVertices[insideNumVertices] = intersectionPoint;
				insideNumVertices++;
			}

			if (currentDot > 0)
			{
				insideVertices[insideNumVertices] = *currentVertice;
				insideNumVertices++;
			}

			previousDot = currentDot;

			previousVertice = currentVertice;
			currentVertice++;
		}

		for (i32 i = 0; i < insideNumVertices; ++i)
			face->vertices[i] = insideVertices[i];
		face->numVertices = insideNumVertices;
		insideNumVertices = 0;
	}

	void swapchain::ClipPolygon(polygon* face)
	{
		ClipPolygonOnWAxis(face);		// w
		ClipPolygonForAxis(face, 0);	// x
		ClipPolygonForAxis(face, 1);	// y
		ClipPolygonForAxis(face, 2);	// z
	}

	swapchain::swapchain()
	{
		m_FrameBuffer = A3NULL;
		m_DepthBuffer = A3NULL;
		m_Texture = A3NULL;
		m_DrawNormals = false;
		m_Projection = m4x4::PerspectiveR(a3ToRadians(90.0f), a3AspectRatio(), 0.1f, 1000.0f);
		m_Viewport = { 0,0,1280,720 };
	}

	void swapchain::SetProjection(const m4x4 & proj)
	{
		m_Projection = proj;
	}

	void swapchain::SetProjection(f32 fov, f32 aspectRatio, f32 cNear, f32 cFar)
	{
		m_Projection = m4x4::PerspectiveR(fov, aspectRatio, cNear, cFar);
	}

	void swapchain::SetView(const m4x4 & view)
	{
		m_View = view;
	}

	void swapchain::SetView(v3 from, v3 to)
	{
		m_View = m4x4::Inverse(m4x4::LookR(from, to));
	}

	void swapchain::SetCamera(const m4x4 & camera)
	{
		m_View = m4x4::Inverse(camera);
	}

	void swapchain::SetViewport(i32 x, i32 y, i32 w, i32 h)
	{
		m_Viewport.x = x;
		m_Viewport.y = y;
		m_Viewport.w = w;
		m_Viewport.h = h;
	}

	void swapchain::SetMesh(mesh * meshObj)
	{
		a3Assert(meshObj);
		m_Meshes = meshObj;
	}

	inline void swapchain::SetTexture(image * tex)
	{
		m_Texture = tex;
	}

	void swapchain::SetFrameBuffer(image * tex)
	{
		m_FrameBuffer = tex;
		m_DepthBuffer = a3Reallocate(m_DepthBuffer, sizeof(f32)*tex->Width*tex->Height, f32);
	}

	void swapchain::SetDrawNormals(b32 normals)
	{
		m_DrawNormals = normals;
	}

	void swapchain::Clear()
	{
		a3::FillImageBuffer(m_FrameBuffer, a3::color::Black);
		for (i32 i = 0; i < m_FrameBuffer->Width*m_FrameBuffer->Height; ++i)
			m_DepthBuffer[i] = 0.0f;
	}

	void swapchain::Render(const m4x4& model, render_type type, const v3& shade, const v3& outline)
	{
		a3Assert(m_FrameBuffer);

		m4x4 mvp = model * m_View * m_Projection;

		u32 nTriangles = m_Meshes->NumOfTriangles;
		v3* vertices = m_Meshes->Vertices;
		u32* indices = m_Meshes->VertexIndices;
		v2* textures = m_Meshes->TextureCoords;
		u32* tindices = m_Meshes->TextureCoordsIndices;

		if (!textures)
		{
			if (type == a3::RenderMapTexture) type = a3::RenderShade;
		}

		for (u32 nTri = 0; nTri < nTriangles; ++nTri)
		{
			const v3& p0 = vertices[indices[nTri * 3 + 0]];
			const v3& p1 = vertices[indices[nTri * 3 + 1]];
			const v3& p2 = vertices[indices[nTri * 3 + 2]];

			v2 t0 = {}, t1 = {}, t2 = {};

			if (textures)
			{
				if (tindices)
				{
					t0 = textures[tindices[nTri * 3] + 0];
					t1 = textures[tindices[nTri * 3] + 1];
					t2 = textures[tindices[nTri * 3] + 2];
				}
				else
				{
					t0 = textures[nTri * 3 + 0];
					t1 = textures[nTri * 3 + 1];
					t2 = textures[nTri * 3 + 2];
				}
			}

			polygon triangle;
			triangle.numVertices = 3;
			triangle.vertices[0] = v4{ p0.x, p0.y, p0.z, 1.0f } *mvp;
			triangle.vertices[1] = v4{ p1.x, p1.y, p1.z, 1.0f } *mvp;
			triangle.vertices[2] = v4{ p2.x, p2.y, p2.z, 1.0f } *mvp;

			// NOTE(Zero): 
			// Since camera is at (0,0,0) and poi32ing towards z direction
			// the z component from result of Cross product gives the dot product
			v3 normal = Normalize(Cross(triangle.vertices[1].xyz - triangle.vertices[0].xyz, triangle.vertices[2].xyz - triangle.vertices[1].xyz));
			f32 dot = normal.z;

			if (dot > 0.0f)
			{
				if (textures && type == a3::RenderMapTexture)
				{
				}
				else
				{
					ClipPolygon(&triangle);
				}

				if (triangle.numVertices > 0)
				{
					v2 finalPoint0;
					finalPoint0.x = triangle.vertices[0].x / triangle.vertices[0].w;
					finalPoint0.y = triangle.vertices[0].y / triangle.vertices[0].w;

					finalPoint0.x = 0.5f * (finalPoint0.x + 1.0f) * (m_FrameBuffer->Width - 1);
					finalPoint0.y = ((finalPoint0.y + 1.0f) * 0.5f) * (m_FrameBuffer->Height - 1);

					f32 w0 = 1.0f / triangle.vertices[0].w;

					//v2 finalUV0;

					if (textures && type == a3::RenderMapTexture)
					{
					}

					for (i32 n = 1; n < triangle.numVertices - 1; ++n)
					{
						v2 finalPoint1;
						v2 finalPoint2;

						finalPoint1.x = triangle.vertices[n + 0].x / triangle.vertices[n + 0].w;
						finalPoint1.y = triangle.vertices[n + 0].y / triangle.vertices[n + 0].w;

						finalPoint2.x = triangle.vertices[n + 1].x / triangle.vertices[n + 1].w;
						finalPoint2.y = triangle.vertices[n + 1].y / triangle.vertices[n + 1].w;

						finalPoint1.x = 0.5f * (finalPoint1.x + 1.0f) * (m_FrameBuffer->Width - 1);
						finalPoint1.y = ((finalPoint1.y + 1.0f) * 0.5f) * (m_FrameBuffer->Height - 1);

						finalPoint2.x = 0.5f * (finalPoint2.x + 1.0f) * (m_FrameBuffer->Width - 1);
						finalPoint2.y = ((finalPoint2.y + 1.0f) * 0.5f) * (m_FrameBuffer->Height - 1);

						f32 w1 = 1.0f / triangle.vertices[n + 0].w;
						f32 w2 = 1.0f / triangle.vertices[n + 1].w;

						if (type == a3::RenderTriangle)
						{
							a3::DrawTriangle(m_FrameBuffer, finalPoint0, finalPoint1, finalPoint2, outline);
						}
						else if (type == a3::RenderShade)
						{
							ShadeTriangle((i32)finalPoint0.x, (i32)finalPoint0.y, w0, (i32)finalPoint1.x, (i32)finalPoint1.y, w1, (i32)finalPoint2.x, (i32)finalPoint2.y, w2, shade * dot);
						}
						else if (type == a3::RenderShadeWithOutline)
						{
							ShadeTriangle((i32)finalPoint0.x, (i32)finalPoint0.y, w0, (i32)finalPoint1.x, (i32)finalPoint1.y, w1, (i32)finalPoint2.x, (i32)finalPoint2.y, w2, shade * dot);
							a3::DrawTriangle(m_FrameBuffer, finalPoint0, finalPoint1, finalPoint2, outline);
						}
						else
						{

							//v2 finalUV1;
							//v2 finalUV2;
							//finalUV1.u = clippedTextureCoords[n].u / clippedVertices[n].w;
							//finalUV2.u = clippedTextureCoords[n + 1].u / clippedVertices[n + 1].w;
							//finalUV1.v = clippedTextureCoords[n].v / clippedVertices[n].w;
							//finalUV2.v = clippedTextureCoords[n + 1].v / clippedVertices[n + 1].w;
							//TextureTriangle((i32)finalPoint0.x, (i32)finalPoint0.y, finalUV0, w0, (i32)finalPoint1.x, (i32)finalPoint1.y, finalUV1, w1, (i32)finalPoint2.x, (i32)finalPoint2.y, finalUV2, w2);
						}

						if (m_DrawNormals)
						{
							v2 centroid{ (finalPoint0.x + finalPoint1.x + finalPoint2.x) / 3.0f, (finalPoint0.y + finalPoint1.y + finalPoint2.y) / 3.0f };
							a3::DrawLine(m_FrameBuffer, centroid, centroid + 30.0f * normal.xy, a3::color::Yellow);
						}
					}
				}

			}
		}
	}

	void swapchain::TextureTriangle(i32 x1, i32 y1, v2 t1, f32 w1, i32 x2, i32 y2, v2 t2, f32 w2, i32 x3, i32 y3, v2 t3, f32 w3)
	{
		if (y2 < y1)
		{
			a3::Swap(&y1, &y2);
			a3::Swap(&x1, &x2);
			a3::Swap(&t1, &t2);
			a3::Swap(&w1, &w2);
		}

		if (y3 < y1)
		{
			a3::Swap(&y1, &y3);
			a3::Swap(&x1, &x3);
			a3::Swap(&t1, &t3);
			a3::Swap(&w1, &w3);
		}

		if (y3 < y2)
		{
			a3::Swap(&y2, &y3);
			a3::Swap(&x2, &x3);
			a3::Swap(&t2, &t3);
			a3::Swap(&w2, &w3);
		}

		i32 dy1 = y2 - y1;
		i32 dx1 = x2 - x1;
		v2 dt1 = t2 - t1;
		f32 dw1 = w2 - w1;

		i32 dy2 = y3 - y1;
		i32 dx2 = x3 - x1;
		v2 dt2 = t3 - t1;
		f32 dw2 = w3 - w1;

		v2 tex_uv;
		f32 tex_w;

		f32 dax_step = 0.0f, dbx_step = 0.0f;
		v2 dt1_step = {};
		v2 dt2_step = {};
		f32 dw1_step = 0.0f, dw2_step = 0.0f;

		if (dy1) dax_step = dx1 / (f32)Abs(dy1);
		if (dy2) dbx_step = dx2 / (f32)Abs(dy2);

		if (dy1) dt1_step = dt1 * (1.0f / (f32)Abs(dy1));
		if (dy1) dw1_step = dw1 / (f32)Abs(dy1);

		if (dy2) dt2_step = dt2 * (1.0f / (f32)Abs(dy2));
		if (dy2) dw2_step = dw2 / (f32)Abs(dy2);

		if (dy1)
		{
			for (i32 i = y1; i <= y2; i++)
			{
				i32 ax = (i32)(x1 + (f32)(i - y1) * dax_step);
				i32 bx = (i32)(x1 + (f32)(i - y1) * dbx_step);

				v2 tex_suv = t1 + (f32)(i - y1) * dt1_step;
				f32 tex_sw = w1 + (f32)(i - y1) * dw1_step;

				v2 tex_euv = t1 + (f32)(i - y1) * dt2_step;
				f32 tex_ew = w1 + (f32)(i - y1) * dw2_step;

				if (ax > bx)
				{
					a3::Swap(&ax, &bx);
					a3::Swap(&tex_suv, &tex_euv);
					a3::Swap(&tex_sw, &tex_ew);
				}

				tex_uv = tex_suv;
				tex_w = tex_sw;

				f32 tstep = 1.0f / ((f32)(bx - ax));
				f32 t = 0.0f;

				for (i32 j = ax; j < bx; j++)
				{
					tex_uv = (1.0f - t) * tex_suv + t * tex_euv;
					tex_w = (1.0f - t) * tex_sw + t * tex_ew;
					if (tex_w > m_DepthBuffer[i*m_FrameBuffer->Width + j])
					{
						v2 fuv = tex_uv * (1.0f / tex_w);
						a3::SetPixelColor(m_FrameBuffer, (f32)j + 0.5f, (f32)i + 0.5f, a3::SamplePixelColor(m_Texture, fuv));
						m_DepthBuffer[i*m_FrameBuffer->Width + j] = tex_w;
					}
					t += tstep;
				}

			}
		}

		dy1 = y3 - y2;
		dx1 = x3 - x2;
		dt1 = t3 - t2;
		dw1 = w3 - w2;

		if (dy1) dax_step = dx1 / (f32)Abs(dy1);
		if (dy2) dbx_step = dx2 / (f32)Abs(dy2);

		v2 dut_step = {};
		if (dy1) dut_step = dt1 * (1.0f / (f32)Abs(dy1));
		if (dy1) dw1_step = dw1 / (f32)Abs(dy1);

		if (dy1)
		{
			for (i32 i = y2; i <= y3; i++)
			{
				i32 ax = (i32)(x2 + (f32)(i - y2) * dax_step);
				i32 bx = (i32)(x1 + (f32)(i - y1) * dbx_step);

				v2 tex_suv = t2 + (f32)(i - y2) * dt1_step;
				f32 tex_sw = w2 + (f32)(i - y2) * dw1_step;

				v2 tex_euv = t1 + (f32)(i - y1) * dt2_step;
				f32 tex_ew = w1 + (f32)(i - y1) * dw2_step;

				if (ax > bx)
				{
					a3::Swap(&ax, &bx);
					a3::Swap(&tex_suv, &tex_euv);
					a3::Swap(&tex_sw, &tex_ew);
				}

				tex_uv = tex_suv;
				tex_w = tex_sw;

				f32 tstep = 1.0f / ((f32)(bx - ax));
				f32 t = 0.0f;

				for (i32 j = ax; j < bx; j++)
				{
					tex_uv = (1.0f - t) * tex_suv + t * tex_euv;
					tex_w = (1.0f - t) * tex_sw + t * tex_ew;

					if (tex_w > m_DepthBuffer[i*m_FrameBuffer->Width + j])
					{
						v2 fuv = tex_uv * (1.0f / tex_w);
						a3Assert(fuv.u >= 0.0f && fuv.y >= 0.0f);
						a3::SetPixelColor(m_FrameBuffer, (f32)j + 0.5f, (f32)i + 0.5f, a3::SamplePixelColor(m_Texture, fuv));
						m_DepthBuffer[i*m_FrameBuffer->Width + j] = tex_w;
					}
					t += tstep;
				}
			}
		}
	}

	void swapchain::ShadeTriangle(i32 x1, i32 y1, f32 w1, i32 x2, i32 y2, f32 w2, i32 x3, i32 y3, f32 w3, const v3& shade)
	{
		if (y2 < y1)
		{
			a3::Swap(&y1, &y2);
			a3::Swap(&x1, &x2);
			a3::Swap(&w1, &w2);
		}

		if (y3 < y1)
		{
			a3::Swap(&y1, &y3);
			a3::Swap(&x1, &x3);
			a3::Swap(&w1, &w3);
		}

		if (y3 < y2)
		{
			a3::Swap(&y2, &y3);
			a3::Swap(&x2, &x3);
			a3::Swap(&w2, &w3);
		}

		i32 dy1 = y2 - y1;
		i32 dx1 = x2 - x1;
		f32 dw1 = w2 - w1;

		i32 dy2 = y3 - y1;
		i32 dx2 = x3 - x1;
		f32 dw2 = w3 - w1;

		f32 tex_w;

		f32 dax_step = 0.0f, dbx_step = 0.0f;
		v2 dt1_step = {};
		v2 dt2_step = {};
		f32 dw1_step = 0.0f, dw2_step = 0.0f;

		if (dy1) dax_step = dx1 / (f32)Abs(dy1);
		if (dy2) dbx_step = dx2 / (f32)Abs(dy2);

		if (dy1) dw1_step = dw1 / (f32)Abs(dy1);

		if (dy2) dw2_step = dw2 / (f32)Abs(dy2);

		if (dy1)
		{
			for (i32 i = y1; i <= y2; i++)
			{
				i32 ax = (i32)(x1 + (f32)(i - y1) * dax_step);
				i32 bx = (i32)(x1 + (f32)(i - y1) * dbx_step);

				f32 tex_sw = w1 + (f32)(i - y1) * dw1_step;

				f32 tex_ew = w1 + (f32)(i - y1) * dw2_step;

				if (ax > bx)
				{
					a3::Swap(&ax, &bx);
					a3::Swap(&tex_sw, &tex_ew);
				}

				tex_w = tex_sw;

				f32 tstep = 1.0f / ((f32)(bx - ax));
				f32 t = 0.0f;

				for (i32 j = ax; j < bx; j++)
				{
					tex_w = (1.0f - t) * tex_sw + t * tex_ew;
					if (tex_w > m_DepthBuffer[i*m_FrameBuffer->Width + j])
					{
						a3::SetPixelColor(m_FrameBuffer, (f32)j + 0.5f, (f32)i + 0.5f, shade);
						m_DepthBuffer[i*m_FrameBuffer->Width + j] = tex_w;
					}
					t += tstep;
				}

			}
		}

		dy1 = y3 - y2;
		dx1 = x3 - x2;
		dw1 = w3 - w2;

		if (dy1) dax_step = dx1 / (f32)Abs(dy1);
		if (dy2) dbx_step = dx2 / (f32)Abs(dy2);

		v2 dut_step = {};
		if (dy1) dw1_step = dw1 / (f32)Abs(dy1);

		if (dy1)
		{
			for (i32 i = y2; i <= y3; i++)
			{
				i32 ax = (i32)(x2 + (f32)(i - y2) * dax_step);
				i32 bx = (i32)(x1 + (f32)(i - y1) * dbx_step);

				f32 tex_sw = w2 + (f32)(i - y2) * dw1_step;

				f32 tex_ew = w1 + (f32)(i - y1) * dw2_step;

				if (ax > bx)
				{
					a3::Swap(&ax, &bx);
					a3::Swap(&tex_sw, &tex_ew);
				}

				tex_w = tex_sw;

				f32 tstep = 1.0f / ((f32)(bx - ax));
				f32 t = 0.0f;

				for (i32 j = ax; j < bx; j++)
				{
					tex_w = (1.0f - t) * tex_sw + t * tex_ew;

					if (tex_w > m_DepthBuffer[i*m_FrameBuffer->Width + j])
					{
						a3::SetPixelColor(m_FrameBuffer, (f32)j + 0.5f, (f32)i + 0.5f, shade);
						m_DepthBuffer[i*m_FrameBuffer->Width + j] = tex_w;
					}
					t += tstep;
				}
			}
		}
	}

}


