#include "vita/anim/transformtrack.h"

TransformTrack::TransformTrack()
	: id(0)
	, position{{}, Interpolation::Linear}
	, rotation{{}, Interpolation::Linear}
	, scale{{}, Interpolation::Linear}
{
}

bool TransformTrack::is_valid()
{
	return position.is_valid() || rotation.is_valid() || scale.is_valid();
}

float min(const std::optional<float>& lhs, float rhs)
{
	return lhs ? std::min(*lhs, rhs) : rhs;
}

float max(const std::optional<float>& lhs, float rhs)
{
	return lhs ? std::max(*lhs, rhs) : rhs;
}

float TransformTrack::get_start_time()
{
	std::optional<float> result = std::nullopt;

	if (position.is_valid())
	{
		result = position.get_start_time();
	}

	if (rotation.is_valid())
	{
		result = min(result, rotation.get_start_time());
	}

	if (scale.is_valid())
	{
		result = min(result, scale.get_start_time());
	}

	return result.value_or(0.0f);
}

float TransformTrack::get_end_time()
{
	std::optional<float> result = std::nullopt;

	if (position.is_valid())
	{
		result = position.get_end_time();
	}

	if (rotation.is_valid())
	{
		result = max(result, rotation.get_end_time());
	}

	if (scale.is_valid())
	{
		result = max(result, scale.get_end_time());
	}

	return result.value_or(0.0f);
}

Transform TransformTrack::get_sample(const Transform& ref, float time, bool looping)
{
	return {
		position.is_valid() ? position.get_sample(time, looping) : ref.position,
		rotation.is_valid() ? rotation.get_sample(time, looping) : ref.rotation,
		scale.is_valid() ? scale.get_sample(time, looping) : ref.scale
	};
}
