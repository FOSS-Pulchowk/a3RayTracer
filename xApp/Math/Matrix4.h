#pragma once
#include "Common/Core.h"

/*
	* NOTE(Zero): 
	* Elements of the matrices are stored in row-major sequential memory order
	* Elements can be accessed by using 2 dimentional array
	* [0][0] [0][1] [0][2] [0][3]
	* [1][0] [1][1] [1][2] [1][3]
	* [2][0] [2][1] [2][2] [2][3]
	* [3][0] [3][1] [3][2] [3][3]
	* ---- OR using single dimentional array ----
	* [0 * 4 + 0] [0 * 4 + 1] [0 * 4 + 2] [0 * 4 + 3]
	* [1 * 4 + 0] [1 * 4 + 1] [1 * 4 + 2] [1 * 4 + 3]
	* [2 * 4 + 0] [2 * 4 + 1] [2 * 4 + 2] [2 * 4 + 3]
	* [3 * 4 + 0] [3 * 4 + 1] [3 * 4 + 2] [3 * 4 + 3]
	* When generating matrices the methods using row-major order are postfixed with 'R'
	* while the methods postfixed with 'C' use column-major ordering
*/
#if 0
	// USAGE(Zero) @ 12/30/2018 20:25
	m4x4 mat; // mat is identity matrix
	m4x4 mat1(5); // mat1 is scalar matrix with each all diagonal elements 5
	f32 e23 = mat.elements[2 * 4 + 3]; // access element from 2nd row and 3rd column
	// f32 e23 = mat.elements[2][3]; // alternative way for the above line
	// +=, -=, *=, +, -, * operators are overloaded
	m4x4 res = mat * mat1; // multiply two matrices

	v3 vec{ 200, 300, 500 };
	// v3 are transformed by m4x4 by taking last element as 1.0f
	// if different value is required in the 4th element of vector v4 can be used
	v3 transformed = vec * res; // `vec` to transform by `res` row-major way
	// v3 transformed = res * vec; // here's transformation occurs in column-major way
	m4x4 rot = m4x4::RotationR(angle, { x, y, z }); // returns rotation matrix in row-major way
	// to get column-major matrix `RotationC` can be used
	// this convention is followed for all matrix transformation present here
#endif

//
// DECLARATIONS
//

struct m4x4
{
	union 
	{ 
		f32 elements[4 * 4];
		f32 elements2[4][4];
		v4 rows[4];
	};

	m4x4(); // Initializes a Identity Matrix
	m4x4(const f32 elementsArray[4 * 4]);

	m4x4(const v4& row0, const v4& row1, const v4& row2, const v4& row3);

	m4x4(
		f32 e00, f32 e10, f32 e20, f32 e30,
		f32 e01, f32 e11, f32 e21, f32 e31,
		f32 e02, f32 e12, f32 e22, f32 e32,
		f32 e03, f32 e13, f32 e23, f32 e33
	);

	m4x4(f32 diagonalElement); // Diagonal Matrix with given element

	m4x4(const m4x4& mat);
	m4x4& operator=(const m4x4& mat);

	m4x4 operator-() const;

	m4x4& operator*=(const m4x4& mat);
	m4x4& operator+=(const m4x4& mat);
	m4x4& operator-=(const m4x4& mat);

	friend m4x4 operator*(f32 scalar, const m4x4& mat);
	friend m4x4 operator*(const m4x4& mat, f32 scalar);

	friend m4x4 operator*(const m4x4& lhs, const m4x4& rhs);

	friend m4x4 operator+(const m4x4& lhs, const m4x4& rhs);
	friend m4x4 operator-(const m4x4& lhs, const m4x4& rhs);

	static m4x4 Transpose(const m4x4& mat);
	// Garbage value will be returned if the determinant is zero
	static m4x4 Inverse(const m4x4& mat);
	static m4x4 Identity();

	static m4x4 ScaleR(v3 vec);
	static m4x4 TranslationR(v3 vec);
	static m4x4 RotationR(f32 angle, v3 axis);
	static m4x4 LookR(v3 from, v3 to, v3 up = { 0.0f, 0.0f, 1.0f });
	// NOTE(Zero): Windows header file defines `far` and `near`, so not to use these as variables
	static m4x4 OrthographicR(f32 left, f32 right, f32 bottom, f32 top, f32 cNear, f32 cFar);
	static m4x4 PerspectiveR(f32 fov, f32 aspectRatio, f32 near, f32 far);
	
