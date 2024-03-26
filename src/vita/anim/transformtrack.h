#pragma once

#include "vita/anim/track.h"
#include "vita/anim/transform.h"

struct TransformTrack
{
	unsigned int mId;
	VectorTrack mPosition;
	QuaternionTrack mRotation;
	VectorTrack mScale;

	TransformTrack();
	unsigned int GetId();
	void SetId(unsigned int id);
	VectorTrack& GetPositionTrack();
	QuaternionTrack& GetRotationTrack();
	VectorTrack& GetScaleTrack();
	float GetStartTime();
	float GetEndTime();
	bool IsValid();
	Transform Sample(const Transform& ref, float time, bool looping);
};
