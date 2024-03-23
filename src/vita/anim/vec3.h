#pragma once

constexpr float VEC3_EPSILON = 0.000001f;

#pragma pack(push, 1)

struct vec3
{
	float x;
	float y;
	float z;

	const float* data_ptr() const
	{
		return &x;
	}

	// float v[3];

	vec3()
		: x(0.0f)
		, y(0.0f)
		, z(0.0f)
	{
	}

	vec3(float ax, float ay, float az)
		: x(ax)
		, y(ay)
		, z(az)
	{
	}

	vec3(float* fv)
		: x(fv[0])
		, y(fv[1])
		, z(fv[2])
	{
	}
};

#pragma pack(pop)

vec3 operator+(const vec3& l, const vec3& r);
vec3 operator-(const vec3& l, const vec3& r);
vec3 operator*(const vec3& v, float f);
vec3 operator*(const vec3& l, const vec3& r);
bool operator==(const vec3& l, const vec3& r);
bool operator!=(const vec3& l, const vec3& r);

float dot(const vec3& l, const vec3& r);
float get_length_sq(const vec3& v);
float get_length(const vec3& v);
void normalize(vec3& v);
vec3 get_normalized(const vec3& v);
float angle(const vec3& l, const vec3& r);
vec3 project(const vec3& a, const vec3& b);
vec3 reject(const vec3& a, const vec3& b);
vec3 reflect(const vec3& a, const vec3& b);
vec3 cross(const vec3& l, const vec3& r);
vec3 lerp(const vec3& s, const vec3& e, float t);
vec3 slerp(const vec3& s, const vec3& e, float t);
vec3 nlerp(const vec3& s, const vec3& e, float t);
