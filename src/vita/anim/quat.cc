#include "quat.h"
#include <cmath>
#include <iostream>

const float* quat::data_ptr() const
{
	return &x;
}

quat::quat()
	: x(0)
	, y(0)
	, z(0)
	, w(1)
{
}

quat::quat(float _x, float _y, float _z, float _w)
	: x(_x)
	, y(_y)
	, z(_z)
	, w(_w)
{
}

vec3 quat::vector() const
{
	return {x, y, z};
}

float quat::scalar() const
{
	return w;
}

vec3 quat::get_axis() const
{
	return get_normalized(vec3(x, y, z));
}

float quat::get_angle() const
{
	return 2.0f * acosf(w);
}

quat quat_from_angle_axis(float angle, const vec3& axis)
{
	const auto norm = get_normalized(axis);
	const auto s = sinf(angle * 0.5f);

	return quat(norm.x * s, norm.y * s, norm.z * s, cosf(angle * 0.5f));
}

quat quat_from_rotation(const vec3& from, const vec3& to)
{
	const auto f = get_normalized(from);
	const auto t = get_normalized(to);

	if (f == t)
	{
		return quat();
	}
	else if (f == t * -1.0f)
	{
		vec3 ortho = vec3(1, 0, 0);
		if (std::abs(f.y) < std::abs(f.x))
		{
			ortho = vec3(0, 1, 0);
		}
		if (std::abs(f.z) < std::abs(f.y) && std::abs(f.z) < std::abs(f.x))
		{
			ortho = vec3(0, 0, 1);
		}

		const auto axis = get_normalized(cross(f, ortho));
		return quat(axis.x, axis.y, axis.z, 0);
	}

	const auto half = get_normalized(f + t);
	const auto axis = cross(f, half);
	return quat(axis.x, axis.y, axis.z, dot(f, half));
}

