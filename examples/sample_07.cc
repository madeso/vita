#include "vita/vita.h"

#include <vector>

#include "vita/assets.h"

#include "vita/anim/debugdraw.h"
#include "vita/anim/pose.h"
#include "vita/anim/clip.h"
#include "vita/anim/skeleton.h"
#include "vita/anim/mesh.h"
#include "vita/anim/texture.h"
#include "vita/anim/intersections.h"
#include "vita/anim/ikleg.h"
#include "vita/anim/gltfloader.h"
#include "vita/anim/uniform.h"

#include "vita/dependency_glad.h"

// #include "vita/anim/glad.h"
// #include "vita/anim/blending.h"
#include <iostream>

constexpr float ANIM_TIME = 6.0f;

struct Sample : public App
{
	Texture* mCourseTexture;
	std::vector<Mesh> mIKCourse;
	std::vector<Triangle> mTriangles;

	VectorTrack mMotionTrack;
	float mWalkingTime = 0;

	Transform mModel;
	std::vector<Mesh> mMeshes;
	Pose mCurrentPose;
	std::vector<mat4> mPosePalette;
	float mSinkIntoGround;

	IKLeg* mLeftLeg;
	IKLeg* mRightLeg;
	float mToeLength;
	float mLastModelY;

	Texture* mDiffuseTexture;
	Shader* mStaticShader;
	Shader* mSkinnedShader;
	Skeleton mSkeleton;
	std::vector<Clip> mClips;
	std::size_t mCurrentClip;
	float mPlaybackTime;
	DebugDraw* mCurrentPoseVisual;

	bool mShowIKPose;
	bool mShowCurrentPose;
	float mTimeMod;
	bool mDepthTest;
	bool mShowMesh;
	bool mShowEnvironment;

	Sample();
	~Sample();

	void on_frame(float deltaTime) override;
	void on_render(float inAspectRatio) override;

	void on_gui() override
	{
		ImGui::SliderFloat("mTimeMod", &mTimeMod, 0.0f, 1.0f);
		ImGui::DragFloat("mSinkIntoGround", &mSinkIntoGround, 0.01f);
		ImGui::SliderFloat("mWalkingTime", &mWalkingTime, 0.00f, ANIM_TIME);

		if (ImGui::BeginCombo("Animation", mClips[mCurrentClip].name.c_str()))
		{
			for (std::size_t i = 0; i < mClips.size(); i += 1)
			{
				if (ImGui::Selectable(mClips[i].name.c_str(), i == mCurrentClip))
				{
					mCurrentClip = i;
				}
			}
			ImGui::EndCombo();
		}

		ImGui::Checkbox("mShowIKPose", &mShowIKPose);
		ImGui::Checkbox("mShowCurrentPose", &mShowCurrentPose);
		ImGui::Checkbox("mDepthTest", &mDepthTest);
		ImGui::Checkbox("mShowMesh", &mShowMesh);
		ImGui::Checkbox("mShowEnvironment", &mShowEnvironment);
	}

	ScalarTrack rightTrack;
	ScalarTrack leftTrack;
};

void GetScalarValues(
	std::vector<float>& outScalars, std::size_t inComponentCount, const cgltf_accessor& inAccessor
)
{
	outScalars.resize(inAccessor.count * inComponentCount);
	for (cgltf_size i = 0; i < inAccessor.count; ++i)
	{
		cgltf_accessor_read_float(
			&inAccessor, i, &outScalars[i * inComponentCount], inComponentCount
		);
	}
}

bool is_parent_of(const Pose& pose, std::size_t index, std::size_t root)
{
	if (root == index) return true;
	const auto& p = pose[index].parent;
	if (p)
		return is_parent_of(pose, *p, root);
	else
		return false;
}

void CopyPose(Pose& target, const Pose& base, const Pose& top, std::size_t root)
{
	if (base.size() != top.size()) return;
	target.resize(base.size());
	for (std::size_t i = 0; i < base.size(); i += 1)
	{
		const auto& p = is_parent_of(base, i, root) ? top : base;
		target[i] = p[i];
	}
}