	static m4x4 ScaleC(v3 vec);
	static m4x4 TranslationC(v3 vec);
	static m4x4 RotationC(f32 angle, v3 axis);
	static m4x4 LookC(v3 from, v3 to, v3 up = { 0.0f, 0.0f, 1.0f });
	// NOTE(Zero): Windows header file defines `far` and `near`, so not to use these as variables
	static m4x4 OrthographicC(f32 left, f32 right, f32 bottom, f32 top, f32 cNear, f32 cFar);
	static m4x4 PerspectiveC(f32 fov, f32 aspectRatio, f32 cNear, f32 cFar);
};


//
// IMPLEMENTATION
//
#ifdef A3_IMPLEMENT_MATRIX

m4x4::m4x4() :
	elements{
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
}
{
}

m4x4::m4x4(const f32 elementsArray[4 * 4])
{
	for (int i = 0; i < 16; i++)
		this->elements[i] = elementsArray[i];
}

m4x4::m4x4(
	const v4& row0,
	const v4& row1,
	const v4& row2,
	const v4& row3
) :
	rows{ row0, row1, row2, row3 }
{
}

m4x4::m4x4(
	f32 e00, f32 e10, f32 e20, f32 e30,
	f32 e01, f32 e11, f32 e21, f32 e31,
	f32 e02, f32 e12, f32 e22, f32 e32,
	f32 e03, f32 e13, f32 e23, f32 e33
) :
	elements{
	e00, e10, e20, e30,
	e01, e11, e21, e31,
	e02, e12, e22, e32,
	e03, e13, e23, e33
}
{
}

m4x4::m4x4(f32 diagonalElement) :
	elements{
	diagonalElement,0.0f,0.0f,0.0f,
	0.0f,diagonalElement,0.0f,0.0f,
	0.0f,0.0f,diagonalElement,0.0f,
	0.0f,0.0f,0.0f,diagonalElement
}
{
}

m4x4::m4x4(const m4x4& mat)
{
	rows[0] = mat.rows[0];
	rows[1] = mat.rows[1];
	rows[2] = mat.rows[2];
	rows[3] = mat.rows[3];
}

m4x4& m4x4::operator=(const m4x4& mat)
{
	if (this == &mat)
		return *this;
	rows[0] = mat.rows[0];
	rows[1] = mat.rows[1];
	rows[2] = mat.rows[2];
	rows[3] = mat.rows[3];
	return *this;
}

m4x4 m4x4::operator-() const
{
	return m4x4(-rows[0], -rows[1], -rows[2], -rows[3]);
}

m4x4& m4x4::operator*=(const m4x4& mat)
{
	*this = *this * mat;
	return *this;
}

m4x4& m4x4::operator+=(const m4x4& mat)
{
	rows[0] += mat.rows[0];
	rows[1] += mat.rows[1];
	rows[2] += mat.rows[2];
	rows[3] += mat.rows[3];
	return *this;
}

m4x4& m4x4::operator-=(const m4x4& mat)
{
	rows[0] -= mat.rows[0];
	rows[1] -= mat.rows[1];
	rows[2] -= mat.rows[2];
	rows[3] -= mat.rows[3];
	return *this;
}

m4x4 operator*(f32 scalar, const m4x4& mat)
{
	return m4x4(scalar * mat.rows[0], scalar * mat.rows[1], scalar * mat.rows[2], scalar * mat.rows[3]);
}

m4x4 operator*(const m4x4& mat, f32 scalar)
{
	return (scalar * mat);
}

m4x4 operator*(const m4x4& lhs, const m4x4& rhs)
{
	m4x4 temp;
	for (i32 y = 0; y < 4; y++)
	{
		for (i32 x = 0; x < 4; x++)
		{
			temp.elements[x + 4 * y] = 0;
			for (i32 e = 0; e < 4; e++)
			{
				temp.elements[x + 4 * y] += lhs.elements[e + 4 * y] * rhs.elements[x + 4 * e];
			}
		}
	}
	return temp;
}

m4x4 operator+(const m4x4& lhs, const m4x4& rhs)
{
	return m4x4(lhs.rows[0] + rhs.rows[0], lhs.rows[1] + rhs.rows[1], lhs.rows[2] + rhs.rows[2], lhs.rows[3] + rhs.rows[3]);
}

m4x4 operator-(const m4x4& lhs, const m4x4& rhs)
{
	return m4x4(lhs.rows[0] - rhs.rows[0], lhs.rows[1] - rhs.rows[1], lhs.rows[2] - rhs.rows[2], lhs.rows[3] - rhs.rows[3]);
}

