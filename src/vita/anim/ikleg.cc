#include "vita/anim/ikleg.h"

IKLeg::IKLeg(
	Skeleton& skeleton,
	const std::string& hip,
	const std::string& knee,
	const std::string& ankle,
	const std::string& toe
)
{
	mLineVisuals = new DebugDraw();
	mPointVisuals = new DebugDraw();

	mPointVisuals->Resize(3);
	mLineVisuals->Resize(4);

	mAnkleToGroundOffset = 0.0f;

	mHipIndex = mKneeIndex = mAnkleIndex = mToeIndex = 0;
	for (std::size_t i = 0, size = skeleton.rest_pose.size(); i < size; ++i)
	{
		std::string& jointName = skeleton.joint_names[i];
		if (jointName == hip)
		{
			mHipIndex = i;
		}
		else if (jointName == knee)
		{
			mKneeIndex = i;
		}
		else if (jointName == ankle)
		{
			mAnkleIndex = i;
		}
		else if (jointName == toe)
		{
			mToeIndex = i;
		}
	}
}

IKLeg::IKLeg()
{
	mAnkleToGroundOffset = 0.0f;
	mLineVisuals = new DebugDraw();
	mPointVisuals = new DebugDraw();

	mPointVisuals->Resize(3);
	mLineVisuals->Resize(4);
}

IKLeg::IKLeg(const IKLeg& other)
{
	mLineVisuals = new DebugDraw();
	mPointVisuals = new DebugDraw();

	mAnkleToGroundOffset = 0.0f;
	mPointVisuals->Resize(3);
	mLineVisuals->Resize(4);

	*this = other;
}

IKLeg& IKLeg::operator=(const IKLeg& other)
{
	if (this == &other)
	{
		return *this;
	}

	mAnkleToGroundOffset = other.mAnkleToGroundOffset;
	mHipIndex = other.mHipIndex;
	mKneeIndex = other.mKneeIndex;
	mAnkleIndex = other.mAnkleIndex;
	mToeIndex = other.mToeIndex;

	return *this;
}

IKLeg::~IKLeg()
{
	delete mLineVisuals;
	delete mPointVisuals;
}

void IKLeg::SolveForLeg(
	IkFunction ik_solver, const Transform& model, Pose& pose, const vec3& ankleTargetPosition
)
{
	auto local_transforms = std::vector<Transform>{
		get_combined(model, calc_global_transform(pose, mHipIndex)),
		pose[mKneeIndex].local,
		pose[mAnkleIndex].local
	};
	mIKPose = pose;

	Transform target(
		ankleTargetPosition + vec3(0, 1, 0) * mAnkleToGroundOffset, quat(), vec3(1, 1, 1)
	);
	ik_solver(local_transforms, target, DEFAULT_NUM_STEPS, DEFAULT_THRESHOLD);

	Transform rootWorld = get_combined(model, calc_global_transform(pose, *pose[mHipIndex].parent));
	mIKPose[mHipIndex].local = get_combined(get_inverse(rootWorld), local_transforms[0]);
	mIKPose[mKneeIndex].local = local_transforms[1];
	mIKPose[mAnkleIndex].local = local_transforms[2];

	const auto global_transforms = GetGlobalTransforms(local_transforms);
	mLineVisuals->LinesFromIKSolver(global_transforms);
	mPointVisuals->PointsFromIKSolver(global_transforms);
}

Pose& IKLeg::GetAdjustedPose()
{
	return mIKPose;
}

std::size_t IKLeg::Hip()
{
	return mHipIndex;
}

std::size_t IKLeg::Knee()
{
	return mKneeIndex;
}

std::size_t IKLeg::Ankle()
{
	return mAnkleIndex;
}

std::size_t IKLeg::Toe()
{
	return mToeIndex;
}

void IKLeg::Draw(const mat4& mv, const vec3& legColor)
{
	mLineVisuals->UpdateOpenGLBuffers();
	mPointVisuals->UpdateOpenGLBuffers();
	mLineVisuals->Draw(DebugDrawMode::Lines, legColor, mv);
	mPointVisuals->Draw(DebugDrawMode::Points, legColor, mv);
}

void IKLeg::SetAnkleOffset(float offset)
{
	mAnkleToGroundOffset = offset;
}
