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

namespace a3 {/*
	struct triangle
	{
		v4 p[3];
		v3 t[3];
		triangle() {}
		triangle(v3 p0, v3 p1, v3 p2, v2 t0, v2 t1, v2 t2)
		{
			p[0].xyz = p0;
			p[0].w = 1.0f;
			p[1].xyz = p1;
			p[1].w = 1.0f;
			p[2].xyz = p2;
			p[2].w = 1.0f;

			t[0].xy = t0;
			t[0].z = 1.0f;
			t[1].xy = t1;
			t[1].z = 1.0f;
			t[2].xy = t2;
			t[2].z = 1.0f;
		}
		triangle(const triangle& t)
		{
			p[0] = t.p[0];
			p[1] = t.p[1];
			p[2] = t.p[2];
			this->t[0] = t.t[0];
			this->t[1] = t.t[1];
			this->t[2] = t.t[2];
		}
		triangle& operator=(const triangle& tt)
		{
			if (this == &tt) return *this;
			p[0] = tt.p[0];
			p[1] = tt.p[1];
			p[2] = tt.p[2];
			this->t[0] = tt.t[0];
			this->t[1] = tt.t[1];
			this->t[2] = tt.t[2];
			return *this;
		}
	};

	a3::darray<triangle> tris;
	a3::darray<triangle> trianglesToRaster;
	a3::darray<triangle> listTriangles;
*/
	struct swapchain
	{
	private:
		m4x4 m_Projection;
		m4x4 m_View;
		rect m_Viewport;
		mesh* m_Meshes;
		image* m_Texture;
		image* m_FrameBuffer;
		v3 m_camera;

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
		void SetCameraPosition(v3 cameraPosition);
		void Render(const m4x4& model);
		//u32 Triangle_ClipAgainstPlane(v4 plane_p, v4 plane_n, triangle &in_tri, triangle &out_tri1, triangle &out_tri2);//Necessary function for
		//v4  Vector_IntersectPlane(v4 &plane_p, v4 &plane_n, v4 &lineStart, v4 &lineEnd, float& t);//clipping
		/*void TexturedTriangle(int x1, int y1, float u1, float v1, float w1,
			int x2, int y2, float u2, float v2, float w2,
			int x3, int y3, float u3, float v3, float w3,
			image *tex);*/
	};

}


//
// IMPLEMENTATION
//


namespace a3 {
	/*v4 swapchain::Vector_IntersectPlane(v4 &plane_p, v4 &plane_n, v4 &lineStart, v4 &lineEnd, float& t)
	{
		plane_n = Normalize(plane_n);
		float plane_d = -Dot(plane_n, plane_p);
		float ad = Dot(lineStart, plane_n);
		float bd = Dot(lineEnd, plane_n);
		t = (-plane_d - ad) / (bd - ad);
		v4 lineStartToEnd = lineEnd - lineStart;
		v4 lineToIntersect = lineStartToEnd * t;
		return lineStart + lineToIntersect;
	}*/


	//u32 swapchain::Triangle_ClipAgainstPlane(v4 plane_p, v4 plane_n, triangle &in_tri, triangle &out_tri1, triangle &out_tri2)
	//{
	//	plane_n = Normalize(plane_n);//normalise

	//	// returns shortest distance
	//	auto dist = [&](v4 &p)
	//	{
	//		v4 n = Normalize(p);//normalise
	//		return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - Dot(plane_n, plane_p));
	//	};


	//	v4* inside_points[3];  int nInsidePointCount = 0;
	//	v4* outside_points[3]; int nOutsidePointCount = 0;
	//	v3* inside_tex[3]; int nInsideTexCount = 0;
	//	v3* outside_tex[3]; int nOutsideTexCount = 0;

