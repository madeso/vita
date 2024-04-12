#include "vita/anim/clip.h"

#include <cmath>

Clip::Clip()
{
	name = "No name given";
	is_looping = true;
}

float AdjustTimeToFitRange(const Clip& self, float inTime)
{
	const auto& span = self.duration;

	if (self.is_looping)
	{
		const auto duration = self.GetDuration();
		if (duration <= 0)
		{
			return 0.0f;
		}
		inTime = std::fmod(inTime - span.start, duration);
		if (inTime < 0.0f)
		{
			inTime += duration;
		}
		inTime = inTime + span.start;
	}
	else
	{
		if (inTime < span.start)
		{
			inTime = span.start;
		}
		if (inTime > span.end)
		{
			inTime = span.end;
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

	const auto size = tracks.size();
	for (std::size_t i = 0; i < size; ++i)
	{
		const auto joint = tracks[i].id;
		Transform local = outPose.GetLocalTransform(joint);
		Transform animated = tracks[i].get_sample(local, time, is_looping);
		outPose.SetLocalTransform(joint, animated);
	}
	return time;
}

ClipDuration duration_from_tracks(const std::vector<TransformTrack>& tracks)
{
	float start_time = 0.0f;
	float end_time = 0.0f;
	bool startSet = false;
	bool endSet = false;
	const auto tracksSize = tracks.size();
	for (std::size_t i = 0; i < tracksSize; ++i)
	{
		if (tracks[i].is_valid())
		{
			float trackStartTime = tracks[i].get_start_time();
			float trackEndTime = tracks[i].get_end_time();

			if (trackStartTime < start_time || ! startSet)
			{
				start_time = trackStartTime;
				startSet = true;
			}

			if (trackEndTime > end_time || ! endSet)
			{
				end_time = trackEndTime;
				endSet = true;
			}
		}
	}

	return {start_time, end_time};
}

TransformTrack& Clip::operator[](std::size_t joint)
{
	const auto size = tracks.size();
	for (std::size_t i = 0; i < size; ++i)
	{
		if (tracks[i].id == joint)
		{
			return tracks[i];
		}
	}

	tracks.emplace_back();
	tracks[tracks.size() - 1].id = joint;
	return tracks[tracks.size() - 1];
}

float Clip::GetDuration() const
{
	return duration.end - duration.start;
}
