#include "vita/anim/skeleton.h"

Skeleton::Skeleton()
{
}

Skeleton::Skeleton(const Pose& rest, const Pose& bind, const std::vector<std::string>& names)
	: rest_pose(rest)
	, bind_pose(bind)
	, joint_names(names)
{
	// UpdateInverseBindPose
	const auto size = bind_pose.size();
	inverse_bind_pose.resize(size);

	for (unsigned int i = 0; i < size; ++i)
	{
		Transform world = calc_global_transform(bind_pose, i);
		inverse_bind_pose[i] = get_inverse(mat4_from_transform(world));
	}
}