void StaticMeshFromAttribute(Mesh& outMesh, const cgltf_attribute& attribute)
{
	const auto attribType = attribute.type;
	const auto& accessor = *attribute.data;

	std::size_t componentCount = 0;
	if (accessor.type == cgltf_type_vec2)
	{
		componentCount = 2;
	}
	else if (accessor.type == cgltf_type_vec3)
	{
		componentCount = 3;
	}
	else if (accessor.type == cgltf_type_vec4)
	{
		componentCount = 4;
	}
	else
	{
		std::cout << "Unknown data type\n";
		return;
	}

	std::vector<float> values;
	GetScalarValues(values, componentCount, accessor);
	const auto acessorCount = accessor.count;

	auto& positions = outMesh.position;
	auto& normals = outMesh.normal;
	auto& texCoords = outMesh.texcoord;

	for (std::size_t i = 0; i < acessorCount; ++i)
	{
		const auto index = i * componentCount;
		switch (attribType)
		{
		case cgltf_attribute_type_position:
			positions.emplace_back(vec3(values[index + 0], values[index + 1], values[index + 2]));
			break;
		case cgltf_attribute_type_normal:
			{
				auto normal = vec3(values[index + 0], values[index + 1], values[index + 2]);
				if (get_length_sq(normal) < 0.000001f)
				{
					normal = vec3(0, 1, 0);
				}
				normals.emplace_back(get_normalized(normal));
			}
			break;
		case cgltf_attribute_type_texcoord:
			texCoords.emplace_back(vec2(values[index + 0], values[index + 1]));
			break;
		default: break;
		}
	}
}

std::vector<Mesh> LoadStaticMeshes(cgltf_data* data)
{
	std::vector<Mesh> result;
	const auto* nodes = data->nodes;
	const auto nodeCount = data->nodes_count;

	for (std::size_t i = 0; i < nodeCount; ++i)
	{
		const auto* node = &nodes[i];
		if (node->mesh == nullptr)
		{
			continue;
		}

		const auto numPrims = node->mesh->primitives_count;
		for (std::size_t j = 0; j < numPrims; ++j)
		{
			result.emplace_back();
			auto& mesh = result[result.size() - 1];

			cgltf_primitive* primitive = &node->mesh->primitives[j];

			const auto numAttributes = primitive->attributes_count;
			for (std::size_t k = 0; k < numAttributes; ++k)
			{
				cgltf_attribute* attribute = &primitive->attributes[k];
				StaticMeshFromAttribute(mesh, *attribute);
			}

			if (primitive->indices != nullptr)
			{
				const auto indexCount = primitive->indices->count;
				auto& indices = mesh.indices;
				indices.resize(indexCount);

				for (std::size_t k = 0; k < indexCount; ++k)
				{
					indices[k]
						= static_cast<unsigned int>(cgltf_accessor_read_index(primitive->indices, k)
						);
				}
			}

			mesh.UpdateOpenGLBuffers();
		}
	}

	return result;
}

Frame<vec3> f(float time, float x, float z)
{
	const vec3 v = {x, 0, z};
	return {time, v, v, v};
}

Frame<float> f(float time, float f)
{
	return {time, f, f, f};
}

