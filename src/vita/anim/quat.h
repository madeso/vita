#pragma once

#include "vita/anim/vec3.h"
#include "vita/anim/mat4.h"

constexpr float QUAT_EPSILON = 0.000001f;

#pragma pack(push, 1)

struct quat
{
	float x;
	float y;
	float z;
	float w;
	// float v[4];

	quat();
	quat(float _x, float _y, float _z, float _w);

	const float* data_ptr() const;

	vec3 vector() const;
	float scalar() const;

	vec3 get_axis() const;
	float get_angle() const;
};

#pragma pack(pop)
static_assert(sizeof(quat) == sizeof(float) * 4, "Invalid size");

quat operator+(const quat& a, const quat& b);
quat operator-(const quat& a, const quat& b);
quat operator*(const quat& a, float b);
quat operator-(const quat& q);
bool operator==(const quat& left, const quat& right);
bool operator!=(const quat& a, const quat& b);
quat operator*(const quat& Q1, const quat& Q2);
quat operator*(const quat& Q1, const quat& Q2);
vec3 operator*(const quat& q, const vec3& v);
quat operator^(const quat& q, float f);

bool is_same_orientation(const quat& left, const quat& right);

float get_length_sq(const quat& q);
float get_length(const quat& q);
quat get_normalized(const quat& q);
quat get_conjugate(const quat& q);
quat get_inverse(const quat& q);

void normalize(quat& q);

quat lerp(const quat& from, const quat& to, float t);
float dot(const quat& a, const quat& b);
quat nlerp(const quat& from, const quat& to, float t);
quat slerp(const quat& start, const quat& end, float t);

quat quat_from_look_rotation(const vec3& direcion, const vec3& up);
quat quat_from_mat4(const mat4& m);
quat quat_from_angle_axis(float angle, const vec3& axis);

/// returns a quat as a vector from was rotated to to (ignoring length)
quat quat_from_rotation(const vec3& from, const vec3& to);

mat4 mat4_from_quat(const quat& q);
