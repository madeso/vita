#pragma once

#include "vita/anim/vec3.h"
#include "vita/anim/quat.h"

template<typename T>
struct Frame
{
	float mTime;
	T mIn;
	T mOut;
	T mValue;

	Frame() = default;

	Frame(float t, const T& i, const T& o, const T& v)
		: mTime(t)
		, mIn(i)
		, mOut(o)
		, mValue(v)
	{
	}
};

typedef Frame<float> ScalarFrame;
typedef Frame<vec3> VectorFrame;
typedef Frame<quat> QuaternionFrame;
