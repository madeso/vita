#include "vita/anim/mat4.h"

#include <cmath>
#include <iostream>

float mat4::operator[](int i) const
{
	return *(&xx + i);
}

float& mat4::operator[](int i)
{
	return *(&xx + i);
}

vec4 mat4::right() const
{
	return {xx, xy, xz, xw};
}

vec4 mat4::up() const
{
	return {yx, yy, yz, yw};
}

vec4 mat4::forward() const
{
	return {zx, zy, zz, zw};
}

vec4 mat4::position() const
{
	return {tx, ty, tz, tw};
}

bool operator==(const mat4& a, const mat4& b)
{
	for (int i = 0; i < 16; ++i)
	{
		if (fabsf(a[i] - b[i]) > MAT4_EPSILON)
		{
			return false;
		}
	}
	return true;
}

bool operator!=(const mat4& a, const mat4& b)
{
	return ! (a == b);
}

mat4 operator*(const mat4& m, float f)
{
	return mat4(
		m.xx * f,
		m.xy * f,
		m.xz * f,
		m.xw * f,
		m.yx * f,
		m.yy * f,
		m.yz * f,
		m.yw * f,
		m.zx * f,
		m.zy * f,
		m.zz * f,
		m.zw * f,
		m.tx * f,
		m.ty * f,
		m.tz * f,
		m.tw * f
	);
}

mat4 operator+(const mat4& a, const mat4& b)
{
	return mat4(
		a.xx + b.xx,
		a.xy + b.xy,
		a.xz + b.xz,
		a.xw + b.xw,
		a.yx + b.yx,
		a.yy + b.yy,
		a.yz + b.yz,
		a.yw + b.yw,
		a.zx + b.zx,
		a.zy + b.zy,
		a.zz + b.zz,
		a.zw + b.zw,
		a.tx + b.tx,
		a.ty + b.ty,
		a.tz + b.tz,
		a.tw + b.tw
	);
}

#define M4D(aRow, bCol) \
	a[0 * 4 + aRow] * b[bCol * 4 + 0] + a[1 * 4 + aRow] * b[bCol * 4 + 1] \
		+ a[2 * 4 + aRow] * b[bCol * 4 + 2] + a[3 * 4 + aRow] * b[bCol * 4 + 3]

mat4 operator*(const mat4& a, const mat4& b)
{
	return mat4(
		M4D(0, 0),
		M4D(1, 0),
		M4D(2, 0),
		M4D(3, 0),	// Column 0
		M4D(0, 1),
		M4D(1, 1),
		M4D(2, 1),
		M4D(3, 1),	// Column 1
		M4D(0, 2),
		M4D(1, 2),
		M4D(2, 2),
		M4D(3, 2),	// Column 2
		M4D(0, 3),
		M4D(1, 3),
		M4D(2, 3),
		M4D(3, 3)  // Column 3
	);
}

#define M4V4D(mRow, x, y, z, w) \
	x* m[0 * 4 + mRow] + y* m[1 * 4 + mRow] + z* m[2 * 4 + mRow] + w* m[3 * 4 + mRow]

vec4 operator*(const mat4& m, const vec4& v)
{
	return vec4(
		M4V4D(0, v.x, v.y, v.z, v.w),
		M4V4D(1, v.x, v.y, v.z, v.w),
		M4V4D(2, v.x, v.y, v.z, v.w),
		M4V4D(3, v.x, v.y, v.z, v.w)
	);
}

vec3 transform_vector(const mat4& m, const vec3& v)
{
	return vec3(
		M4V4D(0, v.x, v.y, v.z, 0.0f), M4V4D(1, v.x, v.y, v.z, 0.0f), M4V4D(2, v.x, v.y, v.z, 0.0f)
	);
}

vec3 transform_point(const mat4& m, const vec3& v)
{
	return vec3(
		M4V4D(0, v.x, v.y, v.z, 1.0f), M4V4D(1, v.x, v.y, v.z, 1.0f), M4V4D(2, v.x, v.y, v.z, 1.0f)
	);
}

vec3 transform_point(const mat4& m, const vec3& v, float& w)
{
	float _w = w;
	w = M4V4D(3, v.x, v.y, v.z, _w);
	return vec3(
		M4V4D(0, v.x, v.y, v.z, _w), M4V4D(1, v.x, v.y, v.z, _w), M4V4D(2, v.x, v.y, v.z, _w)
	);
}

#define M4SWAP(x, y) \
	{ \
		float t = x; \
		x = y; \
		y = t; \
	}

void transpose(mat4& m)
{
	M4SWAP(m.yx, m.xy);
	M4SWAP(m.zx, m.xz);
	M4SWAP(m.tx, m.xw);
	M4SWAP(m.zy, m.yz);
	M4SWAP(m.ty, m.yw);
	M4SWAP(m.tz, m.zw);
}

mat4 transposed(const mat4& m)
{
	return mat4(
		m.xx,
		m.yx,
		m.zx,
		m.tx,
		m.xy,
		m.yy,
		m.zy,
		m.ty,
		m.xz,
		m.yz,
		m.zz,
		m.tz,
		m.xw,
		m.yw,
		m.zw,
		m.tw
	);
}