Sample::Sample()
	: mMotionTrack(
		{f(0.0f, 0, 1), f(1.0f, 0, 10), f(3.0f, 22, 10), f(4.0f, 22, 2), f(6.0f, 0, 1)},
		Interpolation::Linear
	)
	, rightTrack({f(0.0f, 1), f(0.3f, 0), f(0.7f, 0), f(1.0f, 1)}, Interpolation::Linear)
	, leftTrack({f(0.0f, 0), f(0.4f, 1), f(0.6f, 1), f(1.0f, 0)}, Interpolation::Linear)
{
	mLastModelY = 0.0f;

	cgltf_data* gltf = load_gltf_file(assets::woman_gltf());
	mMeshes = LoadMeshes(gltf);
	mSkeleton = LoadSkeleton(gltf);
	mClips = get_animation_clips(gltf);
	free_gltf_file(gltf);

	gltf = load_gltf_file(assets::ik_course_gltf());
	mIKCourse = LoadStaticMeshes(gltf);
	free_gltf_file(gltf);
	mCourseTexture = new Texture(assets::uv_texture());
	mTriangles = MeshesToTriangles(mIKCourse);

	mStaticShader = new Shader(assets::static_shader(), assets::lit_shader());
	mSkinnedShader = new Shader(assets::skinned_shader(), assets::lit_shader());
	mDiffuseTexture = new Texture(assets::woman_texture());

	mLeftLeg = new IKLeg(mSkeleton, "LeftUpLeg", "LeftLeg", "LeftFoot", "LeftToeBase");
	mLeftLeg->SetAnkleOffset(0.2f);
	mRightLeg = new IKLeg(mSkeleton, "RightUpLeg", "RightLeg", "RightFoot", "RightToeBase");
	mRightLeg->SetAnkleOffset(0.2f);

	mCurrentClip = 0;
	mCurrentPose = mSkeleton.rest_pose;

	mCurrentPoseVisual = new DebugDraw();
	mCurrentPoseVisual->FromPose(mCurrentPose);
	mCurrentPoseVisual->UpdateOpenGLBuffers();

	// For the UI
	for (std::size_t i = 0, size = mClips.size(); i < size; ++i)
	{
		std::string& clipName = mClips[i].name;
		if (clipName == "Walking")
		{
			mCurrentClip = i;
		}
	}

	mShowIKPose = false;
	mShowCurrentPose = false;

	mShowEnvironment = true;
	mShowMesh = true;
	mDepthTest = false;
	mTimeMod = 1.0f;
	mSinkIntoGround = 0.15f;

	// Start the character clamped to the ground. Move down a little bit so it's not perfectly up
	const auto groundRay = Ray(vec3(mModel.position.x, 11, mModel.position.z));
	const auto numTriangles = mTriangles.size();
	for (std::size_t i = 0; i < numTriangles; ++i)
	{
		vec3 hitPoint;
		if (RaycastTriangle(groundRay, mTriangles[i], hitPoint))
		{
			mModel.position = hitPoint;
			break;
		}
	}
	mModel.position.y -= mSinkIntoGround;
	mLastModelY = mModel.position.y;

	mToeLength = 0.3f;
}

