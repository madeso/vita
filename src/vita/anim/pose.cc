#include "vita/anim/pose.h"

#include <cstring>

Transform calc_global_transform(const Pose& joints, std::size_t index)
{
	auto result = joints[index].local;

	for (auto parent = joints[index].parent; parent; parent = joints[*parent].parent)
	{
		result = get_combined(joints[*parent].local, result);
	}

	return result;
}

std::vector<mat4> calc_matrix_palette(const Pose& pose)
{
	const auto size = pose.size();

	std::vector<mat4> out;
	out.resize(size);

	for (std::size_t i = 0; i < size; ++i)
	{
		Transform t = calc_global_transform(pose, i);
		out[i] = mat4_from_transform(t);
	}

	return out;
}