m4x4 m4x4::Transpose(const m4x4& mat)
{
	m4x4 temp;
	for (i32 y = 0; y < 4; y++)
	{
		for (i32 x = 0; x < 4; x++)
			temp.elements[4 * y + x] = mat.elements[4 * x + y];
	}
	return temp;
}

m4x4 m4x4::Inverse(const m4x4& mat)
{
	m4x4 result;

	result.elements[0] = mat.elements[5] * mat.elements[10] * mat.elements[15] -
		mat.elements[5] * mat.elements[11] * mat.elements[14] -
		mat.elements[9] * mat.elements[6] * mat.elements[15] +
		mat.elements[9] * mat.elements[7] * mat.elements[14] +
		mat.elements[13] * mat.elements[6] * mat.elements[11] -
		mat.elements[13] * mat.elements[7] * mat.elements[10];

	result.elements[4] = -mat.elements[4] * mat.elements[10] * mat.elements[15] +
		mat.elements[4] * mat.elements[11] * mat.elements[14] +
		mat.elements[8] * mat.elements[6] * mat.elements[15] -
		mat.elements[8] * mat.elements[7] * mat.elements[14] -
		mat.elements[12] * mat.elements[6] * mat.elements[11] +
		mat.elements[12] * mat.elements[7] * mat.elements[10];

	result.elements[8] = mat.elements[4] * mat.elements[9] * mat.elements[15] -
		mat.elements[4] * mat.elements[11] * mat.elements[13] -
		mat.elements[8] * mat.elements[5] * mat.elements[15] +
		mat.elements[8] * mat.elements[7] * mat.elements[13] +
		mat.elements[12] * mat.elements[5] * mat.elements[11] -
		mat.elements[12] * mat.elements[7] * mat.elements[9];

	result.elements[12] = -mat.elements[4] * mat.elements[9] * mat.elements[14] +
		mat.elements[4] * mat.elements[10] * mat.elements[13] +
		mat.elements[8] * mat.elements[5] * mat.elements[14] -
		mat.elements[8] * mat.elements[6] * mat.elements[13] -
		mat.elements[12] * mat.elements[5] * mat.elements[10] +
		mat.elements[12] * mat.elements[6] * mat.elements[9];

	result.elements[1] = -mat.elements[1] * mat.elements[10] * mat.elements[15] +
		mat.elements[1] * mat.elements[11] * mat.elements[14] +
		mat.elements[9] * mat.elements[2] * mat.elements[15] -
		mat.elements[9] * mat.elements[3] * mat.elements[14] -
		mat.elements[13] * mat.elements[2] * mat.elements[11] +
		mat.elements[13] * mat.elements[3] * mat.elements[10];

	result.elements[5] = mat.elements[0] * mat.elements[10] * mat.elements[15] -
		mat.elements[0] * mat.elements[11] * mat.elements[14] -
		mat.elements[8] * mat.elements[2] * mat.elements[15] +
		mat.elements[8] * mat.elements[3] * mat.elements[14] +
		mat.elements[12] * mat.elements[2] * mat.elements[11] -
		mat.elements[12] * mat.elements[3] * mat.elements[10];

	result.elements[9] = -mat.elements[0] * mat.elements[9] * mat.elements[15] +
		mat.elements[0] * mat.elements[11] * mat.elements[13] +
		mat.elements[8] * mat.elements[1] * mat.elements[15] -
		mat.elements[8] * mat.elements[3] * mat.elements[13] -
		mat.elements[12] * mat.elements[1] * mat.elements[11] +
		mat.elements[12] * mat.elements[3] * mat.elements[9];

	result.elements[13] = mat.elements[0] * mat.elements[9] * mat.elements[14] -
		mat.elements[0] * mat.elements[10] * mat.elements[13] -
		mat.elements[8] * mat.elements[1] * mat.elements[14] +
		mat.elements[8] * mat.elements[2] * mat.elements[13] +
		mat.elements[12] * mat.elements[1] * mat.elements[10] -
		mat.elements[12] * mat.elements[2] * mat.elements[9];

	result.elements[2] = mat.elements[1] * mat.elements[6] * mat.elements[15] -
		mat.elements[1] * mat.elements[7] * mat.elements[14] -
		mat.elements[5] * mat.elements[2] * mat.elements[15] +
		mat.elements[5] * mat.elements[3] * mat.elements[14] +
		mat.elements[13] * mat.elements[2] * mat.elements[7] -
		mat.elements[13] * mat.elements[3] * mat.elements[6];

	result.elements[6] = -mat.elements[0] * mat.elements[6] * mat.elements[15] +
		mat.elements[0] * mat.elements[7] * mat.elements[14] +
		mat.elements[4] * mat.elements[2] * mat.elements[15] -
		mat.elements[4] * mat.elements[3] * mat.elements[14] -
		mat.elements[12] * mat.elements[2] * mat.elements[7] +
		mat.elements[12] * mat.elements[3] * mat.elements[6];

	result.elements[10] = mat.elements[0] * mat.elements[5] * mat.elements[15] -
		mat.elements[0] * mat.elements[7] * mat.elements[13] -
		mat.elements[4] * mat.elements[1] * mat.elements[15] +
		mat.elements[4] * mat.elements[3] * mat.elements[13] +
		mat.elements[12] * mat.elements[1] * mat.elements[7] -
		mat.elements[12] * mat.elements[3] * mat.elements[5];

	result.elements[14] = -mat.elements[0] * mat.elements[5] * mat.elements[14] +
		mat.elements[0] * mat.elements[6] * mat.elements[13] +
		mat.elements[4] * mat.elements[1] * mat.elements[14] -
		mat.elements[4] * mat.elements[2] * mat.elements[13] -
		mat.elements[12] * mat.elements[1] * mat.elements[6] +
		mat.elements[12] * mat.elements[2] * mat.elements[5];

	result.elements[3] = -mat.elements[1] * mat.elements[6] * mat.elements[11] +
		mat.elements[1] * mat.elements[7] * mat.elements[10] +
		mat.elements[5] * mat.elements[2] * mat.elements[11] -
		mat.elements[5] * mat.elements[3] * mat.elements[10] -
		mat.elements[9] * mat.elements[2] * mat.elements[7] +
		mat.elements[9] * mat.elements[3] * mat.elements[6];

	result.elements[7] = mat.elements[0] * mat.elements[6] * mat.elements[11] -
		mat.elements[0] * mat.elements[7] * mat.elements[10] -
		mat.elements[4] * mat.elements[2] * mat.elements[11] +
		mat.elements[4] * mat.elements[3] * mat.elements[10] +
		mat.elements[8] * mat.elements[2] * mat.elements[7] -
		mat.elements[8] * mat.elements[3] * mat.elements[6];

	result.elements[11] = -mat.elements[0] * mat.elements[5] * mat.elements[11] +
		mat.elements[0] * mat.elements[7] * mat.elements[9] +
		mat.elements[4] * mat.elements[1] * mat.elements[11] -
		mat.elements[4] * mat.elements[3] * mat.elements[9] -
		mat.elements[8] * mat.elements[1] * mat.elements[7] +
		mat.elements[8] * mat.elements[3] * mat.elements[5];

	result.elements[15] = mat.elements[0] * mat.elements[5] * mat.elements[10] -
		mat.elements[0] * mat.elements[6] * mat.elements[9] -
		mat.elements[4] * mat.elements[1] * mat.elements[10] +
		mat.elements[4] * mat.elements[2] * mat.elements[9] +
		mat.elements[8] * mat.elements[1] * mat.elements[6] -
		mat.elements[8] * mat.elements[2] * mat.elements[5];

	f32 det(mat.elements[0] * result.elements[0] + mat.elements[1] * result.elements[4] + mat.elements[2] * result.elements[8] + mat.elements[3] * result.elements[12]);
	det = 1.0f / det;

	for (i32 i = 0; i < 4; i++)
		result.rows[i] = result.rows[i] * det;
	return result;
}

