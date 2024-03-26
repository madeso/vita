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
Track<T>::Track(const std::vector<Frame<T>>& fra, Interpolation in)
	: mFrames(fra)
	, mInterpolation(in)
{
}

template<typename T>
float Track<T>::GetStartTime()
{
	return mFrames[0].mTime;
}

template<typename T>
float Track<T>::GetEndTime()
{
	return mFrames[mFrames.size() - 1].mTime;
}

template<typename T>
T Track<T>::Sample(float time, bool looping)
{
	if (mInterpolation == Interpolation::Constant)
	{
		return SampleConstant(time, looping);
	}
	else if (mInterpolation == Interpolation::Linear)
	{
		return SampleLinear(time, looping);
	}
	return SampleCubic(time, looping);
}

template<typename T>
Frame<T>& Track<T>::operator[](unsigned int index)
{
	return mFrames[index];
}

template<typename T>
void Track<T>::Resize(unsigned int size)
{
	mFrames.resize(size);
}

template<typename T>
unsigned int Track<T>::Size()
{
	return static_cast<unsigned int>(mFrames.size());
}

template<typename T>
Interpolation Track<T>::GetInterpolation()
{
	return mInterpolation;
}

template<typename T>
void Track<T>::SetInterpolation(Interpolation interpolation)
{
	mInterpolation = interpolation;
}

template<typename T>
T Track<T>::Hermite(float t, const T& p1, const T& s1, const T& _p2, const T& s2)
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
int Track<T>::FrameIndex(float time, bool looping)
{
	unsigned int size = static_cast<unsigned int>(mFrames.size());
	if (size <= 1)
	{
		return -1;
	}
	if (looping)
	{
		float startTime = mFrames[0].mTime;
		float endTime = mFrames[size - 1].mTime;
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
		if (time <= mFrames[0].mTime)
		{
			return 0;
		}
		if (time >= mFrames[size - 2].mTime)
		{
			return static_cast<int>(size) - 2;
		}
	}
	for (int i = static_cast<int>(size) - 1; i >= 0; --i)
	{
		if (time >= mFrames[static_cast<unsigned int>(i)].mTime)
		{
			return i;
		}
	}
	// Invalid code, we should not reach here!
	return -1;
}

template<typename T>
float Track<T>::AdjustTimeToFitTrack(float time, bool looping)
{
	unsigned int size = static_cast<unsigned int>(mFrames.size());
	if (size <= 1)
	{
		return 0.0f;
	}

	float startTime = mFrames[0].mTime;
	float endTime = mFrames[size - 1].mTime;
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
		if (time <= mFrames[0].mTime)
		{
			time = startTime;
		}
		if (time >= mFrames[size - 1].mTime)
		{
			time = endTime;
		}
	}

	return time;
}

template<typename T>
T Track<T>::SampleConstant(float time, bool looping)
{
	int frame = FrameIndex(time, looping);
	if (frame < 0 || frame >= static_cast<int>(mFrames.size()))
	{
		return T();
	}

	return mFrames[static_cast<unsigned int>(frame)].mValue;
}

template<typename T>
T Track<T>::SampleLinear(float time, bool looping)
{
	int thisFrame = FrameIndex(time, looping);
	if (thisFrame < 0 || thisFrame >= static_cast<int>(mFrames.size() - 1))
	{
		return T();
	}
	int nextFrame = thisFrame + 1;

	float trackTime = AdjustTimeToFitTrack(time, looping);
	float frameDelta = mFrames[static_cast<unsigned int>(nextFrame)].mTime
					 - mFrames[static_cast<unsigned int>(thisFrame)].mTime;
	if (frameDelta <= 0.0f)
	{
		return T();
	}
	float t = (trackTime - mFrames[static_cast<unsigned int>(thisFrame)].mTime) / frameDelta;

	T start = mFrames[static_cast<unsigned int>(thisFrame)].mValue;
	T end = mFrames[static_cast<unsigned int>(nextFrame)].mValue;

	return TrackHelpers::Interpolate(start, end, t);
}

template<typename T>
T Track<T>::SampleCubic(float time, bool looping)
{
	int thisFrame = FrameIndex(time, looping);
	if (thisFrame < 0 || thisFrame >= static_cast<int>(mFrames.size() - 1))
	{
		return T();
	}
	int nextFrame = thisFrame + 1;

	float trackTime = AdjustTimeToFitTrack(time, looping);
	float frameDelta = mFrames[static_cast<unsigned int>(nextFrame)].mTime
					 - mFrames[static_cast<unsigned int>(thisFrame)].mTime;
	if (frameDelta <= 0.0f)
	{
		return T();
	}
	float t = (trackTime - mFrames[static_cast<unsigned int>(thisFrame)].mTime) / frameDelta;

	T point1 = mFrames[static_cast<unsigned int>(thisFrame)].mValue;
	T slope1 = mFrames[static_cast<unsigned int>(thisFrame)].mOut * frameDelta;

	T point2 = mFrames[static_cast<unsigned int>(nextFrame)].mValue;
	T slope2 = mFrames[static_cast<unsigned int>(nextFrame)].mIn * frameDelta;

	return Hermite(t, point1, slope1, point2, slope2);
}

template struct Track<float>;
template struct Track<vec3>;
template struct Track<quat>;
