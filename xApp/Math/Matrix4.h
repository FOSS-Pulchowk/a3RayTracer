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

