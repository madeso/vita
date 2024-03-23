#pragma once

#include "vita/anim/vec3.h"
#include "vita/anim/vec4.h"

constexpr float MAT4_EPSILON = 0.000001f;

#pragma pack(push, 1)

struct mat4
{
	// float v[16];
	float operator[](int i) const;
	float& operator[](int i);

	vec4 right() const;
	vec4 up() const;
	vec4 forward() const;
	vec4 position() const;

	const float* data_ptr() const;

	// row 1     row 2     row 3     row 4
	/* column 1 */
	float xx;
	float xy;
	float xz;
	float xw;

	/* column 2 */
	float yx;
	float yy;
	float yz;
	float yw;

	/* column 3 */
	float zx;
	float zy;
	float zz;
	float zw;

	/* column 4 */
	float tx;
	float ty;
	float tz;
	float tw;

	// struct
	// {
	// 	float c0r0;
	// 	float c0r1;
	// 	float c0r2;
	// 	float c0r3;
	// 	float c1r0;
	// 	float c1r1;
	// 	float c1r2;
	// 	float c1r3;
	// 	float c2r0;
	// 	float c2r1;
	// 	float c2r2;
	// 	float c2r3;
	// 	float c3r0;
	// 	float c3r1;
	// 	float c3r2;
	// 	float c3r3;
	// };

	// struct
	// {
	// 	float r0c0;
	// 	float r1c0;
	// 	float r2c0;
	// 	float r3c0;
	// 	float r0c1;
	// 	float r1c1;
	// 	float r2c1;
	// 	float r3c1;
	// 	float r0c2;
	// 	float r1c2;
	// 	float r2c2;
	// 	float r3c2;
	// 	float r0c3;
	// 	float r1c3;
	// 	float r2c3;
	// 	float r3c3;
	// };

	// Include constructors here

	mat4();

	explicit mat4(float* fv);

	mat4(
		float _00,
		float _01,
		float _02,
		float _03,
		float _10,
		float _11,
		float _12,
		float _13,
		float _20,
		float _21,
		float _22,
		float _23,
		float _30,
		float _31,
		float _32,
		float _33
	);
};

static_assert(sizeof(mat4) == sizeof(float) * 16, "Invalid size");
#pragma pack(pop)

bool operator==(const mat4& a, const mat4& b);
bool operator!=(const mat4& a, const mat4& b);
mat4 operator*(const mat4& m, float f);
mat4 operator+(const mat4& a, const mat4& b);
mat4 operator*(const mat4& a, const mat4& b);
vec4 operator*(const mat4& m, const vec4& v);

vec3 get_transformed_vector(const mat4& m, const vec3& v);
vec3 get_transformed_point(const mat4& m, const vec3& v);
vec3 get_transformed_point(const mat4& m, const vec3& v, float& w);

mat4 get_transposed(const mat4& m);
float get_determinant(const mat4& m);
mat4 get_adjugate(const mat4& m);
mat4 get_inverse(const mat4& m);

void transpose(mat4& m);
void invert(mat4& m);

mat4 mat4_from_frustum(float l, float r, float b, float t, float n, float f);
mat4 mat4_from_perspective(float fov, float aspect, float znear, float zfar);
mat4 mat4_from_ortho(float l, float r, float b, float t, float n, float f);
mat4 mat4_from_look_at(const vec3& position, const vec3& target, const vec3& up);
