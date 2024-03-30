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
	unsigned int GetId();
	void SetId(unsigned int id);
	VectorTrack& GetPositionTrack();
	QuaternionTrack& GetRotationTrack();
	VectorTrack& GetScaleTrack();
	float get_start_time();
	float get_end_time();
	bool IsValid();
	Transform get_sample(const Transform& ref, float time, bool looping);
};
