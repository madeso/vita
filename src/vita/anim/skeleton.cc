#include "vita/anim/skeleton.h"

Skeleton::Skeleton()
{
}

Skeleton::Skeleton(const Pose& rest, const Pose& bind, const std::vector<std::string>& names)
{
	Set(rest, bind, names);
}

void Skeleton::Set(const Pose& rest, const Pose& bind, const std::vector<std::string>& names)
{
	mRestPose = rest;
	mBindPose = bind;
	mJointNames = names;
	UpdateInverseBindPose();
}

void Skeleton::UpdateInverseBindPose()
{
	const auto size = mBindPose.size();
	mInvBindPose.resize(size);

	for (unsigned int i = 0; i < size; ++i)
	{
		Transform world = calc_global_transform(mBindPose, i);
		mInvBindPose[i] = get_inverse(mat4_from_transform(world));
	}
}
