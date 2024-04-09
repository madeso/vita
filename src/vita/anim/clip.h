#pragma once

#include <vector>
#include <string>

#include "vita/anim/transformtrack.h"
#include "vita/anim/pose.h"

struct Clip
{
	std::vector<TransformTrack> mTracks;
	std::string mName;
	float mStartTime;
	float mEndTime;
	bool mLooping;

	Clip();

	std::size_t GetIdAtIndex(std::size_t index) const;
	void SetIdAtIndex(std::size_t index, std::size_t id);
	std::size_t Size() const;
	float Sample(Pose& outPose, float inTime) const;
	TransformTrack& operator[](std::size_t index);
	void RecalculateDuration();

	float GetDuration() const;
};
