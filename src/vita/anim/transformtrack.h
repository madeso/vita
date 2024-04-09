#pragma once

#include "vita/anim/track.h"
#include "vita/anim/transform.h"

struct TransformTrack
{
	std::size_t id;

	VectorTrack position;
	QuaternionTrack rotation;
	VectorTrack scale;

	TransformTrack();

	float get_start_time() const;
	float get_end_time() const;

	bool is_valid() const;

	Transform get_sample(const Transform& ref, float time, bool looping) const;
};
