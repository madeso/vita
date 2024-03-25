
#include <vector>

#include "vita/vita.h"
#include "vita/anim/debugdraw.h"
#include "vita/anim/track.h"

struct Sample : public App
{
	std::vector<ScalarTrack> mScalarTracks;
	std::vector<bool> mScalarTracksLooping;

	DebugDraw* mScalarTrackLines;
	DebugDraw* mHandleLines;
	DebugDraw* mHandlePoints;
	DebugDraw* mReferenceLines;

	ScalarFrame MakeFrame(float time, float value);
	ScalarFrame MakeFrame(float time, float in, float value, float out);
	VectorFrame MakeFrame(float time, const vec3& value);
	VectorFrame MakeFrame(float time, const vec3& in, const vec3& value, const vec3& out);
	QuaternionFrame MakeFrame(float time, const quat& value);
	QuaternionFrame MakeFrame(float time, const quat& in, const quat& out, const quat& value);
	ScalarTrack MakeScalarTrack(Interpolation interp, unsigned int numFrames, ...);
	VectorTrack MakeVectorTrack(Interpolation interp, unsigned int numFrames, ...);
	QuaternionTrack MakeQuaternionTrack(Interpolation interp, unsigned int numFrames, ...);

	Sample();
	~Sample();

	void Render(float inAspectRatio);

	void on_frame(float) override
	{
	}

	void on_render(float aspect) override
	{
		Render(aspect);
	}

	void on_gui() override
	{
	}
};

#include <iostream>
#include <cstdarg>

