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

Transform get_combined(const Transform& a, const Transform& b);
Transform get_inverse(const Transform& t);
Transform get_mixed(const Transform& a, const Transform& b, float t);

vec3 get_transformed_point(const Transform& a, const vec3& b);
vec3 get_transformed_vector(const Transform& a, const vec3& b);

mat4 mat4_from_transform(const Transform& t);
Transform transform_from_mat4(const mat4& m);