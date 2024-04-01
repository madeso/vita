#include "vita/anim/track.h"

#include <cmath>
#include <cstring>

namespace linear_track_helpers
{
float inline interpolate(float a, float b, float t)
{
	return a + (b - a) * t;
}

vec3 inline interpolate(const vec3& a, const vec3& b, float t)
{
	return lerp(a, b, t);
}

quat inline interpolate(const quat& a, const quat& b, float t)
{
	return dot(a, b) < 0 ? nlerp(a, -b, t) : nlerp(a, b, t);
}
}  //  namespace linear_track_helpers

namespace hermite_track_helpers
{
float inline adjust_hermite_result(float f)
{
	return f;
}

vec3 inline adjust_hermite_result(const vec3& v)
{
	return v;
}

quat inline adjust_hermite_result(const quat& q)
{
	return get_normalized(q);
}

float inline get_neighborhood(float, float rhs)
{
	return rhs;
}

const vec3 inline& get_neighborhood(const vec3&, const vec3& rhs)
{
	return rhs;
}

quat inline get_neighborhood(const quat& lhs, const quat& rhs)
{
	return dot(lhs, rhs) < 0 ? -rhs : rhs;
}
};	//  namespace hermite_track_helpers

template<typename T>
T calc_hermite_interpolation(float t, const T& p1, const T& s1, const T& ap2, const T& s2)
{
	float tt = t * t;
	float ttt = tt * t;

	const auto p2 = hermite_track_helpers::get_neighborhood(p1, ap2);

	float h1 = 2.0f * ttt - 3.0f * tt + 1.0f;
	float h2 = -2.0f * ttt + 3.0f * tt;
	float h3 = ttt - 2.0f * tt + t;
	float h4 = ttt - tt;

	T result = p1 * h1 + p2 * h2 + s1 * h3 + s2 * h4;
	return hermite_track_helpers::adjust_hermite_result(result);
}

template<typename T>
int find_frame_index(const Track<T>& track, float time, bool looping)
{
	if (track.is_valid() == false)
	{
		return -1;
	}

	const auto number_of_frames = track.frames.size();
	if (looping)
	{
		const auto start_time = track.frames[0].time;
		const auto end_time = track.frames[number_of_frames - 1].time;

		time = std::fmod(time - start_time, end_time - start_time);
		if (time < 0.0f)
		{
			time += end_time - start_time;
		}
		time = time + start_time;
	}
	else
	{
		if (time <= track.frames[0].time)
		{
			return 0;
		}
		if (time >= track.frames[number_of_frames - 2].time)
		{
			return static_cast<int>(number_of_frames) - 2;
		}
	}

	for (int i = static_cast<int>(number_of_frames) - 1; i >= 0; --i)
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
float adjust_time_to_fit_track(const Track<T>& track, float time, bool looping)
{
	if (track.is_valid() == false)
	{
		return 0.0f;
	}

	const auto number_of_frames = static_cast<unsigned int>(track.frames.size());
	const auto start_time = track.frames[0].time;
	const auto end_time = track.frames[number_of_frames - 1].time;
	const auto duration = end_time - start_time;
	if (duration <= 0.0f)
	{
		return 0.0f;
	}
	if (looping)
	{
		auto ret = std::fmod(time - start_time, duration);
		if (ret < 0.0f)
		{
			ret += duration;
		}
		return ret + start_time;
	}
	else
	{
		if (time <= start_time)
		{
			return start_time;
		}
		else if (time >= end_time)
		{
			return end_time;
		}
		else
		{
			return time;
		}
	}
}

template<typename T>
T get_sample_constant(const Track<T>& track, float time, bool looping)
{
	int frame = find_frame_index(track, time, looping);
	if (frame < 0 || frame >= static_cast<int>(track.frames.size()))
	{
		return T();
	}

	return track.frames[static_cast<unsigned int>(frame)].value;
}

template<typename T>
T get_sample_linear(const Track<T>& track, float time, bool looping)
{
	const auto this_frame_index = find_frame_index(track, time, looping);
	if (this_frame_index < 0 || this_frame_index >= static_cast<int>(track.frames.size() - 1))
	{
		return T();
	}
	const auto next_frame_index = this_frame_index + 1;
	const auto& this_frame = track.frames[static_cast<unsigned int>(this_frame_index)];
	const auto& next_frame = track.frames[static_cast<unsigned int>(next_frame_index)];

	const auto track_time = adjust_time_to_fit_track(track, time, looping);
	const auto frame_delta = next_frame.time - this_frame.time;
	if (frame_delta <= 0.0f)
	{
		return T();
	}

	const auto t = (track_time - this_frame.time) / frame_delta;
	const auto start = this_frame.value;
	const auto end = next_frame.value;

	return linear_track_helpers::interpolate(start, end, t);
}

template<typename T>
T get_sample_cubic(const Track<T>& track, float time, bool looping)
{
	const auto this_frame_index = find_frame_index(track, time, looping);
	if (this_frame_index < 0 || this_frame_index >= static_cast<int>(track.frames.size() - 1))
	{
		return T();
	}
	const auto next_frame_index = this_frame_index + 1;
	const auto& this_frame = track.frames[static_cast<unsigned int>(this_frame_index)];
	const auto& next_frame = track.frames[static_cast<unsigned int>(next_frame_index)];

	const auto track_time = adjust_time_to_fit_track(track, time, looping);
	const auto frame_delta = next_frame.time - this_frame.time;
	if (frame_delta <= 0.0f)
	{
		return T();
	}

	const auto t = (track_time - this_frame.time) / frame_delta;

	const auto point1 = this_frame.value;
	const auto slope1 = this_frame.out * frame_delta;

	const auto point2 = next_frame.value;
	const auto slope2 = next_frame.in * frame_delta;

	return calc_hermite_interpolation(t, point1, slope1, point2, slope2);
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
	switch (interpolation)
	{
	case Interpolation::Constant: return get_sample_constant(*this, time, looping);
	case Interpolation::Linear: return get_sample_linear(*this, time, looping);
	case Interpolation::Cubic: return get_sample_cubic(*this, time, looping);
	}
}

template<typename T>
bool Track<T>::is_valid() const
{
	return frames.size() > 1;
}

namespace for_testing
{
int find_frame_index_scalar(const ScalarTrack& track, float time, bool looping)
{
	return find_frame_index(track, time, looping);
}

float adjust_time_to_fit_track_scalar(const ScalarTrack& track, float time, bool looping)
{
	return adjust_time_to_fit_track(track, time, looping);
}
}  //  namespace for_testing

template struct Track<float>;
template struct Track<vec3>;
template struct Track<quat>;
