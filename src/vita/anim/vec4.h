#pragma once

template<typename T>
struct TVec4
{
	T x;
	T y;
	T z;
	T w;

	// T v[4];

	TVec4<T>()
		: x((T) 0)
		, y((T) 0)
		, z((T) 0)
		, w((T) 0)
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

typedef TVec4<float> vec4;
typedef TVec4<int> ivec4;
typedef TVec4<unsigned int> uivec4;