void Sample::on_frame(float deltaTime)
{
	float rayHeight = 2.1f;
	const auto numTriangles = mTriangles.size();
	deltaTime = deltaTime * mTimeMod;
	vec3 hitPoint;

	// Increment time and sample the animation clip that moves the model on the level rails
	// The Y position is a lie, it's a trackt hat only makes sense from an ortho top view
	mWalkingTime += deltaTime * 0.3f;
	while (mWalkingTime > ANIM_TIME)
	{
		mWalkingTime -= ANIM_TIME;
	}

	// Figure out the X and Z position of the model in world spcae
	// Keep the Y position the same as last frame for both to properly orient the model
	float lastYPosition = mModel.position.y;
	vec3 currentPosition = mMotionTrack.get_sample(mWalkingTime, true);
	vec3 nextPosition = mMotionTrack.get_sample(mWalkingTime + 0.1f, true);
	currentPosition.y = lastYPosition;
	nextPosition.y = lastYPosition;
	mModel.position = currentPosition;

	// Figure out the forward direction of the model in world spcae
	vec3 direction = get_normalized(nextPosition - currentPosition);
	quat newDirection = quat_from_look_rotation(direction, vec3(0, 1, 0));
	if (dot(mModel.rotation, newDirection) < 0.0f)
	{
		newDirection = newDirection * -1.0f;
	}
	mModel.rotation = nlerp(mModel.rotation, newDirection, deltaTime * 10.0f);
	vec3 characterForward = mModel.rotation * vec3(0, 0, 1);

	// Figure out the Y position of the model in world spcae
	Ray groundRay(vec3(mModel.position.x, 11, mModel.position.z));
	for (std::size_t i = 0; i < numTriangles; ++i)
	{
		if (RaycastTriangle(groundRay, mTriangles[i], hitPoint))
		{
			// Sink the model a little bit into the ground to avoid hyper extending it's legs
			mModel.position = hitPoint - vec3(0, mSinkIntoGround, 0);
			break;
		}
	}

	// Sample the current animation, update the pose visual and figure out where the left
	// and right leg are in their up/down animation cycle
	mPlaybackTime = mClips[mCurrentClip].sample_to_pose(mCurrentPose, mPlaybackTime + deltaTime);
	mCurrentPoseVisual->FromPose(mCurrentPose);
	float normalizedTime = (mPlaybackTime - mClips[mCurrentClip].duration.start)
						 / mClips[mCurrentClip].GetDuration();
	if (normalizedTime < 0.0f)
	{
		std::cout << "should not be < 0\n";
		normalizedTime = 0.0f;
	}
	if (normalizedTime > 1.0f)
	{
		std::cout << "should not be < 0\n";
		normalizedTime = 1.0f;
	}
	float leftMotion = leftTrack.get_sample(normalizedTime, true);
	float rightMotion = rightTrack.get_sample(normalizedTime, true);

	// Construct a ray for the left ankle, store the world position and the predictive position
	// of the ankle. This is in case the raycasts below don't hit anything.
	vec3 worldLeftAnkle
		= get_combined(mModel, calc_global_transform(mCurrentPose, mLeftLeg->Ankle())).position;
	Ray leftAnkleRay(worldLeftAnkle + vec3(0, 2, 0));
	vec3 predictiveLeftAnkle = worldLeftAnkle;

	// Construct a ray for the right ankle, store the world position and the predictive position
	// of the ankle. This is in case the raycasts below don't hit anything.
	vec3 worldRightAnkle
		= get_combined(mModel, calc_global_transform(mCurrentPose, mRightLeg->Ankle())).position;
	Ray rightAnkleRay(worldRightAnkle + vec3(0, 2, 0));
	vec3 predictiveRightAnkle = worldRightAnkle;

	// Perform some raycasts for the feet, these are done in world space and
	// will define the IK based target points. For each ankle, we need to know
	// the current position (raycast from knee height to the sole of the foot height)
	// and the predictive position (infinate ray cast). The target point will be
	// between these two goals
	vec3 groundReference = mModel.position;
	for (std::size_t i = 0; i < numTriangles; ++i)
	{
		if (RaycastTriangle(leftAnkleRay, mTriangles[i], hitPoint))
		{
			if (get_length_sq(hitPoint - leftAnkleRay.origin) < rayHeight * rayHeight)
			{
				worldLeftAnkle = hitPoint;

				if (hitPoint.y < groundReference.y)
				{
					groundReference = hitPoint - vec3(0, mSinkIntoGround, 0);
				}
			}
			predictiveLeftAnkle = hitPoint;
		}
		if (RaycastTriangle(rightAnkleRay, mTriangles[i], hitPoint))
		{
			if (get_length_sq(hitPoint - rightAnkleRay.origin) < rayHeight * rayHeight)
			{
				worldRightAnkle = hitPoint;

				if (hitPoint.y < groundReference.y)
				{
					groundReference = hitPoint - vec3(0, mSinkIntoGround, 0);
				}
			}
			predictiveRightAnkle = hitPoint;
		}
	}

	// Lerp the Y position of the mode over a small period of time
	// Just to avoid poping
	mModel.position.y = mLastModelY;
	mModel.position = lerp(mModel.position, groundReference, deltaTime * 10.0f);
	mLastModelY = mModel.position.y;

	// Lerp between fully clamped to the ground, and somewhat clamped to the ground based on the
	// current phase of the walk cycle
	worldLeftAnkle = lerp(worldLeftAnkle, predictiveLeftAnkle, leftMotion);
	worldRightAnkle = lerp(worldRightAnkle, predictiveRightAnkle, rightMotion);

	// Now that we know the position of the model, as well as the ankle we can solve the feet.
	mLeftLeg->SolveForLeg(mModel, mCurrentPose, worldLeftAnkle /*, worldLeftToe*/);
	mRightLeg->SolveForLeg(mModel, mCurrentPose, worldRightAnkle /*, worldRightToe*/);
	// Apply the solved feet
	CopyPose(mCurrentPose, mCurrentPose, mLeftLeg->GetAdjustedPose(), mLeftLeg->Hip());
	CopyPose(mCurrentPose, mCurrentPose, mRightLeg->GetAdjustedPose(), mRightLeg->Hip());

	// The toes are still wrong, let's fix those. First, construct some rays for the toes
	Transform leftAnkleWorld
		= get_combined(mModel, calc_global_transform(mCurrentPose, mLeftLeg->Ankle()));
	Transform rightAnkleWorld
		= get_combined(mModel, calc_global_transform(mCurrentPose, mRightLeg->Ankle()));

	vec3 worldLeftToe
		= get_combined(mModel, calc_global_transform(mCurrentPose, mLeftLeg->Toe())).position;
	vec3 leftToeTarget = worldLeftToe;
	vec3 predictiveLeftToe = worldLeftToe;

	vec3 worldRightToe
		= get_combined(mModel, calc_global_transform(mCurrentPose, mRightLeg->Toe())).position;
	vec3 rightToeTarget = worldRightToe;
	vec3 predictiveRightToe = worldRightToe;

	vec3 origin = leftAnkleWorld.position;
	origin.y = worldLeftToe.y;
	Ray leftToeRay(origin + characterForward * mToeLength + vec3(0, 1, 0));
	origin = rightAnkleWorld.position;
	origin.y = worldRightToe.y;
	Ray rightToeRay = Ray(origin + characterForward * mToeLength + vec3(0, 1, 0));

	// Next, see if the toes hit anything
	float ankleRayHeight = 1.1f;
	for (std::size_t i = 0; i < numTriangles; ++i)
	{
		if (RaycastTriangle(leftToeRay, mTriangles[i], hitPoint))
		{
			if (get_length_sq(hitPoint - leftToeRay.origin) < ankleRayHeight * ankleRayHeight)
			{
				leftToeTarget = hitPoint;
			}
			predictiveLeftToe = hitPoint;
		}
		if (RaycastTriangle(rightToeRay, mTriangles[i], hitPoint))
		{
			if (get_length_sq(hitPoint - rightToeRay.origin) < ankleRayHeight * ankleRayHeight)
			{
				rightToeTarget = hitPoint;
			}
			predictiveRightToe = hitPoint;
		}
	}

	// Place the toe target at the right location
	leftToeTarget = lerp(leftToeTarget, predictiveLeftToe, leftMotion);
	rightToeTarget = lerp(rightToeTarget, predictiveRightToe, rightMotion);

	// If the left or right toe hit, adjust the ankle rotation approrpaiteley
	vec3 leftAnkleToCurrentToe = worldLeftToe - leftAnkleWorld.position;
	vec3 leftAnkleToDesiredToe = leftToeTarget - leftAnkleWorld.position;
	if (dot(leftAnkleToCurrentToe, leftAnkleToDesiredToe) > 0.00001f)
	{
		quat ankleRotator = quat_from_rotation(leftAnkleToCurrentToe, leftAnkleToDesiredToe);
		Transform ankleLocal = mCurrentPose[mLeftLeg->Ankle()].local;

		quat worldRotatedAnkle = leftAnkleWorld.rotation * ankleRotator;
		quat localRotatedAnkle = worldRotatedAnkle * get_inverse(leftAnkleWorld.rotation);

		ankleLocal.rotation = localRotatedAnkle * ankleLocal.rotation;
		mCurrentPose[mLeftLeg->Ankle()].local = ankleLocal;
	}

	vec3 rightAnkleToCurrentToe = worldRightToe - rightAnkleWorld.position;
	vec3 rightAnkleToDesiredToe = rightToeTarget - rightAnkleWorld.position;
	if (dot(rightAnkleToCurrentToe, rightAnkleToDesiredToe) > 0.00001f)
	{
		quat ankleRotator = quat_from_rotation(rightAnkleToCurrentToe, rightAnkleToDesiredToe);
		Transform ankleLocal = mCurrentPose[mRightLeg->Ankle()].local;

		quat worldRotatedAnkle = rightAnkleWorld.rotation * ankleRotator;
		quat localRotatedAnkle = worldRotatedAnkle * get_inverse(rightAnkleWorld.rotation);

		ankleLocal.rotation = localRotatedAnkle * ankleLocal.rotation;
		mCurrentPose[mRightLeg->Ankle()].local = ankleLocal;
	}

	// Update the matrix palette for skinning
	mPosePalette = calc_matrix_palette(mCurrentPose);
}

