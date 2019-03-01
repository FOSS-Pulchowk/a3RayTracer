#pragma once
#include "Common/Core.h"
#include "Matrix4.h"
#include "Quaterniod.h"

#define a3Pi32 3.1415926535f
#define a3ToRadians(deg) ((deg)*a3Pi32 / 180)
#define a3ToDegrees(rad) ((rad)*180 / a3Pi32)

//
// DECLARATIONS
//

struct rect
{
	i32 x, y;
	i32 w, h;
};

inline b32 operator==(const rect& l, const rect& r);
inline b32 operator!=(const rect& l, const rect& r);

// Overloaded mathematical functions for matrix and quaternions below

inline v4 operator*(v4 vec, const m4x4 &mat);
inline v3 operator*(v3 vec, const m4x4 &mat); // Treats w as 1.0f
inline v4 operator*(const m4x4 &mat, v4 vec);
inline v3 operator*(const m4x4 &mat, v3 vec); // Treats w as 1.0f

/*
	- Construct Quaternion from euler angles
	- Euler angles should be provided using `v3` class
	- Angles should be provided in radians
	- [USAGE] Euler angles take the Order Yaw-Pitch-Roll (X-Y-Z in `v3` class)
	- [IMLPEMENTATION] Roll does rotation along Z, Pitch along X and Yaw along Y
*/
inline quat EulerAnglesToQuat(v3 eulerAngles);

/*
	- Returns Euler Angles of given Quaternion
	- Angles returned are in radians
	- Euler angles is be provided using `v3` class
	- [USAGE] Euler angles take the Order Yaw-Pitch-Roll (X-Y-Z in `v3` class)
	- [IMLPEMENTATION] Roll does rotation along Z, Pitch along X and Yaw along Y
*/
inline v3 QuatToEulerAngles(const quat &q);

/*
	- Returns a 4x4 Rotation Matrix
	- The matrix can be used as rotation matrix in 3D space
	- Errors can occurs if the provided Quaternion is either null or not unit quaternion
*/
inline m4x4 QuatToMat4x4R(const quat &q);
inline m4x4 QuatMat4x4C(const quat &q);

/*
	- Returns rows or column of the matrix transformation
	- This is useful when we need forward, right or up vector
	- Errors can occurs if the provided Quaternion is either null or not unit quaternion
*/
inline v3 GetAxis0R(const quat& q);
inline v3 GetAxis1R(const quat& q);
inline v3 GetAxis2R(const quat& q);
inline v3 GetAxis0C(const quat& q);
inline v3 GetAxis1C(const quat& q);
inline v3 GetAxis2C(const quat& q);

/*
	- Constructs Quaternion from angle and axis
	- Angle should be provided in radians
	- Axis must be unit vector, this does not normalize the axis
*/
inline quat AngleAxisToQuat(f32 angle, v3 axis);

/*
	- Returns angles and axis representation by the unit quaternion
	- Unit quaternion should be provided, this does not normalize the quaternion
	- If the provided Quaternion is not unit Quaternion, returned value may cause errors
	- Pointer for `f32` and `v3` is used for returning
	- Angle returned will be in radians
*/
inline f32 GetAngleFromQuat(const quat& q);
inline v3 GetAxisFromQuat(const quat& q);
inline void QuatToAngleAxis(const quat &q, f32 *angle, v3 *axis);

/*
	- Rotates the given Vector 3 using quaternion multiplication
	- This function does not check if the quaternion is normalized or not
	- Normalize the quaternion if it is not; unless you know what you are doing
*/
inline v3 operator*(const quat &lhs, v3 vec);

/*
	- Rotates Vector 3 but takes Vector 4 as paramater
	- Discards the `w` component of the Vector 4 while rotating
	- When Vector 4 is returned, the `w` component will always be set to `1.0f`
*/
inline v4 operator*(const quat &lhs, v4 vec);


//
// IMPLEMENTATION
//

////////////////////// Matrix Vector operations ///////////////////////////////////////

inline b32 operator==(const rect & l, const rect & r)
{
	return (l.x == r.x) && (l.y == r.y) && (l.w == r.w) && (l.h == r.h);
}

inline b32 operator!=(const rect & l, const rect & r)
{
	return !(l == r);
}

inline v4 operator*(v4 vec, const m4x4 &mat)
{
	v4 result;
	result.x = vec.x * mat.elements[0] + vec.y * mat.elements[4] + vec.z * mat.elements[8] + vec.w * mat.elements[12];
	result.y = vec.x * mat.elements[1] + vec.y * mat.elements[5] + vec.z * mat.elements[9] + vec.w * mat.elements[13];
	result.z = vec.x * mat.elements[2] + vec.y * mat.elements[6] + vec.z * mat.elements[10] + vec.w * mat.elements[14];
	result.w = vec.x * mat.elements[3] + vec.y * mat.elements[7] + vec.z * mat.elements[11] + vec.w * mat.elements[15];
	return result;
}

inline v3 operator*(v3 vec, const m4x4 &mat)
{
	v4 result;
	result.xyz = vec;
	result.w = 1.0f;
	result = result * mat;
	return result.xyz;
}

inline v4 operator*(const m4x4 &mat, v4 vec)
{
	v4 result;
	result.x = Dot(mat.rows[0], vec);
	result.y = Dot(mat.rows[1], vec);
	result.z = Dot(mat.rows[2], vec);
	result.w = Dot(mat.rows[3], vec);
	return result;
}

inline v3 operator*(const m4x4 &mat, v3 vec)
{
	v4 result;
	result.xyz = vec;
	result.w = 1.0f;
	result = mat * result;
	return result.xyz;
}

