#pragma once

#include <vector>

#include "vita/anim/transform.h"

struct CCDSolver
{
	std::vector<Transform> mIKChain;

	unsigned int mNumSteps = 15;
	float mThreshold = 0.00001f;

	CCDSolver() = default;

	unsigned int Size() const;
	void Resize(unsigned int newSize);

	Transform& operator[](unsigned int index);
	Transform GetGlobalTransform(unsigned int index) const;

	unsigned int GetNumSteps() const;
	void SetNumSteps(unsigned int numSteps);

	float GetThreshold() const;
	void SetThreshold(float value);
	bool Solve(const Transform& target);
};
