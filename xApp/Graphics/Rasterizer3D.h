#pragma once
#include "Common/Core.h"
#include "Math/Math.h"
#include "Platform/Platform.h"
#include "Utility/Algorithm.h"

//
// DECLARATIONS
//

namespace a3 {

	struct swapchain
	{
	private:
		m4x4 m_Projection;
		m4x4 m_View;
		rect m_Viewport;
		mesh* m_Meshes;
		image* m_Texture;
		image* m_FrameBuffer;

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
		void Render(const m4x4& model, const v3& color = a3::color::White, b32 poly = false);
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

	void swapchain::Render(const m4x4& model, const v3& color, b32 poly)
	{
		a3Assert(m_FrameBuffer);

		m4x4 mvp = model * m_View * m_Projection;

		u32 nTriangles = m_Meshes->NumOfTriangles;
		v3* vertices = m_Meshes->Vertices;
		u32* indices = m_Meshes->VertexIndices;
		v2* textures = m_Meshes->TextureCoords;
		u32* tindices = m_Meshes->TextureCoordsIndices;


		for (u32 nTri = 0; nTri < nTriangles; ++nTri)
		{
			const v3& p0 = vertices[indices[nTri * 3 + 0]];
			const v3& p1 = vertices[indices[nTri * 3 + 1]];
			const v3& p2 = vertices[indices[nTri * 3 + 2]];

			/*const v2& t0 = textures[tindices[nTri * 2] + 0];
			const v2& t1 = textures[tindices[nTri * 2] + 1];
			const v2& t2 = textures[tindices[nTri * 2] + 2];*/



			v4 clippedVertices[18];
			i32 nVertices = 3;
			v3 clippedTextureCoords[18];
			i32 nTextureCoords = 3;

			/*clippedTextureCoords[0] = v3{ t0.u, t0.v, 1.0f };
			clippedTextureCoords[1] = v3{ t1.u, t1.v, 1.0f };
			clippedTextureCoords[2] = v3{ t2.u, t2.v, 1.0f };*/

			clippedVertices[0] = v4{ p0.x, p0.y, p0.z, 1.0f } *mvp;
			clippedVertices[1] = v4{ p1.x, p1.y, p1.z, 1.0f } *mvp;
			clippedVertices[2] = v4{ p2.x, p2.y, p2.z, 1.0f } *mvp;

			v3 auxTextureCoords[18];
			i32 nAuxTextureCoords = 0;

			v4 auxVertices[18];
			i32 nAuxVertices = 0;

			auto clipComponent = [](i32 comp, v4* vertices, i32* nVertices, v4* resVertices, i32* nResVertices, f32 sign)
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

			/*auto clipComponent = [](i32 comp, v4* vertices, v3* TextureCoords, 
											  i32* nVertices, i32* nTextureCoords,
											  v4* resVertices, v3* resTextureCoords, 
												i32* nResVertices, i32* nResTextureCoords,
												f32 sign)
			{
				for (i32 cnt = 0; cnt < *nVertices; ++cnt)
				{
					const v4& ver0 = vertices[cnt];
					const v4& ver1 = vertices[(cnt + 1) % *nVertices];
					const v3& tex0 = TextureCoords[cnt];
					const v3& tex1 = TextureCoords[(cnt + 1) % *nTextureCoords];

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
			};*/

			
			// NOTE(Zero): 
			// Since camera is at (0,0,0) and pointing towards z direction
			// the z component from result of Cross product gives the dot product
			//f32 dot = Cross(clippedVertices[1].xyz - clippedVertices[0].xyz, clippedVertices[2].xyz - clippedVertices[1].xyz).z;
			
			// NOTE(Zero):
			// Faster dot calculation because we don't need x and y component of the Cross product
			f32 lhsx = clippedVertices[1].x - clippedVertices[0].x;
			f32 lhsy = clippedVertices[1].y - clippedVertices[0].y;
			f32 rhsx = clippedVertices[2].x - clippedVertices[1].x;
			f32 rhsy = clippedVertices[2].y - clippedVertices[1].y;
			f32 dot = lhsx * rhsy - lhsy * rhsx;

			if (dot >= 0.0f)
			{
				//clipComponent(0, clippedVertices, clippedTextureCoords, &nVertices, &nTextureCoords, auxVertices, auxTextureCoords, &nAuxVertices, &nAuxTextureCoords, 1.0f);	// x <= w
				//clipComponent(0, clippedVertices, clippedTextureCoords, &nVertices, &nTextureCoords, auxVertices, auxTextureCoords, &nAuxVertices, &nAuxTextureCoords, -1.0f);	// x <= w

				//clipComponent(1, clippedVertices, clippedTextureCoords, &nVertices, &nTextureCoords, auxVertices, auxTextureCoords, &nAuxVertices, &nAuxTextureCoords, 1.0f);	// x <= w
				//clipComponent(1, clippedVertices, clippedTextureCoords, &nVertices, &nTextureCoords, auxVertices, auxTextureCoords, &nAuxVertices, &nAuxTextureCoords, -1.0f);	// x <= w

				//clipComponent(2, clippedVertices, clippedTextureCoords, &nVertices, &nTextureCoords, auxVertices, auxTextureCoords, &nAuxVertices, &nAuxTextureCoords, 1.0f);	// x <= w
				//clipComponent(2, clippedVertices, clippedTextureCoords, &nVertices, &nTextureCoords, auxVertices, auxTextureCoords, &nAuxVertices, &nAuxTextureCoords, -1.0f);	// x <= w

				clipComponent(0, clippedVertices, &nVertices, auxVertices, &nAuxVertices, 1.0f);	// x <= w
				clipComponent(0, auxVertices, &nAuxVertices, clippedVertices, &nVertices, -1.0f);	// -w <= x

				clipComponent(1, clippedVertices, &nVertices, auxVertices, &nAuxVertices, 1.0f);	// y <= w
				clipComponent(1, auxVertices, &nAuxVertices, clippedVertices, &nVertices, -1.0f);	// -w <= y
				clipComponent(2, clippedVertices, &nVertices, auxVertices, &nAuxVertices, 1.0f);	// z <= w
				clipComponent(2, auxVertices, &nAuxVertices, clippedVertices, &nVertices, -1.0f);	// -w <= z

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
						if (poly)
						{
							a3::DrawTriangle(m_FrameBuffer, finalPoint0, finalPoint1, finalPoint2, color);
						}
						else if (m_Texture)
						{
							
						}
						else
						{
							a3::FillTriangle(m_FrameBuffer, finalPoint0, finalPoint1, finalPoint2, color);
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