////////////////////////// Quaternions, Vector, Matrix operations ///////////////////////////


inline quat EulerAnglesToQuat(v3 eulerAngles)
{
	quat q;
	f32 cy = Cosf(-eulerAngles.y * 0.5f);
	f32 sy = Sinf(-eulerAngles.y * 0.5f);
	f32 cr = Cosf(eulerAngles.z * 0.5f);
	f32 sr = Sinf(eulerAngles.z * 0.5f);
	f32 cp = Cosf(eulerAngles.x * 0.5f);
	f32 sp = Sinf(eulerAngles.x * 0.5f);
	q.r = cy * cr * cp + sy * sr * sp;
	q.i = cy * sr * cp - sy * cr * sp;
	q.j = cy * cr * sp + sy * sr * cp;
	q.k = sy * cr * cp - cy * sr * sp;
	return q;
}

inline v3 QuatToEulerAngles(const quat &q)
{
	v3 ret;
	f32 sinr = 2.0f * (q.r * q.i + q.j * q.k);
	f32 cosr = 1.0f - 2.0f * (q.i * q.i + q.j * q.j);

	ret.z = ArcTan2f(sinr, cosr);
	f32 sinp = 2.0f * (q.r * q.j - q.k * q.i);
	if(FAbsf(sinp) >= 1.0f)
		ret.x = CopySignf(3.141592f * 0.5f, sinp);
	else
		ret.x = ArcSinf(sinp);
	f32 siny = 2.0f * (q.r * q.k + q.i * q.j);
	f32 cosy = 1.0f - 2.0f * (q.j * q.j + q.k * q.k);
	ret.y = ArcTan2f(siny, cosy);

	return ret;
}

inline m4x4 QuatToMat4x4R(const quat &q)
{
	m4x4 res;
	res.rows[0].xyz = GetAxis0R(q);
	res.rows[1].xyz = GetAxis1R(q);
	res.rows[2].xyz = GetAxis2R(q);
	return res;
}

inline m4x4 QuatMat4x4C(const quat &q)
{
	return m4x4::Transpose(QuatToMat4x4R(q));
}

inline v3 GetAxis0R(const quat & q)
{
	v3 res;
	res.x = q.r * q.r + q.i * q.i - q.j * q.j - q.k * q.k;
	res.y = 2.0f * (q.i * q.j + q.k * q.r);
	res.z = 2.0f * (q.i * q.k - q.j * q.r);
	return res;
}

inline v3 GetAxis1R(const quat & q)
{
	v3 res;
	res.x = 2.0f * (q.i * q.j - q.k * q.r);
	res.y = q.r * q.r - q.i * q.i + q.j * q.j - q.k * q.k;
	res.z = 2.0f * (q.j * q.k + q.i * q.r);
	return res;
}

inline v3 GetAxis2R(const quat & q)
{
	v3 res;
	res.x = 2.0f * (q.i * q.k + q.j * q.r);
	res.y = 2.0f * (q.j * q.k - q.i * q.r);
	res.z = q.r * q.r - q.i * q.i - q.j * q.j + q.k * q.k;
	return res;
}

inline v3 GetAxis0C(const quat & q)
{
	v3 res;
	res.x = q.r * q.r + q.i * q.i - q.j * q.j - q.k * q.k;
	res.y = 2.0f * (q.i * q.j - q.k * q.r);
	res.z = 2.0f * (q.i * q.k + q.j * q.r);
	return res;
}

inline v3 GetAxis1C(const quat & q)
{
	v3 res;
	res.x = 2.0f * (q.i * q.j + q.k * q.r);
	res.y = q.r * q.r - q.i * q.i + q.j * q.j - q.k * q.k;
	res.z = 2.0f * (q.j * q.k - q.i * q.r);
	return res;
}

inline v3 GetAxis2C(const quat & q)
{
	v3 res;
	res.x = 2.0f * (q.i * q.k - q.j * q.r);
	res.y = 2.0f * (q.j * q.k + q.i * q.r);
	res.z = q.r * q.r - q.i * q.i - q.j * q.j + q.k * q.k;
	return res;
}

inline quat AngleAxisToQuat(f32 angle, v3 axis)
{
	quat q;
	q.r = Cosf(angle * 0.5f);
	f32 s = Sinf(angle * 0.5f);
	q.i = s * axis.x;
	q.j = s * axis.y;
	q.k = s * axis.z;
	return q;
}

inline f32 GetAngleFromQuat(const quat & q)
{
	f32 len = q.i * q.i + q.j * q.j + q.k * q.k;
	return (2.0f * ArcTan2f(len, q.r));
}

inline v3 GetAxisFromQuat(const quat & q)
{
	v3 axis;
	f32 len = q.i * q.i + q.j * q.j + q.k * q.k;
	len = 1.0f / len;
	axis.x = q.i * len;
	axis.y = q.j * len;
	axis.z = q.k * len;
	return axis;
}

inline void QuatToAngleAxis(const quat &q, f32 *angle, v3 *axis)
{
	*angle = GetAngleFromQuat(q);
	*axis = GetAxisFromQuat(q);
}

inline v3 operator*(const quat &lhs, v3 vec)
{
	quat v(0.0f, vec.x, vec.y, vec.z);
	quat lhsp = quat::Conjugate(lhs);
	quat vp((lhs * v) * lhsp);
	return v3{ vp.x, vp.y, vp.z };
}

inline v4 operator*(const quat &lhs, v4 vec)
{
	quat v(0.0f, vec.x, vec.y, vec.z);
	quat lhsp = quat::Conjugate(lhs);
	quat vp((lhs * v) * lhsp);
	return v4{ vp.x, vp.y, vp.z, 1.0f };
}
