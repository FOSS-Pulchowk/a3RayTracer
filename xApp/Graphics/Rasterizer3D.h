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

	void swapchain::SetMesh(mesh * meshCube)
	{
		m_Meshes = meshCube;
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

			v4 clippedVertices[6 * 3];
			i32 nVertices = 3;

			v2 clippedTextureCoords[6 * 3];
			i32 nTextureCoords = 3;

			clippedVertices[0] = v4{ p0.x, p0.y, p0.z, 1.0f } *mvp;
			clippedVertices[1] = v4{ p1.x, p1.y, p1.z, 1.0f } *mvp;
			clippedVertices[2] = v4{ p2.x, p2.y, p2.z, 1.0f } *mvp;

			clippedTextureCoords[0] = v2{ t0.u, t0.v };
			clippedTextureCoords[1] = v2{ t1.u, t1.v };
			clippedTextureCoords[2] = v2{ t2.u, t2.v };

			v4 auxVertices[6 * 3];
			i32 nAuxVertices = 0;

			v2 auxTextureCoords[6 * 3];
			i32 nAuxTextureCoords = 0;

			auto clipOnWAxis = [](v4* vertices, i32* nVertices, v4* resVertices, i32* nResVertices)
			{
				f32 wPlane = 0.0f;

				for (i32 cnt = 0; cnt < *nVertices; ++cnt)
				{
					const v4& ver0 = vertices[cnt];
					const v4& ver1 = vertices[(cnt + 1) % *nVertices];
					const f32& comp0 = ver0.w;
					const f32& comp1 = ver1.w;

					b32 in0 = (comp0 > wPlane);
					b32 in1 = (comp1 > wPlane);

					if (in0 ^ in1)
					{
						f32 lerpAmt = (wPlane - comp1) / (comp0 - comp1);
						resVertices[*nResVertices] = Lerp(ver1, ver0, lerpAmt);
						*nResVertices = *nResVertices + 1;
					}

					if (in0)
					{
						resVertices[*nResVertices] = ver0;
						*nResVertices = *nResVertices + 1;
					}
				}
				*nVertices = 0;
			};

			auto clipComponentVertices = [](i32 comp, v4* vertices, i32* nVertices, v4* resVertices, i32* nResVertices, f32 sign)
			{
				for (i32 cnt = 0; cnt < *nVertices; ++cnt)
				{
					const v4& ver0 = vertices[cnt];
					const v4& ver1 = vertices[(cnt + 1) % *nVertices];
					const f32& comp0 = sign * ver0.values[comp];
					const f32& comp1 = sign * ver1.values[comp];

					b32 in0 = (comp0 <= ver0.w);
					b32 in1 = (comp1 <= ver1.w);

					if (in0 ^ in1)
					{
						f32 lerpAmt = (ver1.w - comp1) / ((ver1.w - comp1) - (ver0.w - comp0));
						resVertices[*nResVertices] = Lerp(ver1, ver0, lerpAmt);
						*nResVertices = *nResVertices + 1;
					}

					if (in0)
					{
						resVertices[*nResVertices] = ver0;
						*nResVertices = *nResVertices + 1;
					}
				}
				*nVertices = 0;
			};

			auto clipComponentVerticesWithUV = [](
				i32 comp,
				v4* vertices, v2* textureCoords, i32* nVertices, i32* nTextureCoords,
				v4* resVertices, v2* resTextureCoords, i32* nResVertices, i32* nResTextureCoords,
				f32 sign)
			{
				for (i32 cnt = 0; cnt < *nVertices; ++cnt)
				{
					const v4& ver0 = vertices[cnt];
					const v4& ver1 = vertices[(cnt + 1) % *nVertices];

					const v2& tex0 = textureCoords[cnt];
					const v2& tex1 = textureCoords[(cnt + 1) % *nTextureCoords];

					const f32& comp0 = sign * ver0.values[comp];
					const f32& comp1 = sign * ver1.values[comp];

					b32 in0 = (comp0 <= ver0.w);
					b32 in1 = (comp1 <= ver1.w);

					if (in0 ^ in1)
					{
						f32 lerpAmt = (ver1.w - comp1) / ((ver1.w - comp1) - (ver0.w - comp0));
						resVertices[*nResVertices] = Lerp(ver1, ver0, lerpAmt);
						resTextureCoords[*nResTextureCoords] = Lerp(tex1, tex0, lerpAmt);
						*nResVertices = *nResVertices + 1;
						*nResTextureCoords = *nResTextureCoords + 1;
					}

					if (in0)
					{
						resVertices[*nResVertices] = ver0;
						resTextureCoords[*nResTextureCoords] = tex0;
						*nResVertices = *nResVertices + 1;
						*nResTextureCoords = *nResTextureCoords + 1;
					}
				}
				*nVertices = 0;
				*nTextureCoords = 0;
			};


			// NOTE(Zero): 
			// Since camera is at (0,0,0) and poi32ing towards z direction
			// the z component from result of Cross product gives the dot product
			v3 normal = Normalize(Cross(clippedVertices[1].xyz - clippedVertices[0].xyz, clippedVertices[2].xyz - clippedVertices[1].xyz));
			f32 dot = normal.z;

			if (dot > 0.0f)
			{
				//polygon poly;
				//poly.numVertices = 3;
				//poly.vertices[0] = clippedVertices[0];
				//poly.vertices[1] = clippedVertices[1];
				//poly.vertices[2] = clippedVertices[2];

				if (textures && type == a3::RenderMapTexture)
				{
					clipComponentVerticesWithUV(0, clippedVertices, clippedTextureCoords, &nVertices, &nTextureCoords, auxVertices, auxTextureCoords, &nAuxVertices, &nAuxTextureCoords, 1.0f);	// x <= w
					clipComponentVerticesWithUV(0, auxVertices, auxTextureCoords, &nAuxVertices, &nAuxTextureCoords, clippedVertices, clippedTextureCoords, &nVertices, &nTextureCoords, -1.0f);	// x <= w
					clipComponentVerticesWithUV(1, clippedVertices, clippedTextureCoords, &nVertices, &nTextureCoords, auxVertices, auxTextureCoords, &nAuxVertices, &nAuxTextureCoords, 1.0f);	// x <= w
					clipComponentVerticesWithUV(1, auxVertices, auxTextureCoords, &nAuxVertices, &nAuxTextureCoords, clippedVertices, clippedTextureCoords, &nVertices, &nTextureCoords, -1.0f);	// x <= w
					clipComponentVerticesWithUV(2, clippedVertices, clippedTextureCoords, &nVertices, &nTextureCoords, auxVertices, auxTextureCoords, &nAuxVertices, &nAuxTextureCoords, 1.0f);	// x <= w
					clipComponentVerticesWithUV(2, auxVertices, auxTextureCoords, &nAuxVertices, &nAuxTextureCoords, clippedVertices, clippedTextureCoords, &nVertices, &nTextureCoords, -1.0f);	// x <= w
				}
				else
				{
					//clipOnWAxis(clippedVertices, &nVertices, auxVertices, &nAuxVertices);
					clipComponentVertices(0, clippedVertices, &nVertices, auxVertices, &nAuxVertices, 1.0f);	// x <= w
					clipComponentVertices(0, auxVertices, &nAuxVertices, clippedVertices, &nVertices, -1.0f);	// -w <= x
					clipComponentVertices(1, clippedVertices, &nVertices, auxVertices, &nAuxVertices, 1.0f);	// y <= w
					clipComponentVertices(1, auxVertices, &nAuxVertices, clippedVertices, &nVertices, -1.0f);	// -w <= y
					clipComponentVertices(2, clippedVertices, &nVertices, auxVertices, &nAuxVertices, 1.0f);	// z <= w
					clipComponentVertices(2, auxVertices, &nAuxVertices, clippedVertices, &nVertices, -1.0f);	// -w <= z

					/*for (i32 i = 0; i < nAuxVertices; ++i)
					{
						clippedVertices[i] = auxVertices[i];
					}
					nVertices = nAuxVertices;
					nAuxVertices = 0;*/
				}

				if (nVertices > 0)
				{
					v2 finalPoint0;
					finalPoint0.x = clippedVertices[0].x / clippedVertices[0].w;
					finalPoint0.y = clippedVertices[0].y / clippedVertices[0].w;

					finalPoint0.x = 0.5f * (finalPoint0.x + 1.0f) * (m_FrameBuffer->Width - 1);
					finalPoint0.y = ((finalPoint0.y + 1.0f) * 0.5f) * (m_FrameBuffer->Height - 1);

					f32 w0 = 1.0f / clippedVertices[0].w;

					v2 finalUV0;

					if (textures && type == a3::RenderMapTexture)
					{
						finalUV0.u = clippedTextureCoords[0].u / clippedVertices[0].w;
						finalUV0.v = clippedTextureCoords[0].v / clippedVertices[0].w;
					}

					for (i32 n = 1; n < nVertices - 1; ++n)
					{
						v2 finalPoint1;
						v2 finalPoint2;

						finalPoint1.x = clippedVertices[n].x / clippedVertices[n].w;
						finalPoint1.y = clippedVertices[n].y / clippedVertices[n].w;

						finalPoint2.x = clippedVertices[n + 1].x / clippedVertices[n + 1].w;
						finalPoint2.y = clippedVertices[n + 1].y / clippedVertices[n + 1].w;

						finalPoint1.x = 0.5f * (finalPoint1.x + 1.0f) * (m_FrameBuffer->Width - 1);
						finalPoint1.y = ((finalPoint1.y + 1.0f) * 0.5f) * (m_FrameBuffer->Height - 1);

						finalPoint2.x = 0.5f * (finalPoint2.x + 1.0f) * (m_FrameBuffer->Width - 1);
						finalPoint2.y = ((finalPoint2.y + 1.0f) * 0.5f) * (m_FrameBuffer->Height - 1);

						f32 w1 = 1.0f / clippedVertices[n].w;
						f32 w2 = 1.0f / clippedVertices[n + 1].w;

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

							v2 finalUV1;
							v2 finalUV2;
							finalUV1.u = clippedTextureCoords[n].u / clippedVertices[n].w;
							finalUV2.u = clippedTextureCoords[n + 1].u / clippedVertices[n + 1].w;
							finalUV1.v = clippedTextureCoords[n].v / clippedVertices[n].w;
							finalUV2.v = clippedTextureCoords[n + 1].v / clippedVertices[n + 1].w;
							TextureTriangle((i32)finalPoint0.x, (i32)finalPoint0.y, finalUV0, w0, (i32)finalPoint1.x, (i32)finalPoint1.y, finalUV1, w1, (i32)finalPoint2.x, (i32)finalPoint2.y, finalUV2, w2);
						}

						if (m_DrawNormals)
						{
							v2 centroid{ (finalPoint0.x + finalPoint1.x + finalPoint2.x) / 3.0f, (finalPoint0.y + finalPoint1.y + finalPoint2.y) / 3.0f };
							a3::DrawLine(m_FrameBuffer, centroid, centroid + 30.0f * normal.xy, a3::color::Yellow);
						}
					}
				}


				/*if (nVertices > 0)
				{
					v2 finalPoi320;
					finalPoi320.x = clippedVertices[0].x / clippedVertices[0].w;
					finalPoi320.y = clippedVertices[0].y / clippedVertices[0].w;
					finalPoi320.u = clippedTextureCoords[0].u / clippedVertices[0].w;
					finalPoi320.v = clippedTextureCoords[0].v / clippedVertices[0].w;

					finalPoi320.x = 0.5f * (finalPoi320.x + 1.0f) * (m_FrameBuffer->Width - 1);
					finalPoi320.y = ((finalPoi320.y + 1.0f) * 0.5f) * (m_FrameBuffer->Height - 1);

					for (i32 n = 1; n < nVertices - 1; ++n)
					{
						v2 finalPoi321;
						v2 finalPoi322;

						finalPoi321.x = clippedVertices[n].x / clippedVertices[n].w;
						finalPoi321.y = clippedVertices[n].y / clippedVertices[n].w;
						finalPoi321.u = clippedTextureCoords[n].x / clippedVertices[n].w;
						finalPoi321.v = clippedTextureCoords[n].y / clippedVertices[n].w;

						finalPoi322.x = clippedVertices[n + 1].x / clippedVertices[n + 1].w;
						finalPoi322.y = clippedVertices[n + 1].y / clippedVertices[n + 1].w;
						finalPoi322.u = clippedTextureCoords[n].x / clippedVertices[n].w;
						finalPoi322.v = clippedTextureCoords[n].y / clippedVertices[n].w;

						finalPoi321.x = 0.5f * (finalPoi321.x + 1.0f) * (m_FrameBuffer->Width - 1);
						finalPoi321.y = ((finalPoi321.y + 1.0f) * 0.5f) * (m_FrameBuffer->Height - 1);

						finalPoi322.x = 0.5f * (finalPoi322.x + 1.0f) * (m_FrameBuffer->Width - 1);
						finalPoi322.y = ((finalPoi322.y + 1.0f) * 0.5f) * (m_FrameBuffer->Height - 1);

						a3::DrawTriangle(m_FrameBuffer, finalPoi320, finalPoi321, finalPoi322, a3::color::White);
					}
				}*/


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

	//void swapchain::FillTriangle(v2 p0, v2 p1, v2 p2, const v3& fillColor, f32 w0, f32 w1, f32 w2)
	//{
	//	a3::image* img = m_FrameBuffer;
	//	f32* depthBuffer = m_DepthBuffer;
	//	auto processTopFlatPart = [&img, &fillColor, &depthBuffer](v2* p0, v2* p1, v2* p2, f32 w0, f32 w1, f32 w2)
	//	{
	//		if (p2->x > p1->x) a3::Swap(&p2, &p1);

	//		f32 m0 = (p0->x - p2->x) / (p0->y - p2->y);
	//		f32 m1 = (p0->x - p1->x) / (p0->y - p1->y);
	//		i32 vstep = 2 * (i32)(p1->y - p0->y + 0.5f);

	//		f32 y = p0->y;
	//		for (i32 i = 0; i < vstep; ++i)
	//		{
	//			f32 x0 = m0 * (y - p2->y) + p2->x;
	//			f32 x1 = m1 * (y - p1->y) + p1->x;
	//			// NOTE(Zero):
	//			// Here 1 is added to the step to balance the 0.5f reduction in x
	//			// 0.5f is reduced from x because we sample from the center of the pixel
	//			i32 hstep = 2 * (i32)(x1 - x0 + 0.5f);
	//			f32 x = x0 - 0.5f;
	//			for (i32 j = 0; j < hstep; ++j)
	//			{
	//				a3::SetPixelColor(img, x, y, fillColor);
	//				x += 0.5f;
	//			}
	//			y += 0.5f;
	//		}
	//	};

	//	auto processBottomFlatPart = [&img, &fillColor](v2* p0, v2* p1, v2* p2, f32 w0, f32 w1, f32 w2)
	//	{
	//		if (p2->x > p1->x) a3::Swap(&p2, &p1);

	//		f32 m0 = (p0->x - p2->x) / (p0->y - p2->y);
	//		f32 m1 = (p0->x - p1->x) / (p0->y - p1->y);
	//		i32 vstep = 2 * (i32)(p0->y - p1->y + 0.5f);

	//		f32 y = p1->y;
	//		for (i32 i = 0; i < vstep; ++i)
	//		{
	//			f32 x0 = m0 * (y - p2->y) + p2->x;
	//			f32 x1 = m1 * (y - p1->y) + p1->x;
	//			// NOTE(Zero):
	//			// Here 1 is added to the step to balance the 0.5f reduction in x
	//			// 0.5f is reduced from x because we sample from the center of the pixel
	//			i32 hstep = 2 * (i32)(x1 - x0 + 0.5f);
	//			f32 x = x0 - 0.5f;
	//			for (i32 j = 0; j < hstep; ++j)
	//			{
	//				a3::SetPixelColor(img, x, y, fillColor);
	//				x += 0.5f;
	//			}
	//			y += 0.5f;
	//		}
	//	};

	//	if (p0.y > p1.y) a3::Swap(&p0, &p1);
	//	if (p0.y > p2.y) a3::Swap(&p0, &p2);
	//	if (p1.y > p2.y) a3::Swap(&p1, &p2);

	//	if (FAbsf(p1.y - p2.y) < epsilon_f32) // Top
	//	{
	//		processTopFlatPart(&p0, &p1, &p2);
	//	}
	//	else if (FAbsf(p1.y - p0.y) < epsilon_f32) // Bottom
	//	{
	//		processBottomFlatPart(&p2, &p1, &p0);
	//	}
	//	else // Split i32o 2 flat parts
	//	{
	//		f32 blend = (p1.y - p0.y) / (p2.y - p0.y);
	//		v2 pm = (1.0f - blend) * p0 + blend * p2;
	//		//processTopFlatPart(&p0, &p1, &pm); // top-flat
	//		processBottomFlatPart(&p2, &p1, &pm); // bottom-flat
	//	}

	//}

	//void swapchain::MapTextureTriangle(v2 p0, v2 p1, v2 p2, v2 t0, v2 t1, v2 t2, f32 w0, f32 w1, f32 w2)
	//{
	//	a3::image* img = m_FrameBuffer;
	//	a3::image* texture = m_Texture;
	//	f32* depthBuffer = m_DepthBuffer;
	//	auto processTopFlatPart = [&img, &texture](v2* p0, v2* p1, v2* p2, v2* t0, v2* t1, v2* t2, f32 w0, f32 w1, f32 w2)
	//	{
	//		if (p2->x > p1->x)
	//		{
	//			a3::Swap(&p2, &p1);
	//			a3::Swap(&t2, &t1);
	//		}

	//		f32 m0 = (p0->x - p2->x) / (p0->y - p2->y);
	//		f32 m1 = (p0->x - p1->x) / (p0->y - p1->y);
	//		i32 vstep = 2 * (i32)(p1->y - p0->y + 0.5f);

	//		v2 duvVert0 = (*t1 - *t0) * (1.0f / vstep);
	//		v2 duvVert1 = (*t2 - *t0) * (1.0f / vstep);
	//		f32 dw = (w2 - w0)*(1 / vstep);

	//		f32 y = p0->y;
	//		v2 startUV = *t0;
	//		v2 endUV = *t1;
	//		f32 sw = w0;
	//		for (i32 i = 0; i < vstep; ++i)
	//		{
	//			f32 x0 = m0 * (y - p2->y) + p2->x;
	//			f32 x1 = m1 * (y - p1->y) + p1->x;

	//			// NOTE(Zero):
	//			// Here 1 is added to the step to balance the 0.5f reduction in x
	//			// 0.5f is reduced from x because we sample from the center of the pixel
	//			i32 hstep = 2 * (i32)(x1 - x0 + 0.5f);
	//			f32 x = x0 - 0.5f;
	//			v2 uv = startUV;
	//			v2 duvHor = (endUV - startUV)*(1.0f / hstep);
	//			f32 dwx = ()
	//			for (i32 j = 0; j < hstep; ++j)
	//			{
	//				a3::SetPixelColor(img, x, y, a3::SamplePixelColor(texture, uv));
	//				x += 0.5f;
	//				uv += duvHor;
	//			}
	//			y += 0.5f;
	//			startUV += duvVert0;
	//			endUV += duvVert1;
	//		}
	//	};

	//	auto processBottomFlatPart = [&img, &texture](v2* p0, v2* p1, v2* p2, v2* t0, v2* t1, v2* t2, f32 w0, f32 w1, f32 w2)
	//	{
	//		if (p2->x > p1->x)
	//		{
	//			a3::Swap(&p2, &p1);
	//			a3::Swap(&t2, &t1);
	//		}

	//		f32 m0 = (p0->x - p2->x) / (p0->y - p2->y);
	//		f32 m1 = (p0->x - p1->x) / (p0->y - p1->y);
	//		i32 vstep = 2 * (i32)(p0->y - p1->y + 0.5f);

	//		v2 duvVert0 = (*t1 - *t0) * (1.0f / vstep);
	//		v2 duvVert1 = (*t2 - *t0) * (1.0f / vstep);

	//		f32 y = p1->y;
	//		v2 startUV = *t0;
	//		v2 endUV = *t1;
	//		for (i32 i = 0; i < vstep; ++i)
	//		{
	//			f32 x0 = m0 * (y - p2->y) + p2->x;
	//			f32 x1 = m1 * (y - p1->y) + p1->x;
	//			// NOTE(Zero):
	//			// Here 1 is added to the step to balance the 0.5f reduction in x
	//			// 0.5f is reduced from x because we sample from the center of the pixel
	//			i32 hstep = 2 * (i32)(x1 - x0 + 0.5f);
	//			f32 x = x0 - 0.5f;
	//			v2 uv = startUV;
	//			v2 duvHor = (endUV - startUV)*(1.0f / hstep);
	//			for (i32 j = 0; j < hstep; ++j)
	//			{
	//				a3::SetPixelColor(img, x, y, a3::SamplePixelColor(texture, uv));
	//				x += 0.5f;
	//				uv += duvHor;
	//			}
	//			y += 0.5f;
	//			startUV += duvVert0;
	//			endUV += duvVert1;
	//		}
	//	};

	//	if (p0.y > p1.y)
	//	{
	//		a3::Swap(&p0, &p1);
	//		a3::Swap(&t0, &t1);
	//	}
	//	if (p0.y > p2.y)
	//	{
	//		a3::Swap(&p0, &p2);
	//		a3::Swap(&t0, &t2);
	//	}
	//	if (p1.y > p2.y)
	//	{
	//		a3::Swap(&p1, &p2);
	//		a3::Swap(&t1, &t2);
	//	}

	//	if (FAbsf(p1.y - p2.y) < epsilon_f32) // Top
	//	{
	//		processTopFlatPart(&p0, &p1, &p2, &t0, &t1, &t2);
	//	}
	//	else if (FAbsf(p1.y - p0.y) < epsilon_f32) // Bottom
	//	{
	//		processBottomFlatPart(&p2, &p1, &p0, &t2, &t1, &t0);
	//	}
	//	else // Split i32o 2 flat parts
	//	{
	//		f32 blend = (p1.y - p0.y) / (p2.y - p0.y);
	//		v2 pm = (1.0f - blend) * p0 + blend * p2;
	//		v2 tm = (1.0f - blend) * t0 + blend * t2;
	//		processTopFlatPart(&p0, &p1, &pm, &t0, &t1, &tm); // top-flat
	//		processBottomFlatPart(&p2, &p1, &pm, &t0, &t1, &tm); // bottom-flat
	//	}

	//}

}


