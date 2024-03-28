#include "vita/anim/transformtrack.h"

TransformTrack::TransformTrack()
	: mPosition{{}, Interpolation::Linear}
	, mRotation{{}, Interpolation::Linear}
	, mScale{{}, Interpolation::Linear}
{
	mId = 0;
}

unsigned int TransformTrack::GetId()
{
	return mId;
}

void TransformTrack::SetId(unsigned int id)
{
	mId = id;
}

VectorTrack& TransformTrack::GetPositionTrack()
{
	return mPosition;
}

QuaternionTrack& TransformTrack::GetRotationTrack()
{
	return mRotation;
}

VectorTrack& TransformTrack::GetScaleTrack()
{
	return mScale;
}

bool TransformTrack::IsValid()
{
	return mPosition.is_valid() || mRotation.is_valid() || mScale.is_valid();
}

float TransformTrack::get_start_time()
{
	float result = 0.0f;
	bool isSet = false;

	if (mPosition.is_valid())
	{
		result = mPosition.get_start_time();
		isSet = true;
	}
	if (mRotation.is_valid())
	{
		float rotationStart = mRotation.get_start_time();
		if (rotationStart < result || ! isSet)
		{
			result = rotationStart;
			isSet = true;
		}
	}
	if (mScale.is_valid())
	{
		float scaleStart = mScale.get_start_time();
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

	if (mPosition.is_valid())
	{
		result = mPosition.get_end_time();
		isSet = true;
	}
	if (mRotation.is_valid())
	{
		float rotationEnd = mRotation.get_end_time();
		if (rotationEnd > result || ! isSet)
		{
			result = rotationEnd;
			isSet = true;
		}
	}
	if (mScale.is_valid())
	{
		float scaleEnd = mScale.get_end_time();
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
	Transform result = ref;	 // Assign default values
	if (mPosition.is_valid())
	{  // Only assign if animated
		result.position = mPosition.get_sample(time, looping);
	}
	if (mRotation.is_valid())
	{  // Only assign if animated
		result.rotation = mRotation.get_sample(time, looping);
	}
	if (mScale.is_valid())
	{  // Only assign if animated
		result.scale = mScale.get_sample(time, looping);
	}
	return result;
}
