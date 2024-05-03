#pragma once

#include "vita/anim/ik.h"
#include "vita/anim/debugdraw.h"
#include "vita/anim/skeleton.h"
#include "vita/anim/track.h"

struct IKLeg
{
	Pose mIKPose;

	std::size_t mHipIndex;
	std::size_t mKneeIndex;
	std::size_t mAnkleIndex;
	std::size_t mToeIndex;

	DebugDraw* mLineVisuals;
	DebugDraw* mPointVisuals;

	// The ankle is not flat on the ground. We have to solve taking this into account!
	float mAnkleToGroundOffset;

	IKLeg(
		Skeleton& skeleton,
		const std::string& hip,
		const std::string& knee,
		const std::string& ankle,
		const std::string& toe
	);

	IKLeg();
	IKLeg(const IKLeg&);
	IKLeg& operator=(const IKLeg&);
	~IKLeg();

	void SolveForLeg(
		IkFunction ik_solver, const Transform& model, Pose& pose, const vec3& ankleTargetPosition
	);

	Pose& GetAdjustedPose();

	void Draw(const mat4& vp, const vec3& legColor);

	std::size_t Hip();
	std::size_t Knee();
	std::size_t Ankle();
	std::size_t Toe();

	void SetAnkleOffset(float offset);
};
