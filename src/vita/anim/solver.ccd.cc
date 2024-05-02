#include "vita/anim/solver.ccd.h"

unsigned int CCDSolver::Size() const
{
	return static_cast<unsigned int>(mIKChain.size());
}

void CCDSolver::Resize(unsigned int newSize)
{
	mIKChain.resize(newSize);
}

Transform& CCDSolver::operator[](unsigned int index)
{
	return mIKChain[index];
}

unsigned int CCDSolver::GetNumSteps() const
{
	return mNumSteps;
}

void CCDSolver::SetNumSteps(unsigned int numSteps)
{
	mNumSteps = numSteps;
}

float CCDSolver::GetThreshold() const
{
	return mThreshold;
}

void CCDSolver::SetThreshold(float value)
{
	mThreshold = value;
}

Transform CCDSolver::GetGlobalTransform(unsigned int index) const
{
	Transform world = mIKChain[index];
	for (int i = static_cast<int>(index) - 1; i >= 0; --i)
	{
		world = get_combined(mIKChain[static_cast<std::size_t>(i)], world);
	}
	return world;
}

bool CCDSolver::Solve(const Transform& target)
{
	unsigned int size = Size();
	if (size == 0)
	{
		return false;
	}
	unsigned int last = size - 1;
	float thresholdSq = mThreshold * mThreshold;
	vec3 goal = target.position;
	for (unsigned int i = 0; i < mNumSteps; ++i)
	{
		vec3 effector = GetGlobalTransform(last).position;
		if (get_length_sq(goal - effector) < thresholdSq)
		{
			return true;
		}
		for (int j = static_cast<int>(size) - 2; j >= 0; --j)
		{
			effector = GetGlobalTransform(last).position;

			Transform world = GetGlobalTransform(static_cast<unsigned int>(j));
			vec3 position = world.position;
			quat rotation = world.rotation;

			vec3 toEffector = effector - position;
			vec3 toGoal = goal - position;
			quat effectorToGoal;
			if (get_length_sq(toGoal) > 0.00001f)
			{
				effectorToGoal = quat_from_rotation(toEffector, toGoal);
			}

			quat worldRotated = rotation * effectorToGoal;
			quat localRotate = worldRotated * get_inverse(rotation);
			mIKChain[static_cast<std::size_t>(j)].rotation
				= localRotate * mIKChain[static_cast<std::size_t>(j)].rotation;

			effector = GetGlobalTransform(last).position;
			if (get_length_sq(goal - effector) < thresholdSq)
			{
				return true;
			}
		}
	}

	return false;
}