	//	float d0 = dist(in_tri.p[0]);
	//	float d1 = dist(in_tri.p[1]);
	//	float d2 = dist(in_tri.p[2]);
	//	//comparing shortest distance
	//	if (d0 >= 0) { inside_points[nInsidePointCount++] = &in_tri.p[0]; inside_tex[nInsideTexCount++] = &in_tri.t[0]; }
	//	else {
	//		outside_points[nOutsidePointCount++] = &in_tri.p[0]; outside_tex[nOutsideTexCount++] = &in_tri.t[0];
	//	}
	//	if (d1 >= 0) {
	//		inside_points[nInsidePointCount++] = &in_tri.p[1]; inside_tex[nInsideTexCount++] = &in_tri.t[1];
	//	}
	//	else {
	//		outside_points[nOutsidePointCount++] = &in_tri.p[1];  outside_tex[nOutsideTexCount++] = &in_tri.t[1];
	//	}
	//	if (d2 >= 0) {
	//		inside_points[nInsidePointCount++] = &in_tri.p[2]; inside_tex[nInsideTexCount++] = &in_tri.t[2];
	//	}
	//	else {
	//		outside_points[nOutsidePointCount++] = &in_tri.p[2];  outside_tex[nOutsideTexCount++] = &in_tri.t[2];
	//	}

	//	//store triangles to out_tri1 and 2 if any triangle is to be clipped off

	//	if (nInsidePointCount == 0)
	//	{
	//		//clipp whole triangle
	//		return 0; // No returned triangles are valid
	//	}

	//	if (nInsidePointCount == 3)
	//	{
	//		//no need to anything
	//		out_tri1 = in_tri;

	//		return 1; // Just the one returned original triangle is valid
	//	}

	//	if (nInsidePointCount == 1 && nOutsidePointCount == 2)
	//	{
	//		// Triangle should be clipped. As two points lie outside
	//		// the plane, the triangle simply becomes a smaller triangle


	//		/*out_tri1.col = in_tri.col;
	//		out_tri1.sym = in_tri.sym;*/

	//		out_tri1.p[0] = *inside_points[0];
	//		out_tri1.t[0] = *inside_tex[0];

	//		float t;
	//		out_tri1.p[1] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0], t);
	//		out_tri1.t[1].u = t * (outside_tex[0]->u - inside_tex[0]->u) + inside_tex[0]->u;
	//		out_tri1.t[1].v = t * (outside_tex[0]->v - inside_tex[0]->v) + inside_tex[0]->v;
	//		out_tri1.t[1].w = t * (outside_tex[0]->w - inside_tex[0]->w) + inside_tex[0]->w;

	//		out_tri1.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[1], t);
	//		out_tri1.t[2].u = t * (outside_tex[1]->u - inside_tex[0]->u) + inside_tex[0]->u;
	//		out_tri1.t[2].v = t * (outside_tex[1]->v - inside_tex[0]->v) + inside_tex[0]->v;
	//		out_tri1.t[2].w = t * (outside_tex[1]->w - inside_tex[0]->w) + inside_tex[0]->w;

	//		return 1; // Return the newly formed single triangle
	//	}

	//	if (nInsidePointCount == 2 && nOutsidePointCount == 1)
	//	{
	//		//two triangles are to be returned as it forms quadrilateral
	//		/*out_tri1.col = in_tri.col;
	//		out_tri1.sym = in_tri.sym;

	//		out_tri2.col = in_tri.col;
	//		out_tri2.sym = in_tri.sym;*/
	//		//tri1
	//		out_tri1.p[0] = *inside_points[0];
	//		out_tri1.p[1] = *inside_points[1];
	//		out_tri1.t[0] = *inside_tex[0];
	//		out_tri1.t[1] = *inside_tex[1];
	//		float t;
	//		out_tri1.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0], t);
	//		out_tri1.t[2].u = t * (outside_tex[0]->u - inside_tex[0]->u) + inside_tex[0]->u;
	//		out_tri1.t[2].v = t * (outside_tex[0]->v - inside_tex[0]->v) + inside_tex[0]->v;
	//		out_tri1.t[2].w = t * (outside_tex[0]->w - inside_tex[0]->w) + inside_tex[0]->w;

	//		//tri2
	//		out_tri2.p[0] = *inside_points[1];
	//		out_tri2.p[1] = out_tri1.p[2];
	//		out_tri2.t[0] = *inside_tex[1];
	//		out_tri2.t[1] = out_tri1.t[2];
	//		out_tri2.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[1], *outside_points[0], t);
	//		out_tri2.t[2].u = t * (outside_tex[0]->u - inside_tex[1]->u) + inside_tex[1]->u;
	//		out_tri2.t[2].v = t * (outside_tex[0]->v - inside_tex[1]->v) + inside_tex[1]->v;
	//		out_tri2.t[2].w = t * (outside_tex[0]->w - inside_tex[1]->w) + inside_tex[1]->w;

