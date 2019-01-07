#pragma once
#include "Common/Core.h"
#include "Matrix4.h"
#include "Quaterniod.h"

#define xPi32 3.1415926535f
#define xToRadians(deg) ((deg)*MathPi / 180)
#define xToDegrees(rad) ((rad)*180 / MathPi)

////////////////////// Matrix Vector operations ///////////////////////////////////////

inline v4 operator*(v4 vec, const m4x4 &mat)
{
	v4 result;
	result.x = vec.x * mat.elements[0] + vec.y * mat.elements[4] + vec.z * mat.elements[8] + vec.w * mat.elements[12];
	result.y = vec.x * mat.elements[1] + vec.y * mat.elements[5] + vec.z * mat.elements[9] + vec.w * mat.elements[13];
	result.z = vec.x * mat.elements[2] + vec.y * mat.elements[6] + vec.z * mat.elements[10] + vec.w * mat.elements[14];
	result.w = vec.x * mat.elements[3] + vec.y * mat.elements[7] + vec.z * mat.elements[11] + vec.w * mat.elements[15];
	return result;
}

// Treats w as 1.0f
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

// Treats w as 1.0f
inline v3 operator*(const m4x4 &mat, v3 vec)
{
	v4 result;
	result.xyz = vec;
	result.w = 1.0f;
	result = mat * result;
	return result.xyz;
}

////////////////////////// Quaternions, Vector, Matrix operations ///////////////////////////

/*
			- Construct Quaternion from euler angles
			- Euler angles should be provided using `v3` class
			- Angles should be provided in radians
			- [USAGE] Euler angles take the Order Yaw-Pitch-Roll (X-Y-Z in `v3` class)
			- [IMLPEMENTATION] Roll does rotation along Z, Pitch along X and Yaw along Y
		*/
inline quat EulerAnglesToQuat(v3 eulerAngles)
{
	quat q;
	f32 cy = cosf(-eulerAngles.y * 0.5f);
	f32 sy = sinf(-eulerAngles.y * 0.5f);
	f32 cr = cosf(eulerAngles.z * 0.5f);
	f32 sr = sinf(eulerAngles.z * 0.5f);
	f32 cp = cosf(eulerAngles.x * 0.5f);
	f32 sp = sinf(eulerAngles.x * 0.5f);
	q.r = cy * cr * cp + sy * sr * sp;
	q.i = cy * sr * cp - sy * cr * sp;
	q.j = cy * cr * sp + sy * sr * cp;
	q.k = sy * cr * cp - cy * sr * sp;
	return q;
}

/*
			- Returns Euler Angles of given Quaternion
			- Angles returned are in radians
			- Euler angles is be provided using `v3` class
			- [USAGE] Euler angles take the Order Yaw-Pitch-Roll (X-Y-Z in `v3` class)
			- [IMLPEMENTATION] Roll does rotation along Z, Pitch along X and Yaw along Y
		*/
inline v3 QuatToEulerAngles(const quat &q)
{
	v3 ret;
	f32 sinr = 2.0f * (q.r * q.i + q.j * q.k);
	f32 cosr = 1.0f - 2.0f * (q.i * q.i + q.j * q.j);

	ret.z = atan2f(sinr, cosr);
	f32 sinp = 2.0f * (q.r * q.j - q.k * q.i);
	if(fabs(sinp) >= 1.0f)
		ret.x = copysignf(3.141592f * 0.5f, sinp);
	else
		ret.x = asinf(sinp);
	f32 siny = 2.0f * (q.r * q.k + q.i * q.j);
	f32 cosy = 1.0f - 2.0f * (q.j * q.j + q.k * q.k);
	ret.y = atan2f(siny, cosy);

	return ret;
}

/*
			- Returns a 4x4 Rotation Matrix
			- The matrix can be used as rotation matrix in 3D space
			- Errors can occurs if the provided Quaternion is either null or not unit quaternion
		*/
inline m4x4 QuatToMat4x4R(const quat &q)
{
	return m4x4(
		q.r * q.r + q.i * q.i - q.j * q.j - q.k * q.k, 2.0f * (q.i * q.j + q.k * q.r), 2.0f * (q.i * q.k - q.j * q.r), 0.0f,
		2.0f * (q.i * q.j - q.k * q.r), q.r * q.r - q.i * q.i + q.j * q.j - q.k * q.k, 2.0f * (q.j * q.k + q.i * q.r), 0.0f,
		2.0f * (q.i * q.k + q.j * q.r), 2.0f * (q.j * q.k - q.i * q.r), q.r * q.r - q.i * q.i - q.j * q.j + q.k * q.k, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
}

inline m4x4 QuatMat4x4C(const quat &q)
{
	return m4x4::Transpose(QuatToMat4x4R(q));
}

/*
			- Constructs Quaternion from angle and axis
			- Angle should be provided in radians
			- Axis must be unit vector, this does not normalize the axis
		*/
inline quat AngleAxisToQuat(f32 angle, v3 axis)
{
	quat q;
	q.r = cosf(angle * 0.5f);
	f32 s = sinf(angle * 0.5f);
	q.i = s * axis.x;
	q.j = s * axis.y;
	q.k = s * axis.z;
	return q;
}

/*
			- Returns angles and axis representation by the unit quaternion
			- Unit quaternion should be provided, this does not normalize the quaternion
			- If the provided Quaternion is not unit Quaternion, returned value may cause errors
			- Pointer for `f32` and `v3` is used for returning
			- Angle returned will be in radians
		*/
inline void QuatToAngleAxis(const quat &q, f32 *angle, v3 *axis)
{
	f32 len = q.i * q.i + q.j * q.j + q.k * q.k;
	*angle = 2.0f * atan2f(len, q.r);
	len = 1.0f / len;
	axis->x = q.i * len;
	axis->y = q.j * len;
	axis->z = q.k * len;
}

/*
	- Rotates the given Vector 3 using quaternion multiplication
	- This function does not check if the quaternion is normalized or not
	- Normalize the quaternion if it is not; unless you know what you are doing
	*/
inline v3 operator*(const quat &lhs, v3 vec)
{
	quat v(0.0f, vec.x, vec.y, vec.z);
	quat lhsp = quat::Conjugate(lhs);
	quat vp((lhs * v) * lhsp);
	return v3{ vp.x, vp.y, vp.z };
}
/*
	- Rotates Vector 3 but takes Vector 4 as paramater
	- Discards the `w` component of the Vector 4 while rotating
	- When Vector 4 is returned, the `w` component will always be set to `1.0f`
	*/
inline v4 operator*(const quat &lhs, v4 vec)
{
	quat v(0.0f, vec.x, vec.y, vec.z);
	quat lhsp = quat::Conjugate(lhs);
	quat vp((lhs * v) * lhsp);
	return v4{ vp.x, vp.y, vp.z, 1.0f };
}
