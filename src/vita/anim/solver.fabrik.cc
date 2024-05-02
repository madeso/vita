#include "vita/anim/solver.fabrik.h"

unsigned int FABRIKSolver::GetNumSteps() const
{
	return mNumSteps;
}

void FABRIKSolver::SetNumSteps(unsigned int numSteps)
{
	mNumSteps = numSteps;
}

float FABRIKSolver::GetThreshold() const
{
	return mThreshold;
}

void FABRIKSolver::SetThreshold(float value)
{
	mThreshold = value;
}

unsigned int FABRIKSolver::Size() const
{
	return static_cast<unsigned int>(mIKChain.size());
}

void FABRIKSolver::Resize(unsigned int newSize)
{
	mIKChain.resize(newSize);
	mWorldChain.resize(newSize);
	mLengths.resize(newSize);
}

Transform FABRIKSolver::GetLocalTransform(unsigned int index) const
{
	return mIKChain[index];
}

void FABRIKSolver::SetLocalTransform(unsigned int index, const Transform& t)
{
	mIKChain[index] = t;
}

Transform FABRIKSolver::GetGlobalTransform(unsigned int index) const
{
	Transform world = mIKChain[index];
	for (int i = static_cast<int>(index) - 1; i >= 0; --i)
	{
		world = get_combined(mIKChain[static_cast<std::size_t>(i)], world);
	}
	return world;
}

void FABRIKSolver::IKChainToWorld()
{
	const auto size = Size();
	for (unsigned int i = 0; i < size; ++i)
	{
		const auto world = GetGlobalTransform(i);
		mWorldChain[i] = world.position;

		if (i >= 1)
		{
			vec3 prev = mWorldChain[i - 1];
			mLengths[i] = get_length(world.position - prev);
		}
	}

	if (size > 0)
	{
		mLengths[0] = 0.0f;
	}
}

void FABRIKSolver::WorldToIKChain()
{
	const auto size = Size();
	if (size == 0)
	{
		return;
	}

	for (unsigned int i = 0; i < size - 1; ++i)
	{
		const auto world = GetGlobalTransform(i);
		const auto next = GetGlobalTransform(i + 1);
		const auto position = world.position;
		const auto rotation = world.rotation;

		vec3 toNext = next.position - position;
		toNext = get_inverse(rotation) * toNext;

		vec3 toDesired = mWorldChain[i + 1] - position;
		toDesired = get_inverse(rotation) * toDesired;

		quat delta = quat_from_rotation(toNext, toDesired);
		mIKChain[i].rotation = delta * mIKChain[i].rotation;
	}
}

void FABRIKSolver::IterateBackward(const vec3& goal)
{
	const auto size = static_cast<int>(Size());
	if (size > 0)
	{
		mWorldChain[static_cast<std::size_t>(size - 1)] = goal;
	}

	for (int i = size - 2; i >= 0; --i)
	{
		const auto direction = get_normalized(
			mWorldChain[static_cast<std::size_t>(i)] - mWorldChain[static_cast<std::size_t>(i + 1)]
		);
		const auto offset = direction * mLengths[static_cast<std::size_t>(i + 1)];
		mWorldChain[static_cast<std::size_t>(i)]
			= mWorldChain[static_cast<std::size_t>(i + 1)] + offset;
	}
}

void FABRIKSolver::IterateForward(const vec3& base)
{
	const auto size = Size();
	if (size > 0)
	{
		mWorldChain[0] = base;
	}

	for (unsigned int i = 1; i < size; ++i)
	{
		const auto direction = get_normalized(mWorldChain[i] - mWorldChain[i - 1]);
		const auto offset = direction * mLengths[i];
		mWorldChain[i] = mWorldChain[i - 1] + offset;
	}
}

bool FABRIKSolver::Solve(const Transform& target)
{
	const auto size = Size();
	if (size == 0)
	{
		return false;
	}
	const auto last = size - 1;
	const auto thresholdSq = mThreshold * mThreshold;

	IKChainToWorld();
	const auto goal = target.position;
	const auto base = mWorldChain[0];

	for (unsigned int i = 0; i < mNumSteps; ++i)
	{
		const auto effector = mWorldChain[last];
		if (get_length_sq(goal - effector) < thresholdSq)
		{
			WorldToIKChain();
			return true;
		}

		IterateBackward(goal);
		IterateForward(base);
	}

	WorldToIKChain();
	const auto effector = GetGlobalTransform(last).position;
	if (get_length_sq(goal - effector) < thresholdSq)
	{
		return true;
	}

	return false;
}
