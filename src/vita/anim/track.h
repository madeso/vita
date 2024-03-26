#pragma once

#include <vector>

#include "vita/anim/frame.h"
#include "vita/anim/vec3.h"
#include "vita/anim/quat.h"
#include "vita/anim/interpolation.h"

template<typename T>
struct Track
{
	std::vector<Frame<T>> mFrames;
	Interpolation mInterpolation;

	T SampleConstant(float time, bool looping);
	T SampleLinear(float time, bool looping);
	T SampleCubic(float time, bool looping);
	T Hermite(float time, const T& point1, const T& slope1, const T& point2, const T& slope2);
	int FrameIndex(float time, bool looping);
	float AdjustTimeToFitTrack(float time, bool looping);

	Track(const std::vector<Frame<T>>& fra, Interpolation in);

	void Resize(unsigned int size);
	unsigned int Size();
	Interpolation GetInterpolation();
	void SetInterpolation(Interpolation interpolation);
	float GetStartTime();
	float GetEndTime();
	T Sample(float time, bool looping);
	Frame<T>& operator[](unsigned int index);
};

typedef Track<float> ScalarTrack;
typedef Track<vec3> VectorTrack;
typedef Track<quat> QuaternionTrack;