Sample::Sample()
{
	mScalarTracks.push_back(
		MakeScalarTrack(Interpolation::Linear, 2, MakeFrame(0.0f, 0.0f), MakeFrame(1.0f, 1.0f))
	);
	mScalarTracksLooping.push_back(false);

	mScalarTracks.push_back(
		MakeScalarTrack(Interpolation::Linear, 2, MakeFrame(0.0f, 0.0f), MakeFrame(0.5f, 1.0f))
	);
	mScalarTracksLooping.push_back(false);

	mScalarTracks.push_back(MakeScalarTrack(
		Interpolation::Linear,
		3,
		MakeFrame(0.25f, 0.0f),
		MakeFrame(0.5f, 1.0f),
		MakeFrame(0.75f, 0.0f)
	));
	mScalarTracksLooping.push_back(true);

	mScalarTracks.push_back(MakeScalarTrack(
		Interpolation::Linear,
		3,
		MakeFrame(0.25f, 0.0f),
		MakeFrame(0.5f, 1.0f),
		MakeFrame(0.75f, 0.0f)
	));
	mScalarTracksLooping.push_back(false);

	ScalarTrack stepTrack;
	stepTrack.Resize(11);
	stepTrack.SetInterpolation(Interpolation::Constant);
	for (unsigned int i = 0; i < 11; ++i)
	{
		float time = static_cast<float>(i) / 10.0f * 0.5f + 0.25f;
		stepTrack[i] = MakeFrame(time, (i % 2 == 0.0f) ? 0.0f : 1.0f);
	}
	mScalarTracks.push_back(stepTrack);
	mScalarTracks.push_back(stepTrack);
	mScalarTracksLooping.push_back(true);
	mScalarTracksLooping.push_back(false);

	mScalarTracks.push_back(MakeScalarTrack(
		Interpolation::Cubic,
		2,
		MakeFrame(0.25f, 0.676221f, 0.0f, 0.676221f),
		MakeFrame(0.75f, 4.043837f, 1.0f, 4.043837f)
	));
	mScalarTracksLooping.push_back(false);

	mScalarTracks.push_back(MakeScalarTrack(
		Interpolation::Cubic,
		3,
		MakeFrame(0.25f, 0, 0, 0),
		MakeFrame(0.5f, 0, 1, 0),
		MakeFrame(0.75f, 0, 0, 0)
	));
	mScalarTracksLooping.push_back(true);

	mScalarTracks.push_back(MakeScalarTrack(
		Interpolation::Cubic,
		5,
		MakeFrame(0.25f, 0, 0, 0),
		MakeFrame(0.3833333f, -10.11282f, 0.5499259f, -10.11282f),
		MakeFrame(0.5f, 25.82528f, 1, 25.82528f),
		MakeFrame(0.6333333f, 7.925411f, 0.4500741f, 7.925411f),
		MakeFrame(0.75f, 0, 0, 0)
	));
	mScalarTracksLooping.push_back(true);

	mScalarTracks.push_back(MakeScalarTrack(
		Interpolation::Cubic,
		5,
		MakeFrame(0.25f, 0, 0, 0),
		MakeFrame(0.3833333f, 13.25147f, 0.5499259f, -10.11282f),
		MakeFrame(0.5f, 10.2405f, 1, -5.545671f),
		MakeFrame(0.6333333f, 7.925411f, 0.4500741f, -11.40672f),
		MakeFrame(0.75f, 0, 0, 0)
	));
	mScalarTracksLooping.push_back(true);

	mScalarTrackLines = new DebugDraw();
	mHandlePoints = new DebugDraw();
	mHandleLines = new DebugDraw();
	mReferenceLines = new DebugDraw();

	// Range in both X and Y is 0 to 20

	for (unsigned int i = 0; i < 10; ++i)
	{
		float yPosition = (static_cast<float>(i) * 2.0f) + (static_cast<float>(i) * 0.2f) + 0.1f;
		float height = 1.8f;

		float left = 1.0f;
		float right = 14.0;

		mReferenceLines->Push(vec3(left, yPosition, 0));
		mReferenceLines->Push(vec3(left, yPosition + height, 0));

		mReferenceLines->Push(vec3(left, yPosition, 0));
		mReferenceLines->Push(vec3(right, yPosition, 0));
	}
	mReferenceLines->UpdateOpenGLBuffers();

	// Safe to assume that mScalarTracks has a size of 10

	for (unsigned int i = 0; i < mScalarTracks.size(); ++i)
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

			float thisY = mScalarTracks[i].Sample(thisJNorm, mScalarTracksLooping[i]);
			float nextY = mScalarTracks[i].Sample(nextJNorm, mScalarTracksLooping[i]);

			thisY = yPosition + thisY * height;
			nextY = yPosition + nextY * height;

			mScalarTrackLines->Push(vec3(thisX, thisY, 0.1f));
			mScalarTrackLines->Push(vec3(nextX, nextY, 0.1f));
		}

		unsigned int numFrames = mScalarTracks[i].Size();
		for (unsigned int j = 0; j < numFrames; ++j)
		{
			float thisTime = mScalarTracks[i][j].mTime;
			float thisY
				= yPosition + mScalarTracks[i].Sample(thisTime, mScalarTracksLooping[i]) * height;
			float thisX = left + thisTime * xRange;
			mHandlePoints->Push(vec3(thisX, thisY, 0.9f));

			if (j > 0)
			{
				float prevY
					= yPosition
					+ mScalarTracks[i].Sample(thisTime - 0.0005f, mScalarTracksLooping[i]) * height;
				float prevX = left + (thisTime - 0.0005f) * xRange;

				vec3 thisVec = vec3(thisX, thisY, 0.6f);
				vec3 prevVec = vec3(prevX, prevY, 0.6f);
				vec3 handleVec = thisVec + get_normalized(prevVec - thisVec) * 0.75f;

				mHandleLines->Push(thisVec);
				mHandleLines->Push(handleVec);
			}

			if (j < numFrames - 1 && mScalarTracks[i].GetInterpolation() != Interpolation::Constant)
			{
				float nextY
					= yPosition
					+ mScalarTracks[i].Sample(thisTime + 0.0005f, mScalarTracksLooping[i]) * height;
				float nextX = left + (thisTime + 0.0005f) * xRange;

				vec3 thisVec = vec3(thisX, thisY, 0.6f);
				vec3 nextVec = vec3(nextX, nextY, 0.6f);
				vec3 handleVec = thisVec + get_normalized(nextVec - thisVec) * 0.75f;

				mHandleLines->Push(thisVec);
				mHandleLines->Push(handleVec);
			}
		}
	}
	mScalarTrackLines->UpdateOpenGLBuffers();
	mHandleLines->UpdateOpenGLBuffers();
	mHandlePoints->UpdateOpenGLBuffers();
}

