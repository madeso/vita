#include "vita/anim/ik.h"

Transform GetGlobalTransform(const std::vector<Transform>& mIKChain, unsigned int index)
{
	Transform world = mIKChain[index];
	for (int i = static_cast<int>(index) - 1; i >= 0; --i)
	{
		world = get_combined(mIKChain[static_cast<std::size_t>(i)], world);
	}
	return world;
}

std::vector<vec3> GetGlobalTransforms(const std::vector<Transform>& mIKChain)
{
	std::vector<vec3> ret;
	const auto size = mIKChain.size();
	ret.resize(size);

	for (std::size_t i = 0; i < size; ++i)
	{
		ret[i] = GetGlobalTransform(mIKChain, static_cast<unsigned int>(i)).position;
	}

	return ret;
}

// ------------------------------------------------------------------------------------------------

bool ik_ccd(
	std::vector<Transform>& mIKChain,
	const Transform& target,
	unsigned int mNumSteps,
	float mThreshold
)
{
	const auto size = static_cast<unsigned int>(mIKChain.size());
	if (size == 0)
	{
		return false;
	}
	const auto last = size - 1;
	float thresholdSq = mThreshold * mThreshold;
	vec3 goal = target.position;
	for (unsigned int i = 0; i < mNumSteps; ++i)
	{
		vec3 effector = GetGlobalTransform(mIKChain, last).position;
		if (get_length_sq(goal - effector) < thresholdSq)
		{
			return true;
		}

		for (int j = static_cast<int>(size) - 2; j >= 0; --j)
		{
			effector = GetGlobalTransform(mIKChain, last).position;

			Transform world = GetGlobalTransform(mIKChain, static_cast<unsigned int>(j));
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

			effector = GetGlobalTransform(mIKChain, last).position;
			if (get_length_sq(goal - effector) < thresholdSq)
			{
				return true;
			}
		}
	}

	return false;
}

std::vector<float> IKChainToWorld(
	const std::vector<Transform>& mIKChain, std::vector<vec3>& mWorldChain
)
{
	const auto size = mIKChain.size();
	std::vector<float> mLengths;
	mLengths.resize(size);

	for (unsigned int i = 0; i < size; ++i)
	{
		const auto world = GetGlobalTransform(mIKChain, i);
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

	return mLengths;
}

void WorldToIKChain(std::vector<Transform>& mIKChain, const std::vector<vec3>& mWorldChain)
{
	const auto size = mIKChain.size();
	if (size == 0)
	{
		return;
	}

	for (unsigned int i = 0; i < size - 1; ++i)
	{
		const auto world = GetGlobalTransform(mIKChain, i);
		const auto next = GetGlobalTransform(mIKChain, i + 1);
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

bool ik_fabrik(
	std::vector<Transform>& mIKChain,
	const Transform& target,
	unsigned int mNumSteps,
	float mThreshold
)
{
	const auto size = mIKChain.size();
	if (size == 0)
	{
		return false;
	}

	std::vector<vec3> mWorldChain;
	mWorldChain.resize(size);

	const auto last = size - 1;
	const auto thresholdSq = mThreshold * mThreshold;

	const auto mLengths = IKChainToWorld(mIKChain, mWorldChain);
	const auto goal = target.position;
	const auto base = mWorldChain[0];

	for (unsigned int step_index = 0; step_index < mNumSteps; ++step_index)
	{
		const auto effector = mWorldChain[last];
		if (get_length_sq(goal - effector) < thresholdSq)
		{
			WorldToIKChain(mIKChain, mWorldChain);
			return true;
		}

		// Iterate backward
		mWorldChain[size - 1] = goal;
		for (auto bak = static_cast<int>(size - 2); bak >= 0; --bak)
		{
			const auto direction = get_normalized(
				mWorldChain[static_cast<std::size_t>(bak)]
				- mWorldChain[static_cast<std::size_t>(bak + 1)]
			);
			const auto offset = direction * mLengths[static_cast<std::size_t>(bak + 1)];
			mWorldChain[static_cast<std::size_t>(bak)]
				= mWorldChain[static_cast<std::size_t>(bak + 1)] + offset;
		}

		// Iterate forward
		mWorldChain[0] = base;
		for (std::size_t fwd = 1; fwd < size; ++fwd)
		{
			const auto direction = get_normalized(mWorldChain[fwd] - mWorldChain[fwd - 1]);
			const auto offset = direction * mLengths[fwd];
			mWorldChain[fwd] = mWorldChain[fwd - 1] + offset;
		}
	}

	WorldToIKChain(mIKChain, mWorldChain);
	const auto effector = GetGlobalTransform(mIKChain, static_cast<unsigned int>(last)).position;
	if (get_length_sq(goal - effector) < thresholdSq)
	{
		return true;
	}

	return false;
}
