
#include <vector>

#include <iostream>

#include "vita/vita.h"
#include "vita/anim/debugdraw.h"
#include "vita/anim/track.h"

struct Sample : public App
{
	std::vector<ScalarTrack> scalar_tracks;
	std::vector<bool> scalar_tracks_looping;

	DebugDraw* draw_track_lines;
	DebugDraw* draw_handle_lines;
	DebugDraw* draw_handle_points;
	DebugDraw* draw_referene_lines;

	ScalarFrame MakeFrame(float time, float value)
	{
		return MakeFrame(time, 0.0f, value, 0.0f);
	}

	ScalarFrame MakeFrame(float time, float in, float value, float out)
	{
		return {time, in, out, value};
	}

	ScalarTrack MakeScalarTrack(Interpolation interp, const std::vector<ScalarFrame>& frames)
	{
		return {frames, interp};
	}

	Sample()
	{
		scalar_tracks.push_back(
			MakeScalarTrack(Interpolation::Linear, {MakeFrame(0.0f, 0.0f), MakeFrame(1.0f, 1.0f)})
		);
		scalar_tracks_looping.push_back(false);

		scalar_tracks.push_back(
			MakeScalarTrack(Interpolation::Linear, {MakeFrame(0.0f, 0.0f), MakeFrame(0.5f, 1.0f)})
		);
		scalar_tracks_looping.push_back(false);

		scalar_tracks.push_back(MakeScalarTrack(
			Interpolation::Linear,
			{MakeFrame(0.25f, 0.0f), MakeFrame(0.5f, 1.0f), MakeFrame(0.75f, 0.0f)}
		));
		scalar_tracks_looping.push_back(true);

		scalar_tracks.push_back(MakeScalarTrack(
			Interpolation::Linear,
			{MakeFrame(0.25f, 0.0f), MakeFrame(0.5f, 1.0f), MakeFrame(0.75f, 0.0f)}
		));
		scalar_tracks_looping.push_back(false);

		ScalarTrack stepTrack{{}, Interpolation::Constant};
		stepTrack.frames.resize(11);
		for (std::size_t i = 0; i < 11; ++i)
		{
			float time = static_cast<float>(i) / 10.0f * 0.5f + 0.25f;
			stepTrack.frames[i] = MakeFrame(time, (i % 2 == 0.0f) ? 0.0f : 1.0f);
		}
		scalar_tracks.push_back(stepTrack);
		scalar_tracks.push_back(stepTrack);
		scalar_tracks_looping.push_back(true);
		scalar_tracks_looping.push_back(false);

		scalar_tracks.push_back(MakeScalarTrack(
			Interpolation::Cubic,
			{MakeFrame(0.25f, 0.676221f, 0.0f, 0.676221f),
			 MakeFrame(0.75f, 4.043837f, 1.0f, 4.043837f)}
		));
		scalar_tracks_looping.push_back(false);

		scalar_tracks.push_back(MakeScalarTrack(
			Interpolation::Cubic,
			{MakeFrame(0.25f, 0, 0, 0), MakeFrame(0.5f, 0, 1, 0), MakeFrame(0.75f, 0, 0, 0)}
		));
		scalar_tracks_looping.push_back(true);

		scalar_tracks.push_back(MakeScalarTrack(
			Interpolation::Cubic,
			{MakeFrame(0.25f, 0, 0, 0),
			 MakeFrame(0.3833333f, -10.11282f, 0.5499259f, -10.11282f),
			 MakeFrame(0.5f, 25.82528f, 1, 25.82528f),
			 MakeFrame(0.6333333f, 7.925411f, 0.4500741f, 7.925411f),
			 MakeFrame(0.75f, 0, 0, 0)}
		));
		scalar_tracks_looping.push_back(true);

		scalar_tracks.push_back(MakeScalarTrack(
			Interpolation::Cubic,
			{MakeFrame(0.25f, 0, 0, 0),
			 MakeFrame(0.3833333f, 13.25147f, 0.5499259f, -10.11282f),
			 MakeFrame(0.5f, 10.2405f, 1, -5.545671f),
			 MakeFrame(0.6333333f, 7.925411f, 0.4500741f, -11.40672f),
			 MakeFrame(0.75f, 0, 0, 0)}
		));
		scalar_tracks_looping.push_back(true);

		draw_track_lines = new DebugDraw();
		draw_handle_points = new DebugDraw();
		draw_handle_lines = new DebugDraw();
		draw_referene_lines = new DebugDraw();

		// Range in both X and Y is 0 to 20

		for (unsigned int i = 0; i < 10; ++i)
		{
			float yPosition
				= (static_cast<float>(i) * 2.0f) + (static_cast<float>(i) * 0.2f) + 0.1f;
			float height = 1.8f;

			float left = 1.0f;
			float right = 14.0;

			draw_referene_lines->Push(vec3(left, yPosition, 0));
			draw_referene_lines->Push(vec3(left, yPosition + height, 0));

			draw_referene_lines->Push(vec3(left, yPosition, 0));
			draw_referene_lines->Push(vec3(right, yPosition, 0));
		}
		draw_referene_lines->UpdateOpenGLBuffers();

		// Safe to assume that scalar_tracks has a size of 10

		for (unsigned int i = 0; i < scalar_tracks.size(); ++i)
		{
			float yPosition
				= (static_cast<float>(9 - i) * 2.0f) + (static_cast<float>(9 - i) * 0.2f) + 0.1f;
			float height = 1.8f;

			float left = 1.0f;
			float right = 14.0;
			float xRange = right - left;

			for (unsigned int j = 1; j < 150; ++j)
			{
				float thisJNorm = static_cast<float>(j - 1) / 149.0f;
				float nextJNorm = static_cast<float>(j) / 149.0f;

				float thisX = left + thisJNorm * xRange;
				float nextX = left + nextJNorm * xRange;

				float thisY = scalar_tracks[i].get_sample(thisJNorm, scalar_tracks_looping[i]);
				float nextY = scalar_tracks[i].get_sample(nextJNorm, scalar_tracks_looping[i]);

				thisY = yPosition + thisY * height;
				nextY = yPosition + nextY * height;

				draw_track_lines->Push(vec3(thisX, thisY, 0.1f));
				draw_track_lines->Push(vec3(nextX, nextY, 0.1f));
			}

			const auto numFrames = static_cast<unsigned int>(scalar_tracks[i].frames.size());
			for (unsigned int j = 0; j < numFrames; ++j)
			{
				float thisTime = scalar_tracks[i].frames[j].time;
				float thisY
					= yPosition
					+ scalar_tracks[i].get_sample(thisTime, scalar_tracks_looping[i]) * height;
				float thisX = left + thisTime * xRange;
				draw_handle_points->Push(vec3(thisX, thisY, 0.9f));

				if (j > 0)
				{
					float prevY
						= yPosition
						+ scalar_tracks[i].get_sample(thisTime - 0.0005f, scalar_tracks_looping[i])
							  * height;
					float prevX = left + (thisTime - 0.0005f) * xRange;

					vec3 thisVec = vec3(thisX, thisY, 0.6f);
					vec3 prevVec = vec3(prevX, prevY, 0.6f);
					vec3 handleVec = thisVec + get_normalized(prevVec - thisVec) * 0.75f;

					draw_handle_lines->Push(thisVec);
					draw_handle_lines->Push(handleVec);
				}

				if (j < numFrames - 1 && scalar_tracks[i].interpolation != Interpolation::Constant)
				{
					float nextY
						= yPosition
						+ scalar_tracks[i].get_sample(thisTime + 0.0005f, scalar_tracks_looping[i])
							  * height;
					float nextX = left + (thisTime + 0.0005f) * xRange;

					vec3 thisVec = vec3(thisX, thisY, 0.6f);
					vec3 nextVec = vec3(nextX, nextY, 0.6f);
					vec3 handleVec = thisVec + get_normalized(nextVec - thisVec) * 0.75f;

					draw_handle_lines->Push(thisVec);
					draw_handle_lines->Push(handleVec);
				}
			}
		}
		draw_track_lines->UpdateOpenGLBuffers();
		draw_handle_lines->UpdateOpenGLBuffers();
		draw_handle_points->UpdateOpenGLBuffers();
	}

	~Sample()
	{
		scalar_tracks.clear();
		scalar_tracks_looping.clear();

		delete draw_track_lines;
		delete draw_handle_points;
		delete draw_handle_lines;
		delete draw_referene_lines;
	}

	void on_frame(float) override
	{
	}

	void on_render(float inAspectRatio) override
	{
		mat4 model;
		mat4 view = mat4_from_look_at(vec3(0, 0, 5), vec3(0, 0, 0), vec3(0, 1, 0));
		mat4 projection = mat4_from_ortho(0, inAspectRatio * 22.0f, 0, 22, 0.001f, 10);

		mat4 mvp = projection * view * model;

		draw_referene_lines->Draw(DebugDrawMode::Lines, vec3(1, 1, 1), mvp);
		draw_track_lines->Draw(DebugDrawMode::Lines, vec3(0, 1, 0), mvp);
		draw_handle_lines->Draw(DebugDrawMode::Lines, vec3(1, 0, 0), mvp);
		draw_handle_points->Draw(DebugDrawMode::Points, vec3(0, 0, 1), mvp);
	}

	void on_gui() override
	{
	}
};

IMPLEMENT_MAIN(Sample)