m4x4 m4x4::Identity()
{
	return m4x4();
}

m4x4 m4x4::ScaleR(v3 vec)
{
	m4x4 result;
	result.elements[0 * 4 + 0] = vec.x;
	result.elements[1 * 4 + 1] = vec.y;
	result.elements[2 * 4 + 2] = vec.z;
	return result;
}

m4x4 m4x4::TranslationR(v3 vec)
{
	m4x4 result;
	result.elements[3 * 4 + 0] = vec.x;
	result.elements[3 * 4 + 1] = vec.y;
	result.elements[3 * 4 + 2] = vec.z;
	return result;
}

m4x4 m4x4::RotationR(f32 angle, v3 axis)
{
	f32 c = Cosf(angle);
	f32 s = Sinf(angle);

	f32 x2 = axis.x * axis.x;
	f32 xy = axis.x * axis.y;
	f32 zx = axis.x * axis.z;
	f32 y2 = axis.y * axis.y;
	f32 yz = axis.y * axis.z;
	f32 z2 = axis.z * axis.z;

	m4x4 result;
	result.elements[0 * 4 + 0] = c + x2 * (1 - c);
	result.elements[0 * 4 + 1] = xy * (1 - c) + axis.z*s;
	result.elements[0 * 4 + 2] = zx * (1 - c) - axis.y*s;
	result.elements[1 * 4 + 0] = xy * (1 - c) - axis.z*s;
	result.elements[1 * 4 + 1] = c + y2 * (1 - c);
	result.elements[1 * 4 + 2] = yz * (1 - c) + axis.x*s;
	result.elements[2 * 4 + 0] = zx * (1 - c) + axis.y*s;
	result.elements[2 * 4 + 1] = yz * (1 - c) - axis.x*s;
	result.elements[2 * 4 + 2] = c + z2 * (1 - c);
	result.elements[3 * 4 + 3] = 1.0f;
	return result;
}

