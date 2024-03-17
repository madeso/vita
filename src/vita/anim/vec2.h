#pragma once

template<typename T>
struct TVec2
{
	T x;
	T y;

	TVec2()
		: x(T(0))
		, y(T(0))
	{
	}

	TVec2(T _x, T _y)
		: x(_x)
		, y(_y)
	{
	}

	TVec2(T* fv)
		: x(fv[0])
		, y(fv[1])
	{
	}
};

typedef TVec2<float> vec2;
typedef TVec2<int> ivec2;
