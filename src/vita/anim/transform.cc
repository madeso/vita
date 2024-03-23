#include <cmath>
#include <iostream>

#include "vita/anim/transform.h"

Transform::Transform()
	: position(vec3(0, 0, 0))
	, rotation(quat(0, 0, 0, 1))
	, scale(vec3(1, 1, 1))
{
}

Transform::Transform(const vec3& p, const quat& r, const vec3& s)
	: position(p)
	, rotation(r)
	, scale(s)
{
}

Transform combine(const Transform& a, const Transform& b)
{
	Transform out;

	out.scale = a.scale * b.scale;
	out.rotation = b.rotation * a.rotation;

	out.position = a.rotation * (a.scale * b.position);
	out.position = a.position + out.position;

	return out;
}

Transform inverse(const Transform& t)
{
	Transform inv;

	inv.rotation = inverse(t.rotation);

	inv.scale.x = std::abs(t.scale.x) < VEC3_EPSILON ? 0.0f : 1.0f / t.scale.x;
	inv.scale.y = std::abs(t.scale.y) < VEC3_EPSILON ? 0.0f : 1.0f / t.scale.y;
	inv.scale.z = std::abs(t.scale.z) < VEC3_EPSILON ? 0.0f : 1.0f / t.scale.z;

	const auto invTranslation = t.position * -1.0f;
	inv.position = inv.rotation * (inv.scale * invTranslation);

	return inv;
}

Transform mix(const Transform& a, const Transform& b, float t)
{
	quat bRot = b.rotation;
	if (dot(a.rotation, bRot) < 0.0f)
	{
		bRot = -bRot;
	}
	return Transform(
		lerp(a.position, b.position, t), nlerp(a.rotation, bRot, t), lerp(a.scale, b.scale, t)
	);
}

bool operator==(const Transform& a, const Transform& b)
{
	return a.position == b.position && a.rotation == b.rotation && a.scale == b.scale;
}

bool operator!=(const Transform& a, const Transform& b)
{
	return ! (a == b);
}

mat4 mat4_from_transform(const Transform& t)
{
	// First, extract the rotation basis of the transform
	vec3 x = t.rotation * vec3(1, 0, 0);
	vec3 y = t.rotation * vec3(0, 1, 0);
	vec3 z = t.rotation * vec3(0, 0, 1);

	// Next, scale the basis vectors
	x = x * t.scale.x;
	y = y * t.scale.y;
	z = z * t.scale.z;

	// Extract the position of the transform
	const auto p = t.position;

	// Create matrix
	return mat4(
		x.x,
		x.y,
		x.z,
		0,	// X basis (& Scale)
		y.x,
		y.y,
		y.z,
		0,	// Y basis (& scale)
		z.x,
		z.y,
		z.z,
		0,	// Z basis (& scale)
		p.x,
		p.y,
		p.z,
		1  // Position
	);
}

Transform transform_from_mat4(const mat4& m)
{
	Transform out;

	out.position = vec3(m[12], m[13], m[14]);
	out.rotation = quat_from_mat4(m);

	mat4 rotScaleMat(m[0], m[1], m[2], 0, m[4], m[5], m[6], 0, m[8], m[9], m[10], 0, 0, 0, 0, 1);
	mat4 invRotMat = mat4_from_quat(inverse(out.rotation));
	mat4 scaleSkewMat = rotScaleMat * invRotMat;

	out.scale = vec3(scaleSkewMat[0], scaleSkewMat[5], scaleSkewMat[10]);

	return out;
}

vec3 transform_point(const Transform& a, const vec3& b)
{
	vec3 out = a.rotation * (a.scale * b);
	out = a.position + out;

	return out;
}

vec3 transform_vector(const Transform& a, const vec3& b)
{
	return a.rotation * (a.scale * b);
}
