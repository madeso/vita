#pragma once

#include "vita/anim/track.h"
#include "vita/anim/transform.h"

struct TransformTrack
{
	unsigned int id;

	VectorTrack position;
	QuaternionTrack rotation;
	VectorTrack scale;

	TransformTrack();

	float get_start_time();
	float get_end_time();

	bool is_valid();

	Transform get_sample(const Transform& ref, float time, bool looping);
};