void Sample::on_render(float inAspectRatio)
{
	mat4 projection = mat4_from_perspective(60.0f, inAspectRatio, 0.01f, 1000.0f);
	mat4 view = mat4_from_look_at(
		vec3(mModel.position.x, 0, mModel.position.z) + vec3(0, 5, 10),
		vec3(mModel.position.x, 0, mModel.position.z) + vec3(0, 3, 0),
		vec3(0, 1, 0)
	);
	mat4 model = mat4_from_transform(mModel);
	mat4 mvp = projection * view * model;
	mat4 vp = projection * view;

	if (mShowMesh)
	{
		Shader* characterShader = mSkinnedShader;

		characterShader->bind();
		Uniform<mat4>::Set(characterShader->get_uniform("model"), model);
		Uniform<mat4>::Set(characterShader->get_uniform("view"), view);
		Uniform<mat4>::Set(characterShader->get_uniform("projection"), projection);
		Uniform<vec3>::Set(characterShader->get_uniform("light"), vec3(1, 1, 1));
		Uniform<mat4>::Set(characterShader->get_uniform("pose"), mPosePalette);
		Uniform<mat4>::Set(
			characterShader->get_uniform("invBindPose"), mSkeleton.inverse_bind_pose
		);

		mDiffuseTexture->bind(characterShader->get_uniform("tex0"), 0);
		for (std::size_t i = 0, size = mMeshes.size(); i < size; ++i)
		{
			mMeshes[i].Bind(
				static_cast<int>(characterShader->get_attribute("position")),
				static_cast<int>(characterShader->get_attribute("normal")),
				static_cast<int>(characterShader->get_attribute("texCoord")),
				static_cast<int>(characterShader->get_attribute("weights")),
				static_cast<int>(characterShader->get_attribute("joints"))
			);
			mMeshes[i].Draw();
			mMeshes[i].UnBind(
				static_cast<int>(characterShader->get_attribute("position")),
				static_cast<int>(characterShader->get_attribute("normal")),
				static_cast<int>(characterShader->get_attribute("texCoord")),
				static_cast<int>(characterShader->get_attribute("weights")),
				static_cast<int>(characterShader->get_attribute("joints"))
			);
		}
		mDiffuseTexture->unbind(0);
		characterShader->unbind();
	}

	if (mShowEnvironment)
	{
		Shader* environmentShader = mStaticShader;
		environmentShader->bind();
		Uniform<mat4>::Set(environmentShader->get_uniform("model"), mat4());
		Uniform<mat4>::Set(environmentShader->get_uniform("view"), view);
		Uniform<mat4>::Set(environmentShader->get_uniform("projection"), projection);
		Uniform<vec3>::Set(environmentShader->get_uniform("light"), vec3(1, 1, 1));
		mCourseTexture->bind(environmentShader->get_uniform("tex0"), 0);
		for (std::size_t i = 0, size = mIKCourse.size(); i < size; ++i)
		{
			mIKCourse[i].Bind(
				static_cast<int>(environmentShader->get_attribute("position")),
				static_cast<int>(environmentShader->get_attribute("normal")),
				static_cast<int>(environmentShader->get_attribute("texCoord")),
				-1,
				-1
			);
			mIKCourse[i].Draw();
			mIKCourse[i].UnBind(
				static_cast<int>(environmentShader->get_attribute("position")),
				static_cast<int>(environmentShader->get_attribute("normal")),
				static_cast<int>(environmentShader->get_attribute("texCoord")),
				-1,
				-1
			);
		}
		mCourseTexture->unbind(0);
		environmentShader->unbind();
	}

	if (! mDepthTest)
	{
		glDisable(GL_DEPTH_TEST);
	}

	if (mShowCurrentPose)
	{
		mCurrentPoseVisual->UpdateOpenGLBuffers();
		mCurrentPoseVisual->Draw(DebugDrawMode::Lines, vec3(0, 0, 1), mvp);
	}

	if (mShowIKPose)
	{
		mLeftLeg->Draw(vp, vec3(1, 0, 0));
		mRightLeg->Draw(vp, vec3(0, 1, 0));
	}

	if (! mDepthTest)
	{
		glEnable(GL_DEPTH_TEST);
	}
}

Sample::~Sample()
{
	delete mCurrentPoseVisual;
	delete mStaticShader;
	delete mDiffuseTexture;
	delete mSkinnedShader;
	delete mCourseTexture;
	delete mLeftLeg;
	delete mRightLeg;
	mClips.clear();
	mMeshes.clear();
}


IMPLEMENT_MAIN(Sample)
