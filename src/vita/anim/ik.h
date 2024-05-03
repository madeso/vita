#pragma once

#include <vector>

#include "vita/anim/transform.h"

constexpr unsigned int DEFAULT_NUM_STEPS = 15;
constexpr float DEFAULT_THRESHOLD = 0.00001f;

using IkFunction = bool (*)(
	std::vector<Transform>& mIKChain,
	const Transform& target,
	unsigned int mNumSteps,
	float mThreshold
);

bool ik_ccd(
	std::vector<Transform>& mIKChain,
	const Transform& target,
	unsigned int mNumSteps,
	float mThreshold
);

bool ik_fabrik(
	std::vector<Transform>& mIKChain,
	const Transform& target,
	unsigned int mNumSteps,
	float mThreshold
);

std::vector<vec3> GetGlobalTransforms(const std::vector<Transform>& mIKChain);
