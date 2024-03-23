#pragma once

#pragma pack(push, 1)

template<typename T>
struct TVec4
{
	T x;
	T y;
	T z;
	T w;

	const T* data_ptr() const
	{
		return &x;
	}

	// T v[4];

	TVec4<T>()
		: x(static_cast<T>(0))
		, y(static_cast<T>(0))
		, z(static_cast<T>(0))
		, w(static_cast<T>(0))
	{
	}

	TVec4<T>(T _x, T _y, T _z, T _w)
		: x(_x)
		, y(_y)
		, z(_z)
		, w(_w)
	{
	}

	TVec4<T>(T* fv)
		: x(fv[0])
		, y(fv[1])
		, z(fv[2])
		, w(fv[3])
	{
	}
};

#pragma pack(pop)

typedef TVec4<float> vec4;
typedef TVec4<int> ivec4;
typedef TVec4<unsigned int> uivec4;

static_assert(sizeof(vec4) == sizeof(float) * 4, "Invalid size");
static_assert(sizeof(ivec4) == sizeof(int) * 4, "Invalid size");
static_assert(sizeof(uivec4) == sizeof(unsigned int) * 4, "Invalid size");
