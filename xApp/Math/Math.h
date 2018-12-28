#pragma once
#include "Common/Core.h"
#include "Matrix4.h"

#define MathPi 3.1415926535f
#define MathToRadians(x) ((x) * MathPi / 180.0f)
#define MathToDegrees(x) ((x) * 180.0f / MathPi)

inline v4 operator*(v4 vec, const m4x4& mat)
{
	v4 result;
	result.x = vec.x*mat.elements[0] + vec.y*mat.elements[4] + vec.z*mat.elements[8] + vec.w*mat.elements[12];
	result.y = vec.x*mat.elements[1] + vec.y*mat.elements[5] + vec.z*mat.elements[9] + vec.w*mat.elements[13];
	result.z = vec.x*mat.elements[2] + vec.y*mat.elements[6] + vec.z*mat.elements[10] + vec.w*mat.elements[14];
	result.w = vec.x*mat.elements[3] + vec.y*mat.elements[7] + vec.z*mat.elements[11] + vec.w*mat.elements[15];
	return result;
}

inline v4 operator*(const m4x4& mat, v4 vec)
{
	v4 result;
	result.x = Dot(mat.rows[0], vec);
	result.y = Dot(mat.rows[1], vec);
	result.z = Dot(mat.rows[2], vec);
	result.w = Dot(mat.rows[3], vec);
	return result;
}