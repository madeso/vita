#include <cmath>
#include <iostream>

#include "vita/anim/vec3.h"

const float* vec3::data_ptr() const
{
	return &x;
}

vec3::vec3()
	: x(0.0f)
	, y(0.0f)
	, z(0.0f)
{
}

vec3::vec3(float ax, float ay, float az)
	: x(ax)
	, y(ay)
	, z(az)
{
}

vec3::vec3(float* fv)
	: x(fv[0])
	, y(fv[1])
	, z(fv[2])
{
}

vec3 operator+(const vec3& l, const vec3& r)
{
	return vec3(l.x + r.x, l.y + r.y, l.z + r.z);
}

vec3 operator-(const vec3& l, const vec3& r)
{
	return vec3(l.x - r.x, l.y - r.y, l.z - r.z);
}

vec3 operator*(const vec3& v, float f)
{
	return vec3(v.x * f, v.y * f, v.z * f);
}

vec3 operator*(const vec3& l, const vec3& r)
{
	return vec3(l.x * r.x, l.y * r.y, l.z * r.z);
}

float dot(const vec3& l, const vec3& r)
{
	return l.x * r.x + l.y * r.y + l.z * r.z;
}

float get_length_sq(const vec3& v)
{
	return v.x * v.x + v.y * v.y + v.z * v.z;
}

float get_length(const vec3& v)
{
	float lenSq = v.x * v.x + v.y * v.y + v.z * v.z;
	if (lenSq < VEC3_EPSILON)
	{
		return 0.0f;
	}
	return sqrtf(lenSq);
}

void normalize(vec3& v)
{
	float lenSq = v.x * v.x + v.y * v.y + v.z * v.z;
	if (lenSq < VEC3_EPSILON)
	{
		return;
	}
	float invLen = 1.0f / sqrtf(lenSq);

	v.x *= invLen;
	v.y *= invLen;
	v.z *= invLen;
}

vec3 get_normalized(const vec3& v)
{
	float lenSq = v.x * v.x + v.y * v.y + v.z * v.z;
	if (lenSq < VEC3_EPSILON)
	{
		return v;
	}
	float invLen = 1.0f / sqrtf(lenSq);

	return vec3(v.x * invLen, v.y * invLen, v.z * invLen);
}

float angle(const vec3& l, const vec3& r)
{
	float sqMagL = l.x * l.x + l.y * l.y + l.z * l.z;
	float sqMagR = r.x * r.x + r.y * r.y + r.z * r.z;

	if (sqMagL < VEC3_EPSILON || sqMagR < VEC3_EPSILON)
	{
		return 0.0f;
	}

	float dot = l.x * r.x + l.y * r.y + l.z * r.z;
	float len = sqrtf(sqMagL) * sqrtf(sqMagR);
	return acosf(dot / len);
}

vec3 project(const vec3& a, const vec3& b)
{
	float magBSq = get_length_sq(b);
	if (magBSq < VEC3_EPSILON)
	{
		return vec3();
	}
	float scale = dot(a, b) / magBSq;
	return b * scale;
}

vec3 reject(const vec3& a, const vec3& b)
{
	vec3 projection = project(a, b);
	return a - projection;
}

vec3 reflect(const vec3& a, const vec3& b)
{
	float magBSq = get_length_sq(b);
	if (magBSq < VEC3_EPSILON)
	{
		return vec3();
	}
	float scale = dot(a, b) / magBSq;
	vec3 proj2 = b * (scale * 2);
	return a - proj2;
}

vec3 cross(const vec3& l, const vec3& r)
{
	return vec3(l.y * r.z - l.z * r.y, l.z * r.x - l.x * r.z, l.x * r.y - l.y * r.x);
}

vec3 lerp(const vec3& s, const vec3& e, float t)
{
	return vec3(s.x + (e.x - s.x) * t, s.y + (e.y - s.y) * t, s.z + (e.z - s.z) * t);
}

vec3 slerp(const vec3& s, const vec3& e, float t)
{
	if (t < 0.01f)
	{
		return lerp(s, e, t);
	}

	vec3 from = get_normalized(s);
	vec3 to = get_normalized(e);

	float theta = angle(from, to);
	float sin_theta = sinf(theta);

	float a = sinf((1.0f - t) * theta) / sin_theta;
	float b = sinf(t * theta) / sin_theta;

	return from * a + to * b;
}

vec3 nlerp(const vec3& s, const vec3& e, float t)
{
	vec3 linear(s.x + (e.x - s.x) * t, s.y + (e.y - s.y) * t, s.z + (e.z - s.z) * t);
	return get_normalized(linear);
}

bool operator==(const vec3& l, const vec3& r)
{
	vec3 diff(l - r);
	return get_length_sq(diff) < VEC3_EPSILON;
}

bool operator!=(const vec3& l, const vec3& r)
{
	return ! (l == r);
}
