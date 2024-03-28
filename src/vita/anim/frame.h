#pragma once

#include "vita/anim/vec3.h"
#include "vita/anim/quat.h"

template<typename T>
struct Frame
{
	float time;
	T in;
	T out;
	T value;

	Frame() = default;

	Frame(float t, const T& i, const T& o, const T& v)
		: time(t)
		, in(i)
		, out(o)
		, value(v)
	{
	}
};

typedef Frame<float> ScalarFrame;
typedef Frame<vec3> VectorFrame;
typedef Frame<quat> QuaternionFrame;
