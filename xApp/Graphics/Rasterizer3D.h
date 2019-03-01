#pragma once
#include "Common/Core.h"
#include "Math/Math.h"
#include "Platform/Platform.h"
#include "Utility/Algorithm.h"
//#include <vector>
//#include <list>
#include "Utility/DArray.h"

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
		void Render(const m4x4& model);
	};

}


//
// IMPLEMENTATION
//


namespace a3 {

	swapchain::swapchain()
	{
		m_FrameBuffer = A3NULL;
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

	void swapchain::Render(const m4x4& model)
	{
		a3Assert(m_FrameBuffer);

		m4x4 mvp = model * m_View * m_Projection;

		u32 nTriangles = m_Meshes->NumOfTriangles;
		v3* vertices = m_Meshes->Vertices;
		u32* indices = m_Meshes->VertexIndices;

		for (u32 nTri = 0; nTri < nTriangles; ++nTri)
		{
			const v3& p0 = vertices[indices[nTri * 3 + 0]];
			const v3& p1 = vertices[indices[nTri * 3 + 1]];
			const v3& p2 = vertices[indices[nTri * 3 + 2]];

			v4 clippedVertices[18];
			i32 nVertices = 3;

			clippedVertices[0] = v4{ p0.x, p0.y, p0.z, 1.0f } *mvp;
			clippedVertices[1] = v4{ p1.x, p1.y, p1.z, 1.0f } *mvp;
			clippedVertices[2] = v4{ p2.x, p2.y, p2.z, 1.0f } *mvp;

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

						a3::DrawTriangle(m_FrameBuffer, finalPoint0, finalPoint1, finalPoint2, a3::color::White);
					}
				}

			}
		}
	}

}


