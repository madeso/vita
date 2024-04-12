#include "vita/anim/pose.h"

#include <cstring>

Pose::Pose()
{
}

Pose::Pose(std::size_t numJoints)
{
	Resize(numJoints);
}

void Pose::Resize(std::size_t size)
{
	parents.resize(size);
	joints.resize(size);
}

std::size_t Pose::Size() const
{
	return joints.size();
}

Transform Pose::GetLocalTransform(std::size_t index) const
{
	return joints[index];
}

void Pose::SetLocalTransform(std::size_t index, const Transform& transform)
{
	joints[index] = transform;
}

Transform Pose::GetGlobalTransform(std::size_t index) const
{
	auto result = joints[index];

	for (auto parent = parents[index]; parent; parent = parents[*parent])
	{
		result = get_combined(joints[*parent], result);
	}

	return result;
}

Transform Pose::operator[](std::size_t index) const
{
	return GetGlobalTransform(index);
}

void Pose::GetMatrixPalette(std::vector<mat4>& out)
{
	const auto size = Size();
	if (out.size() != size)
	{
		out.resize(size);
	}

	for (std::size_t i = 0; i < size; ++i)
	{
		Transform t = GetGlobalTransform(i);
		out[i] = mat4_from_transform(t);
	}
}

std::optional<std::size_t> Pose::GetParent(std::size_t index) const
{
	return parents[index];
}

void Pose::SetParent(std::size_t index, std::optional<std::size_t> parent)
{
	parents[index] = parent;
}
