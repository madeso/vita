#include "vita/anim/transformtrack.h"

TransformTrack::TransformTrack()
	: position{{}, Interpolation::Linear}
	, rotation{{}, Interpolation::Linear}
	, scale{{}, Interpolation::Linear}
{
	id = 0;
}

unsigned int TransformTrack::GetId()
{
	return id;
}

void TransformTrack::SetId(unsigned int nid)
{
	id = nid;
}

VectorTrack& TransformTrack::GetPositionTrack()
{
	return position;
}

QuaternionTrack& TransformTrack::GetRotationTrack()
{
	return rotation;
}

VectorTrack& TransformTrack::GetScaleTrack()
{
	return scale;
}

bool TransformTrack::IsValid()
{
	return position.is_valid() || rotation.is_valid() || scale.is_valid();
}

float TransformTrack::get_start_time()
{
	float result = 0.0f;
	bool isSet = false;

	if (position.is_valid())
	{
		result = position.get_start_time();
		isSet = true;
	}
	if (rotation.is_valid())
	{
		float rotationStart = rotation.get_start_time();
		if (rotationStart < result || ! isSet)
		{
			result = rotationStart;
			isSet = true;
		}
	}
	if (scale.is_valid())
	{
		float scaleStart = scale.get_start_time();
		if (scaleStart < result || ! isSet)
		{
			result = scaleStart;
			isSet = true;
		}
	}

	return result;
}

float TransformTrack::get_end_time()
{
	float result = 0.0f;
	bool isSet = false;

	if (position.is_valid())
	{
		result = position.get_end_time();
		isSet = true;
	}
	if (rotation.is_valid())
	{
		float rotationEnd = rotation.get_end_time();
		if (rotationEnd > result || ! isSet)
		{
			result = rotationEnd;
			isSet = true;
		}
	}
	if (scale.is_valid())
	{
		float scaleEnd = scale.get_end_time();
		if (scaleEnd > result || ! isSet)
		{
			result = scaleEnd;
			isSet = true;
		}
	}

	return result;
}

Transform TransformTrack::get_sample(const Transform& ref, float time, bool looping)
{
	return {
		position.is_valid() ? position.get_sample(time, looping) : ref.position,
		rotation.is_valid() ? rotation.get_sample(time, looping) : ref.rotation,
		scale.is_valid() ? scale.get_sample(time, looping) : ref.scale
	};
}
