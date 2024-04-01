#include "catch.hpp"

#include "vita/anim/track.h"

using namespace for_testing;

ScalarFrame F(float t, float v)
{
	return {t, v, 0.0f, 0.0f};
}

TEST_CASE("invalid track")
{
	const auto inter
		= GENERATE(Interpolation::Constant, Interpolation::Linear, Interpolation::Cubic);
	const auto ignored_float = GENERATE(0.0f, 42.0f, -10.0f);
	const auto ignored_time = GENERATE(0.0f, 10.0f, -42.0f);

	const auto empty = ScalarTrack{{}, inter};
	const auto one = ScalarTrack{{F(ignored_float, 0)}, inter};
	CHECK(-1 == find_frame_index_scalar(empty, ignored_time, false));
	CHECK(-1 == find_frame_index_scalar(one, ignored_time, false));
}

TEST_CASE("track test")
{
	const auto inter
		= GENERATE(Interpolation::Constant, Interpolation::Linear, Interpolation::Cubic);

	const auto track = ScalarTrack{{F(0, 0), F(1, 0)}, inter};
	const auto many = ScalarTrack{{F(0, 0), F(1, 0), F(2, 0), F(3, 0)}, inter};

	CHECK(0 == find_frame_index_scalar(track, 0.0f, false));
	CHECK(0 == find_frame_index_scalar(track, 10.0f, false));
	CHECK(0 == find_frame_index_scalar(track, -10.0f, false));

	CHECK(0 == find_frame_index_scalar(many, 0.0f, false));
	CHECK(1 == find_frame_index_scalar(many, 1.5f, false));
	CHECK(2 == find_frame_index_scalar(many, 2.5f, false));

	CHECK(2 == find_frame_index_scalar(many, 3.5f, false));
	CHECK(0 == find_frame_index_scalar(many, 3.5f, true));
	CHECK(1 == find_frame_index_scalar(many, 4.5f, true));

	// todo(Gustav): add test for float adjust_time_to_fit_track_scalar(const ScalarTrack& track, float time, bool looping);
}