m4x4 m4x4::LookR(v3 from, v3 to, v3 worldUp)
{
	v3 fwd = Normalize(to - from);
	v3 right = Normalize(Cross(fwd, worldUp));
	v3 up = Cross(right, fwd);
	m4x4 result;
	result.rows[0] = v4{ right.x, right.y, right.z, 0.0f };
	result.rows[1] = v4{ up.x, up.y, up.z, 0.0f };
	result.rows[2] = -v4{ fwd.x, fwd.y, fwd.z, 0.0f };
	result.rows[3] = v4{ from.x, from.y, from.z, 1.0f };
	return result;
}

// NOTE(Zero): Windows header file defines `far` and `near`, so not to use these as variables
m4x4 m4x4::OrthographicR(f32 left, f32 right, f32 bottom, f32 top, f32 cNear, f32 cFar)
{
	m4x4 result;
	result.elements[0 * 4 + 0] = 2.0f / (right - left);
	result.elements[1 * 4 + 1] = 2.0f / (top - bottom);
	result.elements[2 * 4 + 2] = -2.0f / (cFar - cNear);
	result.elements[3 * 4 + 0] = -(right + left) / (right - left);
	result.elements[3 * 4 + 1] = -(top + bottom) / (top - bottom);
	result.elements[3 * 4 + 2] = -(cFar + cNear) / (cFar - cNear);
	result.elements[3 * 4 + 3] = 1.0f;
	return result;
}

// NOTE(Zero): Windows header file defines `far` and `near`, so not to use these as variables
m4x4 m4x4::PerspectiveR(f32 fov, f32 aspectRatio, f32 cNear, f32 cFar)
{
	f32 cot = 1.0f / Tanf(fov * 0.5f);
	m4x4 result;
	result.elements[0 * 4 + 0] = cot / aspectRatio;
	result.elements[1 * 4 + 1] = cot;
	result.elements[2 * 4 + 2] = -(cFar + cNear) / (cFar - cNear);
	result.elements[2 * 4 + 3] = -1.0f;
	result.elements[3 * 4 + 2] = (-2.0f * cFar * cNear) / (cFar - cNear);
	result.elements[3 * 4 + 3] = 0.0f;
	return result;
}

m4x4 m4x4::ScaleC(v3 vec)
{
	return m4x4::Transpose(m4x4::ScaleR(vec));
}

m4x4 m4x4::TranslationC(v3 vec)
{
	return m4x4::Transpose(m4x4::TranslationR(vec));
}

m4x4 m4x4::RotationC(f32 angle, v3 axis)
{
	return m4x4::Transpose(m4x4::RotationR(angle, axis));
}

m4x4 m4x4::LookC(v3 from, v3 to, v3 up)
{
	return m4x4::Transpose(m4x4::LookR(from, to, up));
}

// NOTE(Zero): Windows header file defines `far` and `near`, so not to use these as variables
m4x4 m4x4::OrthographicC(f32 left, f32 right, f32 bottom, f32 top, f32 cNear, f32 cFar)
{
	return m4x4::Transpose(m4x4::OrthographicR(left, right, bottom, top, cNear, cFar));
}

// NOTE(Zero): Windows header file defines `far` and `near`, so not to use these as variables
m4x4 m4x4::PerspectiveC(f32 fov, f32 aspectRatio, f32 cNear, f32 cFar)
{
	return m4x4::Transpose(m4x4::PerspectiveR(fov, aspectRatio, cNear, cFar));
}

#endif