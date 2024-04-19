#include "vita/vita.h"

#include <vector>
// #include "glad.h"

#include "vita/anim/pose.h"
#include "vita/anim/clip.h"
#include "vita/anim/skeleton.h"
#include "vita/anim/mesh.h"
#include "vita/anim/texture.h"
#include "vita/anim/shader.h"
#include "vita/anim/gltfloader.h"
#include "vita/anim/uniform.h"
#include "vita/assets.h"

struct AnimationInstance
{
	Pose mAnimatedPose;
	std::vector<mat4> mPosePalette;
	std::size_t mClip;
	float mPlayback;
	Transform mModel;

	AnimationInstance()
		: mClip(0)
		, mPlayback(0.0f)
	{
	}
};

struct Sample : public App
{
	Texture* mDiffuseTexture;
	Shader* mStaticShader;
	Shader* mSkinnedShader;
	std::vector<Mesh> mCPUMeshes;
	std::vector<Mesh> mGPUMeshes;
	Skeleton mSkeleton;
	std::vector<Clip> mClips;

	AnimationInstance mGPUAnimInfo;
	AnimationInstance mCPUAnimInfo;

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
	mCPUMeshes = LoadMeshes(gltf);
	mSkeleton = LoadSkeleton(gltf);
	mClips = get_animation_clips(gltf);
	free_gltf_file(gltf);

	mGPUMeshes = mCPUMeshes;
	for (std::size_t i = 0, size = mGPUMeshes.size(); i < size; ++i)
	{
		mGPUMeshes[i].UpdateOpenGLBuffers();
	}

	mStaticShader = new Shader(assets::static_shader(), assets::lit_shader());
	mSkinnedShader = new Shader(assets::skinned_shader(), assets::lit_shader());
	mDiffuseTexture = new Texture(assets::woman_texture());

	mGPUAnimInfo.mAnimatedPose = mSkeleton.mRestPose;
	mGPUAnimInfo.mPosePalette.resize(mSkeleton.mRestPose.size());
	mCPUAnimInfo.mAnimatedPose = mSkeleton.mRestPose;
	mCPUAnimInfo.mPosePalette.resize(mSkeleton.mRestPose.size());

	mGPUAnimInfo.mModel.position = vec3(-2, 0, 0);
	mCPUAnimInfo.mModel.position = vec3(2, 0, 0);

	const auto numUIClips = mClips.size();
	for (std::size_t i = 0; i < numUIClips; ++i)
	{
		if (mClips[i].name == "Walking")
		{
			mCPUAnimInfo.mClip = i;
		}
		else if (mClips[i].name == "Running")
		{
			mGPUAnimInfo.mClip = i;
		}
	}
}

void Sample::on_frame(float deltaTime)
{
	mCPUAnimInfo.mPlayback = mClips[mCPUAnimInfo.mClip].sample_to_pose(
		mCPUAnimInfo.mAnimatedPose, mCPUAnimInfo.mPlayback + deltaTime
	);
	mGPUAnimInfo.mPlayback = mClips[mGPUAnimInfo.mClip].sample_to_pose(
		mGPUAnimInfo.mAnimatedPose, mGPUAnimInfo.mPlayback + deltaTime
	);

	for (std::size_t i = 0, size = mCPUMeshes.size(); i < size; ++i)
	{
		mCPUMeshes[i].CPUSkin(mSkeleton, mCPUAnimInfo.mAnimatedPose);
	}

	mGPUAnimInfo.mPosePalette = calc_matrix_palette(mGPUAnimInfo.mAnimatedPose);
}

void Sample::on_render(float inAspectRatio)
{
	mat4 projection = mat4_from_perspective(60.0f, inAspectRatio, 0.01f, 1000.0f);
	mat4 view = mat4_from_look_at(vec3(0, 5, 7), vec3(0, 3, 0), vec3(0, 1, 0));
	mat4 model;

	// CPU Skinned Mesh
	model = mat4_from_transform(mCPUAnimInfo.mModel);
	mStaticShader->bind();
	Uniform<mat4>::Set(mStaticShader->get_uniform("model"), model);
	Uniform<mat4>::Set(mStaticShader->get_uniform("view"), view);
	Uniform<mat4>::Set(mStaticShader->get_uniform("projection"), projection);
	Uniform<vec3>::Set(mStaticShader->get_uniform("light"), vec3(1, 1, 1));

	mDiffuseTexture->bind(mStaticShader->get_uniform("tex0"), 0);
	for (std::size_t i = 0, size = mCPUMeshes.size(); i < size; ++i)
	{
		mCPUMeshes[i].Bind(
			static_cast<int>(mStaticShader->get_attribute("position")),
			static_cast<int>(mStaticShader->get_attribute("normal")),
			static_cast<int>(mStaticShader->get_attribute("texCoord")),
			-1,
			-1
		);
		mCPUMeshes[i].Draw();
		mCPUMeshes[i].UnBind(
			static_cast<int>(mStaticShader->get_attribute("position")),
			static_cast<int>(mStaticShader->get_attribute("normal")),
			static_cast<int>(mStaticShader->get_attribute("texCoord")),
			-1,
			-1
		);
	}
	mDiffuseTexture->unbind(0);
	mStaticShader->unbind();

	// GPU Skinned Mesh
	model = mat4_from_transform(mGPUAnimInfo.mModel);
	mSkinnedShader->bind();
	Uniform<mat4>::Set(mSkinnedShader->get_uniform("model"), model);
	Uniform<mat4>::Set(mSkinnedShader->get_uniform("view"), view);
	Uniform<mat4>::Set(mSkinnedShader->get_uniform("projection"), projection);
	Uniform<vec3>::Set(mSkinnedShader->get_uniform("light"), vec3(1, 1, 1));

	Uniform<mat4>::Set(mSkinnedShader->get_uniform("pose"), mGPUAnimInfo.mPosePalette);
	Uniform<mat4>::Set(mSkinnedShader->get_uniform("invBindPose"), mSkeleton.mInvBindPose);

	mDiffuseTexture->bind(mSkinnedShader->get_uniform("tex0"), 0);
	for (std::size_t i = 0, size = mGPUMeshes.size(); i < size; ++i)
	{
		mGPUMeshes[i].Bind(
			static_cast<int>(mSkinnedShader->get_attribute("position")),
			static_cast<int>(mSkinnedShader->get_attribute("normal")),
			static_cast<int>(mSkinnedShader->get_attribute("texCoord")),
			static_cast<int>(mSkinnedShader->get_attribute("weights")),
			static_cast<int>(mSkinnedShader->get_attribute("joints"))
		);
		mGPUMeshes[i].Draw();
		mGPUMeshes[i].UnBind(
			static_cast<int>(mSkinnedShader->get_attribute("position")),
			static_cast<int>(mSkinnedShader->get_attribute("normal")),
			static_cast<int>(mSkinnedShader->get_attribute("texCoord")),
			static_cast<int>(mSkinnedShader->get_attribute("weights")),
			static_cast<int>(mSkinnedShader->get_attribute("joints"))
		);
	}
	mDiffuseTexture->unbind(0);
	mSkinnedShader->unbind();
}

Sample::~Sample()
{
	delete mStaticShader;
	delete mDiffuseTexture;
	delete mSkinnedShader;
	mClips.clear();
	mCPUMeshes.clear();
	mGPUMeshes.clear();
}

IMPLEMENT_MAIN(Sample)
