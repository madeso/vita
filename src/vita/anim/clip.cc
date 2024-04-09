#include "vita/anim/clip.h"

#include <cmath>

Clip::Clip()
{
	mName = "No name given";
	mStartTime = 0.0f;
	mEndTime = 0.0f;
	mLooping = true;
}

float AdjustTimeToFitRange(const Clip& self, float inTime)
{
	if (self.mLooping)
	{
		const auto duration = self.GetDuration();
		if (duration <= 0)
		{
			return 0.0f;
		}
		inTime = std::fmod(inTime - self.mStartTime, duration);
		if (inTime < 0.0f)
		{
			inTime += duration;
		}
		inTime = inTime + self.mStartTime;
	}
	else
	{
		if (inTime < self.mStartTime)
		{
			inTime = self.mStartTime;
		}
		if (inTime > self.mEndTime)
		{
			inTime = self.mEndTime;
		}
	}
	return inTime;
}

float Clip::Sample(Pose& outPose, float time) const
{
	if (GetDuration() == 0.0f)
	{
		return 0.0f;
	}
	time = AdjustTimeToFitRange(*this, time);

	const auto size = mTracks.size();
	for (std::size_t i = 0; i < size; ++i)
	{
		const auto joint = mTracks[i].id;
		Transform local = outPose.GetLocalTransform(joint);
		Transform animated = mTracks[i].get_sample(local, time, mLooping);
		outPose.SetLocalTransform(joint, animated);
	}
	return time;
}

void Clip::RecalculateDuration()
{
	mStartTime = 0.0f;
	mEndTime = 0.0f;
	bool startSet = false;
	bool endSet = false;
	const auto tracksSize = mTracks.size();
	for (std::size_t i = 0; i < tracksSize; ++i)
	{
		if (mTracks[i].is_valid())
		{
			float trackStartTime = mTracks[i].get_start_time();
			float trackEndTime = mTracks[i].get_end_time();

			if (trackStartTime < mStartTime || ! startSet)
			{
				mStartTime = trackStartTime;
				startSet = true;
			}

			if (trackEndTime > mEndTime || ! endSet)
			{
				mEndTime = trackEndTime;
				endSet = true;
			}
		}
	}
}

TransformTrack& Clip::operator[](std::size_t joint)
{
	const auto size = mTracks.size();
	for (std::size_t i = 0; i < size; ++i)
	{
		if (mTracks[i].id == joint)
		{
			return mTracks[i];
		}
	}

	mTracks.emplace_back();
	mTracks[mTracks.size() - 1].id = joint;
	return mTracks[mTracks.size() - 1];
}

std::size_t Clip::GetIdAtIndex(std::size_t index) const
{
	return mTracks[index].id;
}

void Clip::SetIdAtIndex(std::size_t index, std::size_t id)
{
	mTracks[index].id = id;
}

std::size_t Clip::Size() const
{
	return mTracks.size();
}

float Clip::GetDuration() const
{
	return mEndTime - mStartTime;
}
