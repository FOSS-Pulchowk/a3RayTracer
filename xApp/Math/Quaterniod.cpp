#include "Quaterniod.h"

quat::quat() : r(1.0f), i(0.0f), j(0.0f), k(0.0f)
{
}

quat::quat(const quat &q) : r(q.r), i(q.i), j(q.j), k(q.k)
{
}

quat::quat(f32 r, f32 i, f32 j, f32 k) : r(r), i(i), j(j), k(k)
{
}

quat &quat::operator=(const quat &q)
{
	if(this == &q)
		return *this;
	r = q.r;
	i = q.i;
	j = q.j;
	k = q.k;
	return *this;
}

quat &quat::operator+=(const quat &q)
{
	r += q.r;
	i += q.i;
	j += q.j;
	k += q.k;
	return *this;
}

quat &quat::operator-=(const quat &q)
{
	r -= q.r;
	i -= q.i;
	j -= q.j;
	k -= q.k;
	return *this;
}

quat &quat::operator*=(const quat &q)
{
	quat temp;
	temp.r = r * q.r - i * q.i - j * q.j - k * q.k;
	temp.i = r * q.i + i * q.r + j * q.k - k * q.j;
	temp.j = r * q.j - i * q.k + j * q.r + k * q.i;
	temp.k = r * q.k + i * q.j - j * q.i + k * q.r;
	*this = temp;
	return *this;
}

f32 quat::Length(const quat &q)
{
	return xSqrtf(q.r * q.r + q.i * q.i + q.j * q.j + q.k * q.k);
}

quat quat::Conjugate(const quat &q)
{
	return quat(q.r, -q.i, -q.j, -q.k);
}

quat quat::Normalize(const quat &q)
{
	f32 len = xSqrtf(q.r * q.r + q.i * q.i + q.j * q.j + q.k * q.k);
	len = 1.0f / len;
	return quat(q.r * len, q.i * len, q.j * len, q.k * len);
}

f32 quat::Dot(const quat &q1, const quat &q2)
{
	return (q1.r * q2.r + q1.i + q2.i + q1.j * q2.j + q1.k * q2.k);
}

quat operator*(const quat &lhs, const quat &rhs)
{
	quat ret;
	ret.r = lhs.r * rhs.r - lhs.i * rhs.i - lhs.j * rhs.j - lhs.k * rhs.k;
	ret.i = lhs.r * rhs.i + lhs.i * rhs.r + lhs.j * rhs.k - lhs.k * rhs.j;
	ret.j = lhs.r * rhs.j - lhs.i * rhs.k + lhs.j * rhs.r + lhs.k * rhs.i;
	ret.k = lhs.r * rhs.k + lhs.i * rhs.j - lhs.j * rhs.i + lhs.k * rhs.r;
	return ret;
}