	//		return 2; // Return two newly formed triangles which form a quad
	//	}

	//	return 0;
	//}


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
		//tris.Resize(m_Meshes->NumOfTriangles);
	}

	inline void swapchain::SetTexture(image * tex)
	{
		m_Texture = tex;
	}

	void swapchain::SetFrameBuffer(image * tex)
	{
		m_FrameBuffer = tex;
	}

	void swapchain::SetCameraPosition(v3 cameraPosition)
	{
		m_camera = cameraPosition;
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
			v3 cullingCheck[3];
			i32 nVertices = 3;
			cullingCheck[0] = p0 *model;
			cullingCheck[1] = p1 *model;
			cullingCheck[2] = p2 *model;

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
			v3 normal;

			normal = Cross(cullingCheck[2] - cullingCheck[0], cullingCheck[1] - cullingCheck[0]);
		
			if (Dot(cullingCheck[0], Normalize(normal)) > 0.0f)
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

	//void swapchain::Render(const m4x4 & model)
	//{
	//	//m4x4 mvp = model * m4x4::Inverse(m_View) * m_Projection;

	//	u32 nTriangles = m_Meshes->NumOfTriangles;
	//	v3* vertices = m_Meshes->Vertices;
	//	u32* indices = m_Meshes->VertexIndices;

	//	v2* uv = m_Meshes->TextureCoords;
	//	u32* uvIndices = m_Meshes->TextureCoordsIndices;
	//	// if (uvIndices == A3NULL) {/*use directly*/ };

	//	for (u32 n = 0; n < nTriangles; ++n)
	//	{
	//		v3 t0 = vertices[indices[n * 3 + 0]];
	//		v3 t1 = vertices[indices[n * 3 + 1]];
	//		v3 t2 = vertices[indices[n * 3 + 2]];

	//		v2 u0, u1, u2;

	//		if (uvIndices == A3NULL)
	//		{
	//			u0 = uv[n * 3 + 0];
	//			u1 = uv[n * 3 + 1];
	//			u2 = uv[n * 3 + 2];
	//		}
	//		else
	//		{
	//			u0 = uv[uvIndices[n * 3 + 0]];
	//			u1 = uv[uvIndices[n * 3 + 1]];
	//			u2 = uv[uvIndices[n * 3 + 2]];
	//		}
	//		tris.Emplace(t0, t1, t2, u0, u1, u2);
	//	}

	//	v4 vCamera = {};
	//	vCamera.w = 1;

	//	v3 vUp = { 0,1,0 };
	//	v3 vTarget = { 0,0,1 };
	//	//mat4x4 matCameraRot = Matrix_MakeRotationY(fYax);
	//	vTarget = vCamera.xyz + vTarget;
	//	m4x4 matCamera = m4x4::LookR(vCamera.xyz, vTarget, vUp);

	//	// Make view matrix from camera
	//	m4x4 matView = m4x4::Inverse(matCamera);

	//	// Draw Triangles
	//	for (i32 i = 0; i < tris.QuerySize(); ++i)
	//	{
	//		a3::triangle tri = tris.GetElement(i);
	//		triangle triProjected, triTransformed, triViewed;

	//		// World Matrix Transform
	//		triTransformed.p[0] = tri.p[0] * model;
	//		triTransformed.p[1] = tri.p[1] * model;
	//		triTransformed.p[2] = tri.p[2] * model;
	//		triTransformed.t[0] = tri.t[0];
	//		triTransformed.t[1] = tri.t[1];
	//		triTransformed.t[2] = tri.t[2];

	//		v4 normal, line1, line2;

	//		line1 = triTransformed.p[1] - triTransformed.p[0];
	//		line2 = triTransformed.p[2] - triTransformed.p[0];

	//		normal = Cross(line1, line2);
	//		normal = Normalize(normal);

	//		// Get Ray from triangle to camera
	//		v4 vCameraRay = triTransformed.p[0] - vCamera;

	//		if (Dot(normal, vCameraRay) < 0.0f)
	//		{
	//			v4 light_direction = { 0.0f, 1.0f, -1.0f };
	//			light_direction = Normalize(light_direction);

	//			// Convert World Space --> View Space
	//			triViewed.p[0] = triTransformed.p[0] * m_View;
	//			triViewed.p[1] = triTransformed.p[1] * m_View;
	//			triViewed.p[2] = triTransformed.p[2] * m_View;
	//			triViewed.t[0] = triTransformed.t[0];
	//			triViewed.t[1] = triTransformed.t[1];
	//			triViewed.t[2] = triTransformed.t[2];

	//			int nClippedTriangles = 0;
	//			triangle clipped[2];
	//			nClippedTriangles = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.5f }, { 0.0f, 0.0f, 1.0f }, triViewed, clipped[0], clipped[1]);

	//			for (int n = 0; n < nClippedTriangles; n++)
	//			{
	//				// from 3D --> 2D
	//				triProjected.p[0] = clipped[n].p[0] * m_Projection;
	//				triProjected.p[1] = clipped[n].p[1] * m_Projection;
	//				triProjected.p[2] = clipped[n].p[2] * m_Projection;
	//				/*triProjected.col = clipped[n].col;
	//				triProjected.sym = clipped[n].sym;*/
	//				triProjected.t[0] = clipped[n].t[0];
	//				triProjected.t[1] = clipped[n].t[1];
	//				triProjected.t[2] = clipped[n].t[2];

	//				//Changing texture as per projection
	//				triProjected.t[0].u = triProjected.t[0].u / triProjected.p[0].w;
	//				triProjected.t[1].u = triProjected.t[1].u / triProjected.p[1].w;
	//				triProjected.t[2].u = triProjected.t[2].u / triProjected.p[2].w;

	//				triProjected.t[0].v = triProjected.t[0].v / triProjected.p[0].w;
	//				triProjected.t[1].v = triProjected.t[1].v / triProjected.p[1].w;
	//				triProjected.t[2].v = triProjected.t[2].v / triProjected.p[2].w;
	//				//updating w value of ve2d
	//				triProjected.t[0].w = 1.0f / triProjected.p[0].w;
	//				triProjected.t[1].w = 1.0f / triProjected.p[1].w;
	//				triProjected.t[2].w = 1.0f / triProjected.p[2].w;


	//				//scaling to view
	//				triProjected.p[0] = triProjected.p[0] * (1 / triProjected.p[0].w);
	//				triProjected.p[1] = triProjected.p[1] * (1 / triProjected.p[1].w);
	//				triProjected.p[2] = triProjected.p[2] * (1 / triProjected.p[2].w);

	//				//back inversion
	//				triProjected.p[0].x *= -1.0f;
	//				triProjected.p[1].x *= -1.0f;
	//				triProjected.p[2].x *= -1.0f;
	//				triProjected.p[0].y *= -1.0f;
	//				triProjected.p[1].y *= -1.0f;
	//				triProjected.p[2].y *= -1.0f;

	//				// Offset 
	//				v4 vOffsetView = { 1,1,0 };
	//				triProjected.p[0] = triProjected.p[0] + vOffsetView;
	//				triProjected.p[1] = triProjected.p[1] + vOffsetView;
	//				triProjected.p[2] = triProjected.p[2] + vOffsetView;
	//				triProjected.p[0].x *= 0.5f * (float)m_Viewport.w;
	//				triProjected.p[0].y *= 0.5f * (float)m_Viewport.h;
	//				triProjected.p[1].x *= 0.5f * (float)m_Viewport.w;
	//				triProjected.p[1].y *= 0.5f * (float)m_Viewport.h;
	//				triProjected.p[2].x *= 0.5f * (float)m_Viewport.w;
	//				triProjected.p[2].y *= 0.5f * (float)m_Viewport.h;

	//				trianglesToRaster.Push(triProjected);
	//			}
	//		}
	//	}

	//	for (u32 tt = 0; tt < trianglesToRaster.QuerySize(); ++tt)
	//	{
	//		// infinite loop here!!!!!
	//		a3::triangle triToRaster = trianglesToRaster.GetElement(tt);

	//		//triangle clipped[2];

	//		//listTriangles.Push(triToRaster);
	//		//int nNewTriangles = 1;

	//		//for (int p = 0; p < 4; p++)
	//		//{
	//		//	int nTrisToAdd = 0;
	//		//	while (nNewTriangles > 0)
	//		//	{
	//		//		// Take triangle from front of queue
	//		//		triangle test = *listTriangles.Begin();
	//		//		listTriangles.PopFront();
	//		//		nNewTriangles--;

	//		//		switch (p)
	//		//		{
	//		//		case 0:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
	//		//		case 1:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, (float)m_Viewport.h - 1, 0.0f }, { 0.0f, -1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
	//		//		case 2:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
	//		//		case 3:	nTrisToAdd = Triangle_ClipAgainstPlane({ (float)m_Viewport.w - 1, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
	//		//		}


	//		//		for (int i = 0; i < nTrisToAdd; i++)
	//		//			listTriangles.Push(clipped[i]);
	//		//	}
	//		//	nNewTriangles = (i32)listTriangles.QuerySize();
	//		//}

	//		//for (i32 i = 0; i < listTriangles.QuerySize(); ++i) 
	//		//{
	//		//	a3::triangle tri = listTriangles.GetElement(i);
	//			a3::DrawTriangle(m_FrameBuffer, triToRaster.p[0].xy, triToRaster.p[1].xy, triToRaster.p[2].xy, a3::color::White);
	//			/*TexturedTriangle(tri.p[0].x, tri.p[0].y, tri.t[0].u, tri.t[0].v, tri.t[0].w,
	//				tri.p[1].x, tri.p[1].y, tri.t[1].u, tri.t[1].v, tri.t[1].w,
	//				tri.p[2].x, tri.p[2].y, tri.t[2].u, tri.t[2].v, tri.t[2].w,
	//				m_Texture);*/
	//		//}
	//	}
	//}

	//void swapchain::TexturedTriangle(
	//	int x1, int y1, float u1, float v1, float w1,
	//	int x2, int y2, float u2, float v2, float w2,
	//	int x3, int y3, float u3, float v3, float w3, image * tex)
	//{
	//	{
	//		if (y2 < y1)
	//		{
	//			a3::Swap(&y1, &y2);
	//			a3::Swap(&x1, &x2);
	//			a3::Swap(&u1, &u2);
	//			a3::Swap(&v1, &v2);
	//			a3::Swap(&w1, &w2);
	//		}

	//		if (y3 < y1)
	//		{
	//			a3::Swap(&y1, &y3);
	//			a3::Swap(&x1, &x3);
	//			a3::Swap(&u1, &u3);
	//			a3::Swap(&v1, &v3);
	//			a3::Swap(&w1, &w3);
	//		}

	//		if (y3 < y2)
	//		{
	//			a3::Swap(&y2, &y3);
	//			a3::Swap(&x2, &x3);
	//			a3::Swap(&u2, &u3);
	//			a3::Swap(&v2, &v3);
	//			a3::Swap(&w2, &w3);
	//		}

	//		int dy1 = y2 - y1;
	//		int dx1 = x2 - x1;
	//		float dv1 = v2 - v1;
	//		float du1 = u2 - u1;
	//		float dw1 = w2 - w1;

	//		int dy2 = y3 - y1;
	//		int dx2 = x3 - x1;
	//		float dv2 = v3 - v1;
	//		float du2 = u3 - u1;
	//		float dw2 = w3 - w1;

	//		float tex_u, tex_v, tex_w;

	//		float dax_step = 0, dbx_step = 0,
	//			du1_step = 0, dv1_step = 0,
	//			du2_step = 0, dv2_step = 0,
	//			dw1_step = 0, dw2_step = 0;

	//		if (dy1) dax_step = dx1 / (float)abs(dy1);
	//		if (dy2) dbx_step = dx2 / (float)abs(dy2);

	//		if (dy1) du1_step = du1 / (float)abs(dy1);
	//		if (dy1) dv1_step = dv1 / (float)abs(dy1);
	//		if (dy1) dw1_step = dw1 / (float)abs(dy1);

	//		if (dy2) du2_step = du2 / (float)abs(dy2);
	//		if (dy2) dv2_step = dv2 / (float)abs(dy2);
	//		if (dy2) dw2_step = dw2 / (float)abs(dy2);

	//		if (dy1)
	//		{
	//			for (int i = y1; i <= y2; i++)
	//			{
	//				int ax = x1 + (float)(i - y1) * dax_step;
	//				int bx = x1 + (float)(i - y1) * dbx_step;

	//				float tex_su = u1 + (float)(i - y1) * du1_step;
	//				float tex_sv = v1 + (float)(i - y1) * dv1_step;
	//				float tex_sw = w1 + (float)(i - y1) * dw1_step;

	//				float tex_eu = u1 + (float)(i - y1) * du2_step;
	//				float tex_ev = v1 + (float)(i - y1) * dv2_step;
	//				float tex_ew = w1 + (float)(i - y1) * dw2_step;

	//				if (ax > bx)
	//				{
	//					a3::Swap(&ax, &bx);
	//					a3::Swap(&tex_su, &tex_eu);
	//					a3::Swap(&tex_sv, &tex_ev);
	//					a3::Swap(&tex_sw, &tex_ew);
	//				}

	//				tex_u = tex_su;
	//				tex_v = tex_sv;
	//				tex_w = tex_sw;

	//				float tstep = 1.0f / ((float)(bx - ax));
	//				float t = 0.0f;

	//				for (int j = ax; j < bx; j++)
	//				{
	//					tex_u = (1.0f - t) * tex_su + t * tex_eu;
	//					tex_v = (1.0f - t) * tex_sv + t * tex_ev;
	//					tex_w = (1.0f - t) * tex_sw + t * tex_ew;
	//					//if (tex_w > pDepthBuffer[i][j])
	//					//{

	//					a3::SetPixelColor(m_FrameBuffer, j, i, a3::SamplePixelColor(tex, tex_u / tex_w, tex_v / tex_w));


	//					//Draw(j, i, tex->SampleGlyph(tex_u / tex_w, tex_v / tex_w), tex->SampleColour(tex_u / tex_w, tex_v / tex_w));
	//					//pDepthBuffer[i][j] = tex_w;
	//				//}
	//					t += tstep;
	//				}

	//			}
	//		}

	//		dy1 = y3 - y2;
	//		dx1 = x3 - x2;
	//		dv1 = v3 - v2;
	//		du1 = u3 - u2;
	//		dw1 = w3 - w2;

	//		if (dy1) dax_step = dx1 / (float)abs(dy1);
	//		if (dy2) dbx_step = dx2 / (float)abs(dy2);

	//		du1_step = 0, dv1_step = 0;
	//		if (dy1) du1_step = du1 / (float)abs(dy1);
	//		if (dy1) dv1_step = dv1 / (float)abs(dy1);
	//		if (dy1) dw1_step = dw1 / (float)abs(dy1);

	//		if (dy1)
	//		{
	//			for (int i = y2; i <= y3; i++)
	//			{
	//				int ax = x2 + (float)(i - y2) * dax_step;
	//				int bx = x1 + (float)(i - y1) * dbx_step;

	//				float tex_su = u2 + (float)(i - y2) * du1_step;
	//				float tex_sv = v2 + (float)(i - y2) * dv1_step;
	//				float tex_sw = w2 + (float)(i - y2) * dw1_step;

	//				float tex_eu = u1 + (float)(i - y1) * du2_step;
	//				float tex_ev = v1 + (float)(i - y1) * dv2_step;
	//				float tex_ew = w1 + (float)(i - y1) * dw2_step;

	//				if (ax > bx)
	//				{
	//					a3::Swap(&ax, &bx);
	//					a3::Swap(&tex_su, &tex_eu);
	//					a3::Swap(&tex_sv, &tex_ev);
	//					a3::Swap(&tex_sw, &tex_ew);
	//				}

	//				tex_u = tex_su;
	//				tex_v = tex_sv;
	//				tex_w = tex_sw;

	//				float tstep = 1.0f / ((float)(bx - ax));
	//				float t = 0.0f;

	//				for (int j = ax; j < bx; j++)
	//				{
	//					tex_u = (1.0f - t) * tex_su + t * tex_eu;
	//					tex_v = (1.0f - t) * tex_sv + t * tex_ev;
	//					tex_w = (1.0f - t) * tex_sw + t * tex_ew;

	//					//if (tex_w > pDepthBuffer[i][j])
	//					//{
	//					a3::SetPixelColor(m_FrameBuffer, j, i, a3::SamplePixelColor(tex, tex_u / tex_w, tex_v / tex_w));
	//					//Draw(j, i, tex->SampleGlyph(tex_u / tex_w, tex_v / tex_w), tex->SampleColour(tex_u / tex_w, tex_v / tex_w));
	//					//	pDepthBuffer[i][j] = tex_w;
	//						//}
	//					t += tstep;
	//				}
	//			}
	//		}
	//	}
	//}



}