#define M4_3X3MINOR(c0, c1, c2, r0, r1, r2) \
	(m[c0 * 4 + r0] * (m[c1 * 4 + r1] * m[c2 * 4 + r2] - m[c1 * 4 + r2] * m[c2 * 4 + r1]) \
	 - m[c1 * 4 + r0] * (m[c0 * 4 + r1] * m[c2 * 4 + r2] - m[c0 * 4 + r2] * m[c2 * 4 + r1]) \
	 + m[c2 * 4 + r0] * (m[c0 * 4 + r1] * m[c1 * 4 + r2] - m[c0 * 4 + r2] * m[c1 * 4 + r1]))

float determinant(const mat4& m)
{
	return m[0] * M4_3X3MINOR(1, 2, 3, 1, 2, 3) - m[4] * M4_3X3MINOR(0, 2, 3, 1, 2, 3)
		 + m[8] * M4_3X3MINOR(0, 1, 3, 1, 2, 3) - m[12] * M4_3X3MINOR(0, 1, 2, 1, 2, 3);
}

mat4 adjugate(const mat4& m)
{
	// Cofactor(M[i, j]) = Minor(M[i, j]] * pow(-1, i + j)
	mat4 cofactor;

	cofactor[0] = M4_3X3MINOR(1, 2, 3, 1, 2, 3);
	cofactor[1] = -M4_3X3MINOR(1, 2, 3, 0, 2, 3);
	cofactor[2] = M4_3X3MINOR(1, 2, 3, 0, 1, 3);
	cofactor[3] = -M4_3X3MINOR(1, 2, 3, 0, 1, 2);

	cofactor[4] = -M4_3X3MINOR(0, 2, 3, 1, 2, 3);
	cofactor[5] = M4_3X3MINOR(0, 2, 3, 0, 2, 3);
	cofactor[6] = -M4_3X3MINOR(0, 2, 3, 0, 1, 3);
	cofactor[7] = M4_3X3MINOR(0, 2, 3, 0, 1, 2);

	cofactor[8] = M4_3X3MINOR(0, 1, 3, 1, 2, 3);
	cofactor[9] = -M4_3X3MINOR(0, 1, 3, 0, 2, 3);
	cofactor[10] = M4_3X3MINOR(0, 1, 3, 0, 1, 3);
	cofactor[11] = -M4_3X3MINOR(0, 1, 3, 0, 1, 2);

	cofactor[12] = -M4_3X3MINOR(0, 1, 2, 1, 2, 3);
	cofactor[13] = M4_3X3MINOR(0, 1, 2, 0, 2, 3);
	cofactor[14] = -M4_3X3MINOR(0, 1, 2, 0, 1, 3);
	cofactor[15] = M4_3X3MINOR(0, 1, 2, 0, 1, 2);

	return transposed(cofactor);
}

mat4 inverse(const mat4& m)
{
	const auto det = determinant(m);

	if (det == 0.0f)
	{  // Epsilon check would need to be REALLY small
		std::cout << "WARNING: Trying to invert a matrix with a zero determinant\n";
		return mat4();
	}
	mat4 adj = adjugate(m);

	return adj * (1.0f / det);
}

void invert(mat4& m)
{
	const auto det = determinant(m);

	if (det == 0.0f)
	{
		std::cout << "WARNING: Trying to inverse a matrix with a zero determinant\n";
		m = mat4();
		return;
	}

	m = adjugate(m) * (1.0f / det);
}

mat4 mat4_from_frustum(float l, float r, float b, float t, float n, float f)
{
	if (l == r || t == b || n == f)
	{
		std::cout << "WARNING: Trying to create invalid frustum\n";
		return mat4();	// Error
	}
	return mat4(
		(2.0f * n) / (r - l),
		0,
		0,
		0,
		0,
		(2.0f * n) / (t - b),
		0,
		0,
		(r + l) / (r - l),
		(t + b) / (t - b),
		(-(f + n)) / (f - n),
		-1,
		0,
		0,
		(-2 * f * n) / (f - n),
		0
	);
}

mat4 mat4_from_perspective(float fov, float aspect, float znear, float zfar)
{
	const auto ymax = znear * tanf(fov * 3.14159265359f / 360.0f);
	const auto xmax = ymax * aspect;

	return mat4_from_frustum(-xmax, xmax, -ymax, ymax, znear, zfar);
}

mat4 mat4_from_ortho(float l, float r, float b, float t, float n, float f)
{
	if (l == r || t == b || n == f)
	{
		return mat4();	// Error
	}
	return mat4(
		2.0f / (r - l),
		0,
		0,
		0,
		0,
		2.0f / (t - b),
		0,
		0,
		0,
		0,
		-2.0f / (f - n),
		0,
		-((r + l) / (r - l)),
		-((t + b) / (t - b)),
		-((f + n) / (f - n)),
		1
	);
}

mat4 mat4_from_look_at(const vec3& position, const vec3& target, const vec3& up)
{
	// Remember, forward is negative z
	const auto f = get_normalized(target - position) * -1.0f;
	vec3 r = cross(up, f);	// Right handed
	if (r == vec3(0, 0, 0))
	{
		return mat4();	// Error
	}
	normalize(r);
	const auto u = get_normalized(cross(f, r));	 // Right handed

	const auto t = vec3(-dot(r, position), -dot(u, position), -dot(f, position));

	return mat4(
		// Transpose upper 3x3 matrix to invert it
		r.x,
		u.x,
		f.x,
		0,
		r.y,
		u.y,
		f.y,
		0,
		r.z,
		u.z,
		f.z,
		0,
		t.x,
		t.y,
		t.z,
		1
	);
}
