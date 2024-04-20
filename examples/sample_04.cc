#include "vita/vita.h"

#include <vector>

#include "vita/anim/debugdraw.h"
#include "vita/anim/pose.h"
#include "vita/anim/clip.h"
#include "vita/anim/skeleton.h"
#include "vita/anim/gltfloader.h"
#include "vita/assets.h"

struct Sample : public App
{
	Skeleton mSkeleton;
	Pose mCurrentPose;
	std::vector<Clip> mClips;
	std::size_t mCurrentClip;
	float mPlaybackTime;
	DebugDraw* mBindPoseVisual;
	DebugDraw* mRestPoseVisual;
	DebugDraw* mCurrentPoseVisual;

	Sample();
	~Sample();

	void on_frame(float deltaTime) override;
	void on_render(float inAspectRatio) override;

	void on_gui() override
	{
	}
};

Sample::Sample()
{
	cgltf_data* gltf = load_gltf_file(assets::woman_gltf());
	mSkeleton = LoadSkeleton(gltf);
	mClips = get_animation_clips(gltf);
	free_gltf_file(gltf);

	mRestPoseVisual = new DebugDraw();
	mRestPoseVisual->FromPose(mSkeleton.rest_pose);
	mRestPoseVisual->UpdateOpenGLBuffers();

	mBindPoseVisual = new DebugDraw();
	mBindPoseVisual->FromPose(mSkeleton.bind_pose);
	mBindPoseVisual->UpdateOpenGLBuffers();

	mCurrentClip = 0;
	mCurrentPose = mSkeleton.rest_pose;

	mCurrentPoseVisual = new DebugDraw();
	mCurrentPoseVisual->FromPose(mCurrentPose);
	mCurrentPoseVisual->UpdateOpenGLBuffers();

	// For the UI
	const auto numUIClips = mClips.size();
	for (std::size_t i = 0; i < numUIClips; ++i)
	{
		if (mClips[i].name == "Walking")
		{
			mCurrentClip = i;
			break;
		}
	}
}

void Sample::on_frame(float deltaTime)
{
	mPlaybackTime = mClips[mCurrentClip].sample_to_pose(mCurrentPose, mPlaybackTime + deltaTime);
	mCurrentPoseVisual->FromPose(mCurrentPose);
}

void Sample::on_render(float inAspectRatio)
{
	mat4 projection = mat4_from_perspective(60.0f, inAspectRatio, 0.01f, 1000.0f);
	mat4 view = mat4_from_look_at(vec3(0, 4, -7), vec3(0, 4, 0), vec3(0, 1, 0));
	mat4 mvp = projection * view;  // No model

	mRestPoseVisual->Draw(DebugDrawMode::Lines, vec3(1, 0, 0), mvp);

	mCurrentPoseVisual->UpdateOpenGLBuffers();
	mCurrentPoseVisual->Draw(DebugDrawMode::Lines, vec3(0, 0, 1), mvp);

	mBindPoseVisual->Draw(DebugDrawMode::Lines, vec3(0, 1, 0), mvp);
}

Sample::~Sample()
{
	delete mRestPoseVisual;
	delete mCurrentPoseVisual;
	delete mBindPoseVisual;
	mClips.clear();
}

IMPLEMENT_MAIN(Sample)
