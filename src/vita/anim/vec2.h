#pragma once

#pragma pack(push, 1)

template<typename T>
struct TVec2
{
	T x;
	T y;

	const T* data_ptr() const
	{
		return &x;
	}

	TVec2()
		: x(static_cast<T>(0))
		, y(static_cast<T>(0))
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

#pragma pack(pop)

typedef TVec2<float> vec2;
typedef TVec2<int> ivec2;
