#pragma once
#include "Common/Core.h"
#include "Math/Math.h"
#include "Utility/AssetData.h"
#include "Graphics/Rasterizer2D.h"
#include "Math/Color.h"

namespace a3 {

	f32 Max(f32 a, f32  b) {
		if (a > b) {
			return a;
		}
		return b;
	}
	f32 Min(f32 a, f32 b) {
		if (a < b) {
			return a;
		}
		return b;
	}

	b32 RayTriangleIntersect(
		const v3 &orig, const v3 &dir,
		const v3 &v0, const v3 &v1, const v3 &v2,
		f32 *nearDistance, f32 *u, f32 *v)
	{
		v3 v0v1 = v1 - v0;
		v3 v0v2 = v2 - v0;

		v3 pvec = Cross(dir, v0v2);
		f32 det = Dot(v0v1, pvec);

		if (FAbsf(det) < epsilon_f32) return false;

		f32 invDet = 1 / det;

		v3 tvec = orig - v0;
		*u = Dot(tvec, pvec) * invDet;
		if ((*u) < 0 || (*u) > 1) return false;

		v3 qvec = Cross(tvec, v0v1);
		*v = Dot(dir, qvec) * invDet;
		if ((*v) < 0 || (*u + *v) > 1) return false;

		*nearDistance = Dot(v0v2, qvec) * invDet;

		return true;
	}


	b32 RayIntersectMesh(mesh* meshObj, const v3 &orig, const v3 &dir, f32 *tNear, u32 *triIndex, v2 *uv)
	{
		b32 isect = false;
		u32 numTris = meshObj->NumOfTriangles;
		v3* vertices = meshObj->Vertices;
		u32* trisIndex = meshObj->VertexIndices;
		for (u32 i = 0; i < numTris; ++i) {
			const v3 &v0 = vertices[trisIndex[i * 3 + 0]];
			const v3 &v1 = vertices[trisIndex[i * 3 + 1]];
			const v3 &v2 = vertices[trisIndex[i * 3 + 2]];
			f32 t = max_f32, u, v;
			if (RayTriangleIntersect(orig, dir, v0, v1, v2, &t, &u, &v) && t < *tNear) {
				*tNear = t;
				uv->x = u;
				uv->y = v;
				*triIndex = i;
				isect = true;
			}
		}

		return isect;
	}

	b32 Trace(mesh* meshObj,
		const v3 &orig, const v3 &dir,
		f32 *tNear, u32 *index, v2 *uv)
	{
		f32 tNearTriangle = max_f32;
		u32 indexTriangle;
		v2 uvTriangle;
		b32 trace = false;
		if (RayIntersectMesh(meshObj, orig, dir, &tNearTriangle, &indexTriangle, &uvTriangle) && tNearTriangle < *tNear)
		{
			*tNear = tNearTriangle;
			*index = indexTriangle;
			*uv = uvTriangle;
			trace = true;
		}
		return trace;
	}


	void GetSurfaceProperties(mesh* meshObj,
		const v3 &hitPoi32,
		const v3 &viewDirection,
		const u32 &triIndex,
		const v2 &uv,
		v3 *hitNormal,
		v2 *hitTextureCoordinates, b32* texIsPresent)
	{
		v3* vertices = meshObj->Vertices;
		u32* trisIndex = meshObj->VertexIndices;
		v2* texCoordinates = meshObj->TextureCoords;
		// face normal
		const v3 &p0 = vertices[trisIndex[triIndex * 3 + 0]];
		const v3 &p1 = vertices[trisIndex[triIndex * 3 + 1]];
		const v3 &p2 = vertices[trisIndex[triIndex * 3 + 2]];
		*hitNormal = Cross(p1 - p0, p2 - p0);
		*hitNormal = Normalize(*hitNormal);

		// texture coordinates
		if (texCoordinates)
		{
			const v2 &st0 = texCoordinates[triIndex * 3 + 0];
			const v2 &st1 = texCoordinates[triIndex * 3 + 1];
			const v2 &st2 = texCoordinates[triIndex * 3 + 2];
			*hitTextureCoordinates = (1 - uv.x - uv.y) * st0 + uv.x * st1 + uv.y * st2;
			*texIsPresent = true;
		}
		else
		{
			*texIsPresent = false;
		}
	}


	u32 CastRay(v3 origin, v3 dir, mesh* meshObj, a3::image* frameBuffer, a3::image* texture)
	{
		v3 hitColor;
		hitColor = a3::color::Black;

		f32 tnear = max_f32;
		v2 uv;
		u32 index = 0;
		if (Trace(meshObj, origin, dir, &tnear, &index, &uv))
		{
			v3 hitPoint = origin + dir * tnear;
			v3 hitNormal;
			v2 hitTexCoordinates;
			b32 texPresent;
			GetSurfaceProperties(meshObj, hitPoint, dir, index, uv, &hitNormal, &hitTexCoordinates, &texPresent);
			f32 normDotView = Max(0.f, Dot(hitNormal, -dir));
			const f32 mat = 10.0f;
			hitColor = a3::color::Blurple; // default color
			if (texPresent)
			{
				if (texture)
				{
					hitColor = a3::SamplePixelColor(texture, hitTexCoordinates).rgb;
				}
				else
				{
					f32 checker = (f32)((FModf(hitTexCoordinates.x * mat, 1.0f) > 0.5f) ^ (FModf(hitTexCoordinates.y * mat, 1.0f) < 0.5f));
					f32 cs = 0.3f * (1.0f - checker) + 0.7f * checker;
					hitColor = v3{ cs,cs,cs };
				}
			}
			hitColor *= normDotView;
		}

		u32 hitHexColor = a3Normalv3ToRGBA(hitColor, 0xffffff);
		return hitHexColor;
	}

	void RayTrace(image* frameBuffer, mesh* meshObj, const m4x4& view, a3::image* texture, i32* major, i32* minor)
	{
		f32 aspectRatio = (f32)frameBuffer->Height / (f32)frameBuffer->Width;
		v3 origin = v3{ 0,0,0 } *view;

		for (i32 j = 0; j < frameBuffer->Height; j++)
		{
			for (i32 i = 0; i < frameBuffer->Width; i++)
			{
				f32 x = (2.0f * ((f32)i + 0.5f) / (f32)frameBuffer->Width - 1.0f) * aspectRatio;
				f32 y = (1.0f - 2.0f * ((f32)j + 0.5f) / (f32)frameBuffer->Height);
				v3 dir = v3{ x,y,1.0f } *view;
				dir = Normalize(dir);
				u32 hColor = CastRay(origin, dir, meshObj, frameBuffer, texture);
				a3::SetPixel(frameBuffer, i, j, hColor);
				f32 percentComplete = (f32)x*(f32)y / ((f32)(frameBuffer->Width*frameBuffer->Height));
				*major = (i32)percentComplete;
				*minor = (i32)((f32)(percentComplete - *major)*100.0f);
			}
		}
	}

}