#pragma once

#include <cassert>

constexpr float VEC3_EPSILON = 0.000001f;

#pragma pack(push, 1)

struct vec3
{
	float x;
	float y;
	float z;

	const float* data_ptr() const;

	float& operator[](std::size_t i)
	{
		switch (i)
		{
		case 0: return x;
		case 1: return y;
		case 2: return z;
		default: assert(false); return x;
		}
	}

	vec3();
	vec3(float ax, float ay, float az);
	vec3(float* fv);
};

static_assert(sizeof(vec3) == sizeof(float) * 3, "Invalid size");
#pragma pack(pop)

vec3 operator+(const vec3& l, const vec3& r);
vec3 operator-(const vec3& l, const vec3& r);
vec3 operator*(const vec3& v, float f);
vec3 operator*(const vec3& l, const vec3& r);
bool operator==(const vec3& l, const vec3& r);
bool operator!=(const vec3& l, const vec3& r);


float get_length_sq(const vec3& v);
float get_length(const vec3& v);
vec3 get_normalized(const vec3& v);
float get_angle_between(const vec3& l, const vec3& r);
vec3 get_projected(const vec3& a, const vec3& b);
vec3 get_rejected(const vec3& a, const vec3& b);
vec3 get_reflected(const vec3& a, const vec3& b);

void normalize(vec3& v);

float dot(const vec3& l, const vec3& r);
vec3 cross(const vec3& l, const vec3& r);
vec3 lerp(const vec3& s, const vec3& e, float t);
vec3 slerp(const vec3& s, const vec3& e, float t);
vec3 nlerp(const vec3& s, const vec3& e, float t);
