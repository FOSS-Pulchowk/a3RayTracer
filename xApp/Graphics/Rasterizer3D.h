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
		void Render(const m4x4& model, render_type type, const v3& shade = a3::color::White, const v3& outline = a3::color::Yellow);
	};

}


//
// IMPLEMENTATION
//


namespace a3 {

	swapchain::swapchain()
	{
		m_FrameBuffer = A3NULL;
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
	}

	inline void swapchain::SetDrawNormals(b32 normals)
	{
		m_DrawNormals = normals;
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
				v4* vertices, v2* textureCoords,  i32* nVertices, i32* nTextureCoords,
				v4* resVertices, v2* resTextureCoords,  i32* nResVertices, i32* nResTextureCoords,
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
			// Since camera is at (0,0,0) and pointing towards z direction
			// the z component from result of Cross product gives the dot product
			v3 normal = Normalize(Cross(clippedVertices[1].xyz - clippedVertices[0].xyz, clippedVertices[2].xyz - clippedVertices[1].xyz));
			f32 dot = normal.z;

			if (dot > 0.0f)
			{
				if (textures && type == a3::RenderMapTexture)
				{
					clipComponentVerticesWithUV(0, clippedVertices, clippedTextureCoords, &nVertices, &nTextureCoords, auxVertices, auxTextureCoords, &nAuxVertices, &nAuxTextureCoords, 1.0f);	// x <= w
					clipComponentVerticesWithUV(0, clippedVertices, clippedTextureCoords, &nVertices, &nTextureCoords, auxVertices, auxTextureCoords, &nAuxVertices, &nAuxTextureCoords, -1.0f);	// x <= w
					clipComponentVerticesWithUV(1, clippedVertices, clippedTextureCoords, &nVertices, &nTextureCoords, auxVertices, auxTextureCoords, &nAuxVertices, &nAuxTextureCoords, 1.0f);	// x <= w
					clipComponentVerticesWithUV(1, clippedVertices, clippedTextureCoords, &nVertices, &nTextureCoords, auxVertices, auxTextureCoords, &nAuxVertices, &nAuxTextureCoords, -1.0f);	// x <= w
					clipComponentVerticesWithUV(2, clippedVertices, clippedTextureCoords, &nVertices, &nTextureCoords, auxVertices, auxTextureCoords, &nAuxVertices, &nAuxTextureCoords, 1.0f);	// x <= w
					clipComponentVerticesWithUV(2, clippedVertices, clippedTextureCoords, &nVertices, &nTextureCoords, auxVertices, auxTextureCoords, &nAuxVertices, &nAuxTextureCoords, -1.0f);	// x <= w
				}
				else
				{
					clipComponentVertices(0, clippedVertices, &nVertices, auxVertices, &nAuxVertices, 1.0f);	// x <= w
					clipComponentVertices(0, auxVertices, &nAuxVertices, clippedVertices, &nVertices, -1.0f);	// -w <= x
					clipComponentVertices(1, clippedVertices, &nVertices, auxVertices, &nAuxVertices, 1.0f);	// y <= w
					clipComponentVertices(1, auxVertices, &nAuxVertices, clippedVertices, &nVertices, -1.0f);	// -w <= y
					clipComponentVertices(2, clippedVertices, &nVertices, auxVertices, &nAuxVertices, 1.0f);	// z <= w
					clipComponentVertices(2, auxVertices, &nAuxVertices, clippedVertices, &nVertices, -1.0f);	// -w <= z
				}

				if (nVertices > 0)
				{
					v2 finalPoint0;
					finalPoint0.x = clippedVertices[0].x / clippedVertices[0].w;
					finalPoint0.y = clippedVertices[0].y / clippedVertices[0].w;

					finalPoint0.x = 0.5f * (finalPoint0.x + 1.0f) * (m_FrameBuffer->Width - 1);
					finalPoint0.y = ((finalPoint0.y + 1.0f) * 0.5f) * (m_FrameBuffer->Height - 1);

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
						if (type == a3::RenderTriangle)
						{
							a3::DrawTriangle(m_FrameBuffer, finalPoint0, finalPoint1, finalPoint2, outline);
						}
						else if (type == a3::RenderShade)
						{
							a3::FillTriangle(m_FrameBuffer, finalPoint0, finalPoint1, finalPoint2, shade * dot);
						}
						else if (type == a3::RenderShadeWithOutline)
						{
							a3::FillTriangle(m_FrameBuffer, finalPoint0, finalPoint1, finalPoint2, shade * dot);
							a3::DrawTriangle(m_FrameBuffer, finalPoint0, finalPoint1, finalPoint2, outline);
						}
						else
						{
							
						}
						if (m_DrawNormals)
						{
							v2 centroid{ (finalPoint0.x + finalPoint1.x + finalPoint2.x) / 3.0f, (finalPoint0.y + finalPoint1.y + finalPoint2.y) / 3.0f };
							a3::DrawLine(m_FrameBuffer, centroid, centroid + 30.0f * normal.xy, a3::color::Red);
						}
					}
				}


				/*if (nVertices > 0)
				{
					v2 finalPoint0;
					finalPoint0.x = clippedVertices[0].x / clippedVertices[0].w;
					finalPoint0.y = clippedVertices[0].y / clippedVertices[0].w;
					finalPoint0.u = clippedTextureCoords[0].u / clippedVertices[0].w;
					finalPoint0.v = clippedTextureCoords[0].v / clippedVertices[0].w;

					finalPoint0.x = 0.5f * (finalPoint0.x + 1.0f) * (m_FrameBuffer->Width - 1);
					finalPoint0.y = ((finalPoint0.y + 1.0f) * 0.5f) * (m_FrameBuffer->Height - 1);

					for (i32 n = 1; n < nVertices - 1; ++n)
					{
						v2 finalPoint1;
						v2 finalPoint2;

						finalPoint1.x = clippedVertices[n].x / clippedVertices[n].w;
						finalPoint1.y = clippedVertices[n].y / clippedVertices[n].w;
						finalPoint1.u = clippedTextureCoords[n].x / clippedVertices[n].w;
						finalPoint1.v = clippedTextureCoords[n].y / clippedVertices[n].w;

						finalPoint2.x = clippedVertices[n + 1].x / clippedVertices[n + 1].w;
						finalPoint2.y = clippedVertices[n + 1].y / clippedVertices[n + 1].w;
						finalPoint2.u = clippedTextureCoords[n].x / clippedVertices[n].w;
						finalPoint2.v = clippedTextureCoords[n].y / clippedVertices[n].w;

						finalPoint1.x = 0.5f * (finalPoint1.x + 1.0f) * (m_FrameBuffer->Width - 1);
						finalPoint1.y = ((finalPoint1.y + 1.0f) * 0.5f) * (m_FrameBuffer->Height - 1);

						finalPoint2.x = 0.5f * (finalPoint2.x + 1.0f) * (m_FrameBuffer->Width - 1);
						finalPoint2.y = ((finalPoint2.y + 1.0f) * 0.5f) * (m_FrameBuffer->Height - 1);

						a3::DrawTriangle(m_FrameBuffer, finalPoint0, finalPoint1, finalPoint2, a3::color::White);
					}
				}*/


			}
		}
	}

}