quat operator+(const quat& a, const quat& b)
{
	return quat(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

quat operator-(const quat& a, const quat& b)
{
	return quat(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

quat operator*(const quat& a, float b)
{
	return quat(a.x * b, a.y * b, a.z * b, a.w * b);
}

quat operator-(const quat& q)
{
	return quat(-q.x, -q.y, -q.z, -q.w);
}

bool operator==(const quat& left, const quat& right)
{
	return (
		fabsf(left.x - right.x) <= QUAT_EPSILON && fabsf(left.y - right.y) <= QUAT_EPSILON
		&& fabsf(left.z - right.z) <= QUAT_EPSILON && fabsf(left.w - right.w) <= QUAT_EPSILON
	);
}

bool operator!=(const quat& a, const quat& b)
{
	return ! (a == b);
}

bool is_same_orientation(const quat& left, const quat& right)
{
	return (fabsf(left.x - right.x) <= QUAT_EPSILON && fabsf(left.y - right.y) <= QUAT_EPSILON
			&& fabsf(left.z - right.z) <= QUAT_EPSILON && fabsf(left.w - right.w) <= QUAT_EPSILON)
		|| (fabsf(left.x + right.x) <= QUAT_EPSILON && fabsf(left.y + right.y) <= QUAT_EPSILON
			&& fabsf(left.z + right.z) <= QUAT_EPSILON && fabsf(left.w + right.w) <= QUAT_EPSILON);
}

float dot(const quat& a, const quat& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

float get_length_sq(const quat& q)
{
	return q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
}

float get_length(const quat& q)
{
	const auto lenSq = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
	if (lenSq < QUAT_EPSILON)
	{
		return 0.0f;
	}
	return sqrtf(lenSq);
}

void normalize(quat& q)
{
	const auto lenSq = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
	if (lenSq < QUAT_EPSILON)
	{
		return;
	}
	const auto i_len = 1.0f / sqrtf(lenSq);

	q.x *= i_len;
	q.y *= i_len;
	q.z *= i_len;
	q.w *= i_len;
}

quat get_normalized(const quat& q)
{
	const auto lenSq = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
	if (lenSq < QUAT_EPSILON)
	{
		return quat();
	}
	const auto i_len = 1.0f / sqrtf(lenSq);

	return quat(q.x * i_len, q.y * i_len, q.z * i_len, q.w * i_len);
}

quat get_conjugate(const quat& q)
{
	return quat(-q.x, -q.y, -q.z, q.w);
}

quat get_inverse(const quat& q)
{
	const auto lenSq = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
	if (lenSq < QUAT_EPSILON)
	{
		return quat();
	}
	float recip = 1.0f / lenSq;

	// conjugate / norm
	return quat(-q.x * recip, -q.y * recip, -q.z * recip, q.w * recip);
}

quat operator*(const quat& Q1, const quat& Q2)
{
	return quat(
		Q2.x * Q1.w + Q2.y * Q1.z - Q2.z * Q1.y + Q2.w * Q1.x,
		-Q2.x * Q1.z + Q2.y * Q1.w + Q2.z * Q1.x + Q2.w * Q1.y,
		Q2.x * Q1.y - Q2.y * Q1.x + Q2.z * Q1.w + Q2.w * Q1.z,
		-Q2.x * Q1.x - Q2.y * Q1.y - Q2.z * Q1.z + Q2.w * Q1.w
	);
}

vec3 operator*(const quat& q, const vec3& v)
{
	return q.vector() * 2.0f * dot(q.vector(), v)
		 + v * (q.scalar() * q.scalar() - dot(q.vector(), q.vector()))
		 + cross(q.vector(), v) * 2.0f * q.scalar();
}

quat get_mixed(const quat& from, const quat& to, float t)
{
	return from * (1.0f - t) + to * t;
}

quat nlerp(const quat& from, const quat& to, float t)
{
	return get_normalized(from + (to - from) * t);
}

quat operator^(const quat& q, float f)
{
	const auto angle = 2.0f * acosf(q.scalar());
	const auto axis = get_normalized(q.vector());

	const auto halfCos = cosf(f * angle * 0.5f);
	const auto halfSin = sinf(f * angle * 0.5f);

	return quat(axis.x * halfSin, axis.y * halfSin, axis.z * halfSin, halfCos);
}

quat slerp(const quat& start, const quat& end, float t)
{
	if (fabsf(dot(start, end)) > 1.0f - QUAT_EPSILON)
	{
		return nlerp(start, end, t);
	}

	return get_normalized(((get_inverse(start) * end) ^ t) * start);
}

quat quat_from_look_rotation(const vec3& direcion, const vec3& up)
{
	// Find orthonormal basis vectors
	const auto f = get_normalized(direcion);
	vec3 u = get_normalized(up);
	const auto r = cross(u, f);
	u = cross(f, r);

	// From world forward to object forward
	const auto f2d = quat_from_rotation(vec3(0, 0, 1), f);

	// what direction is the new object up?
	const auto objectUp = f2d * vec3(0, 1, 0);
	// From object up to desired up
	const auto u2u = quat_from_rotation(objectUp, u);

	// Rotate to forward direction first, then twist to correct up
	const auto result = f2d * u2u;
	// Don't forget to normalize the result
	return get_normalized(result);
}

mat4 mat4_from_quat(const quat& q)
{
	const auto r = q * vec3(1, 0, 0);
	const auto u = q * vec3(0, 1, 0);
	const auto f = q * vec3(0, 0, 1);

	return mat4(r.x, r.y, r.z, 0, u.x, u.y, u.z, 0, f.x, f.y, f.z, 0, 0, 0, 0, 1);
}

quat quat_from_mat4(const mat4& m)
{
	vec3 up = get_normalized(vec3(m.up().x, m.up().y, m.up().z));
	const auto forward = get_normalized(vec3(m.forward().x, m.forward().y, m.forward().z));
	const auto right = cross(up, forward);
	up = cross(forward, right);

	return quat_from_look_rotation(forward, up);
}