void Sample::Render(float inAspectRatio)
{
	mat4 model;
	mat4 view = mat4_from_look_at(vec3(0, 0, 5), vec3(0, 0, 0), vec3(0, 1, 0));
	mat4 projection = mat4_from_ortho(0, inAspectRatio * 22.0f, 0, 22, 0.001f, 10);

	mat4 mvp = projection * view * model;

	mReferenceLines->Draw(DebugDrawMode::Lines, vec3(1, 1, 1), mvp);
	mScalarTrackLines->Draw(DebugDrawMode::Lines, vec3(0, 1, 0), mvp);
	mHandleLines->Draw(DebugDrawMode::Lines, vec3(1, 0, 0), mvp);
	mHandlePoints->Draw(DebugDrawMode::Points, vec3(0, 0, 1), mvp);
}

Sample::~Sample()
{
	mScalarTracks.clear();
	mScalarTracksLooping.clear();

	delete mScalarTrackLines;
	delete mHandlePoints;
	delete mHandleLines;
	delete mReferenceLines;
}

ScalarFrame Sample::MakeFrame(float time, float value)
{
	return MakeFrame(time, 0.0f, value, 0.0f);
}

VectorFrame Sample::MakeFrame(float time, const vec3& value)
{
	return MakeFrame(time, vec3(), value, vec3());
}

QuaternionFrame Sample::MakeFrame(float time, const quat& value)
{
	return MakeFrame(time, quat(0, 0, 0, 0), value, quat(0, 0, 0, 0));
}

ScalarFrame Sample::MakeFrame(float time, float in, float value, float out)
{
	ScalarFrame result;
	result.mTime = time;
	result.mIn[0] = in;
	result.mValue[0] = value;
	result.mOut[0] = out;
	return result;
}

VectorFrame Sample::MakeFrame(float time, const vec3& in, const vec3& value, const vec3& out)
{
	VectorFrame result;
	result.mTime = time;
	result.mIn[0] = in.x;
	result.mIn[1] = in.y;
	result.mIn[2] = in.z;
	result.mValue[0] = value.x;
	result.mValue[1] = value.y;
	result.mValue[2] = value.z;
	result.mOut[0] = out.x;
	result.mOut[1] = out.y;
	result.mOut[2] = out.z;
	return result;
}

QuaternionFrame Sample::MakeFrame(float time, const quat& in, const quat& out, const quat& value)
{
	QuaternionFrame result;
	result.mTime = time;
	result.mIn[0] = in.x;
	result.mIn[1] = in.y;
	result.mIn[2] = in.z;
	result.mIn[3] = in.w;
	result.mValue[0] = value.x;
	result.mValue[1] = value.y;
	result.mValue[2] = value.z;
	result.mValue[3] = value.w;
	result.mOut[0] = out.x;
	result.mOut[1] = out.y;
	result.mOut[2] = out.z;
	result.mOut[3] = out.w;
	return result;
}

ScalarTrack Sample::MakeScalarTrack(Interpolation interp, unsigned int numFrames, ...)
{
	ScalarTrack result;
	result.SetInterpolation(interp);
	result.Resize(numFrames);

	va_list args;
	va_start(args, numFrames);

	for (unsigned int i = 0; i < numFrames; ++i)
	{
		result[i] = va_arg(args, ScalarFrame);
	}

	va_end(args);

	return result;
}

VectorTrack Sample::MakeVectorTrack(Interpolation interp, unsigned int numFrames, ...)
{
	VectorTrack result;
	result.SetInterpolation(interp);
	result.Resize(numFrames);

	va_list args;
	va_start(args, numFrames);

	for (unsigned int i = 0; i < numFrames; ++i)
	{
		result[i] = va_arg(args, VectorFrame);
	}

	va_end(args);

	return result;
}

QuaternionTrack Sample::MakeQuaternionTrack(Interpolation interp, unsigned int numFrames, ...)
{
	QuaternionTrack result;
	result.SetInterpolation(interp);
	result.Resize(numFrames);

	va_list args;
	va_start(args, numFrames);

	for (unsigned int i = 0; i < numFrames; ++i)
	{
		result[i] = va_arg(args, QuaternionFrame);
	}

	va_end(args);

	return result;
}


IMPLEMENT_MAIN(Sample)
