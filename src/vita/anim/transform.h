#pragma once

#include "vita/anim/vec3.h"
#include "vita/anim/mat4.h"
#include "vita/anim/quat.h"

struct Transform
{
	vec3 position;
	quat rotation;
	vec3 scale;

	Transform();
	Transform(const vec3& p, const quat& r, const vec3& s);
};

bool operator==(const Transform& a, const Transform& b);
bool operator!=(const Transform& a, const Transform& b);

Transform combine(const Transform& a, const Transform& b);
Transform inverse(const Transform& t);
Transform mix(const Transform& a, const Transform& b, float t);

vec3 transform_point(const Transform& a, const vec3& b);
vec3 transform_vector(const Transform& a, const vec3& b);

mat4 mat4_from_transform(const Transform& t);
Transform transform_from_mat4(const mat4& m);