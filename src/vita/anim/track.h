#pragma once

#include <vector>

#include "vita/anim/frame.h"
#include "vita/anim/vec3.h"
#include "vita/anim/quat.h"
#include "vita/anim/interpolation.h"

template<typename T>
struct Track
{
	std::vector<Frame<T>> frames;
	Interpolation interpolation;

	Track(const std::vector<Frame<T>>& fra, Interpolation in);

	bool is_valid() const;

	float get_start_time();
	float get_end_time();
	T get_sample(float time, bool looping);
};

typedef Track<float> ScalarTrack;
typedef Track<vec3> VectorTrack;
typedef Track<quat> QuaternionTrack;
