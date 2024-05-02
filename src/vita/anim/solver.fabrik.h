#pragma once

#include <vector>

#include "vita/anim/transform.h"

struct FABRIKSolver
{
	std::vector<Transform> mIKChain;
	unsigned int mNumSteps = 15;
	float mThreshold = 0.00001f;

	std::vector<vec3> mWorldChain;
	std::vector<float> mLengths;

	// hidden functions
	void IKChainToWorld();
	void IterateForward(const vec3& goal);
	void IterateBackward(const vec3& base);
	void WorldToIKChain();

	FABRIKSolver() = default;

	unsigned int Size() const;
	void Resize(unsigned int newSize);

	Transform GetLocalTransform(unsigned int index) const;
	void SetLocalTransform(unsigned int index, const Transform& t);
	Transform GetGlobalTransform(unsigned int index) const;

	unsigned int GetNumSteps() const;
	void SetNumSteps(unsigned int numSteps);

	float GetThreshold() const;
	void SetThreshold(float value);

	bool Solve(const Transform& target);
};
