#pragma once

#include <vector>
#include <string>

#include "vita/anim/transformtrack.h"
#include "vita/anim/pose.h"

struct ClipDuration
{
	float start = 0.0f;
	float end = 0.0f;
};

struct Clip
{
	std::vector<TransformTrack> tracks;
	std::string name;
	ClipDuration duration;
	bool is_looping;

	Clip();

	float sample_to_pose(Pose& outPose, float inTime) const;
	TransformTrack& operator[](std::size_t index);

	float GetDuration() const;
};

ClipDuration duration_from_tracks(const std::vector<TransformTrack>& tracks);
