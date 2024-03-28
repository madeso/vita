#include "vita/anim/track.h"

#include <cmath>
#include <cstring>

namespace TrackHelpers
{
float inline Interpolate(float a, float b, float t)
{
	return a + (b - a) * t;
}

vec3 inline Interpolate(const vec3& a, const vec3& b, float t)
{
	return lerp(a, b, t);
}

quat inline Interpolate(const quat& a, const quat& b, float t)
{
	quat result = get_mixed(a, b, t);
	if (dot(a, b) < 0)
	{  // Neighborhood
		result = get_mixed(a, -b, t);
	}
	return get_normalized(result);	//NLerp, not slerp
}

// Hermite helpers
float inline AdjustHermiteResult(float f)
{
	return f;
}

vec3 inline AdjustHermiteResult(const vec3& v)
{
	return v;
}

quat inline AdjustHermiteResult(const quat& q)
{
	return get_normalized(q);
}

void inline Neighborhood(const float&, float&)
{
}

void inline Neighborhood(const vec3&, vec3&)
{
}

void inline Neighborhood(const quat& a, quat& b)
{
	if (dot(a, b) < 0)
	{
		b = -b;
	}
}
};	//  namespace TrackHelpers

template<typename T>
T Hermite(float t, const T& p1, const T& s1, const T& _p2, const T& s2)
{
	float tt = t * t;
	float ttt = tt * t;

	T p2 = _p2;
	TrackHelpers::Neighborhood(p1, p2);

	float h1 = 2.0f * ttt - 3.0f * tt + 1.0f;
	float h2 = -2.0f * ttt + 3.0f * tt;
	float h3 = ttt - 2.0f * tt + t;
	float h4 = ttt - tt;

	T result = p1 * h1 + p2 * h2 + s1 * h3 + s2 * h4;
	return TrackHelpers::AdjustHermiteResult(result);
}

template<typename T>
int FrameIndex(const Track<T>& track, float time, bool looping)
{
	if (track.is_valid() == false)
	{
		return -1;
	}

	unsigned int size = static_cast<unsigned int>(track.frames.size());
	if (looping)
	{
		float startTime = track.frames[0].time;
		float endTime = track.frames[size - 1].time;
		// float duration = endTime - startTime;

		time = std::fmod(time - startTime, endTime - startTime);
		if (time < 0.0f)
		{
			time += endTime - startTime;
		}
		time = time + startTime;
	}
	else
	{
		if (time <= track.frames[0].time)
		{
			return 0;
		}
		if (time >= track.frames[size - 2].time)
		{
			return static_cast<int>(size) - 2;
		}
	}
	for (int i = static_cast<int>(size) - 1; i >= 0; --i)
	{
		if (time >= track.frames[static_cast<unsigned int>(i)].time)
		{
			return i;
		}
	}
	// Invalid code, we should not reach here!
	return -1;
}

template<typename T>
float AdjustTimeToFitTrack(const Track<T>& track, float time, bool looping)
{
	if (track.is_valid() == false)
	{
		return 0.0f;
	}

	unsigned int size = static_cast<unsigned int>(track.frames.size());
	float startTime = track.frames[0].time;
	float endTime = track.frames[size - 1].time;
	float duration = endTime - startTime;
	if (duration <= 0.0f)
	{
		return 0.0f;
	}
	if (looping)
	{
		time = std::fmod(time - startTime, endTime - startTime);
		if (time < 0.0f)
		{
			time += endTime - startTime;
		}
		time = time + startTime;
	}
	else
	{
		if (time <= track.frames[0].time)
		{
			time = startTime;
		}
		if (time >= track.frames[size - 1].time)
		{
			time = endTime;
		}
	}

	return time;
}

template<typename T>
T SampleConstant(const Track<T>& track, float time, bool looping)
{
	int frame = FrameIndex(track, time, looping);
	if (frame < 0 || frame >= static_cast<int>(track.frames.size()))
	{
		return T();
	}

	return track.frames[static_cast<unsigned int>(frame)].value;
}

template<typename T>
T SampleLinear(const Track<T>& track, float time, bool looping)
{
	const auto thisFrameIndex = FrameIndex(track, time, looping);
	if (thisFrameIndex < 0 || thisFrameIndex >= static_cast<int>(track.frames.size() - 1))
	{
		return T();
	}
	const auto nextFrameIndex = thisFrameIndex + 1;
	const auto& thisFrame = track.frames[static_cast<unsigned int>(thisFrameIndex)];
	const auto& nextFrame = track.frames[static_cast<unsigned int>(nextFrameIndex)];

	float trackTime = AdjustTimeToFitTrack(track, time, looping);
	float frameDelta = nextFrame.time - thisFrame.time;
	if (frameDelta <= 0.0f)
	{
		return T();
	}
	float t = (trackTime - thisFrame.time) / frameDelta;

	T start = thisFrame.value;
	T end = nextFrame.value;

	return TrackHelpers::Interpolate(start, end, t);
}

template<typename T>
T SampleCubic(const Track<T>& track, float time, bool looping)
{
	const auto thisFrameIndex = FrameIndex(track, time, looping);
	if (thisFrameIndex < 0 || thisFrameIndex >= static_cast<int>(track.frames.size() - 1))
	{
		return T();
	}
	const auto nextFrameIndex = thisFrameIndex + 1;
	const auto& thisFrame = track.frames[static_cast<unsigned int>(thisFrameIndex)];
	const auto& nextFrame = track.frames[static_cast<unsigned int>(nextFrameIndex)];

	float trackTime = AdjustTimeToFitTrack(track, time, looping);
	float frameDelta = nextFrame.time - thisFrame.time;
	if (frameDelta <= 0.0f)
	{
		return T();
	}
	float t = (trackTime - thisFrame.time) / frameDelta;

	T point1 = thisFrame.value;
	T slope1 = thisFrame.out * frameDelta;

	T point2 = nextFrame.value;
	T slope2 = nextFrame.in * frameDelta;

	return Hermite(t, point1, slope1, point2, slope2);
}

template<typename T>
Track<T>::Track(const std::vector<Frame<T>>& fra, Interpolation in)
	: frames(fra)
	, interpolation(in)
{
}

template<typename T>
float Track<T>::get_start_time()
{
	return frames[0].time;
}

template<typename T>
float Track<T>::get_end_time()
{
	return frames[frames.size() - 1].time;
}

template<typename T>
T Track<T>::get_sample(float time, bool looping)
{
	if (interpolation == Interpolation::Constant)
	{
		return SampleConstant(*this, time, looping);
	}
	else if (interpolation == Interpolation::Linear)
	{
		return SampleLinear(*this, time, looping);
	}
	return SampleCubic(*this, time, looping);
}

template<typename T>
bool Track<T>::is_valid() const
{
	return frames.size() > 1;
}

template struct Track<float>;
template struct Track<vec3>;
template struct Track<quat>;
