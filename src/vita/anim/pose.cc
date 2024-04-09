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
	mParents.resize(size);
	mJoints.resize(size);
}

std::size_t Pose::Size() const
{
	return mJoints.size();
}

Transform Pose::GetLocalTransform(std::size_t index) const
{
	return mJoints[index];
}

void Pose::SetLocalTransform(std::size_t index, const Transform& transform)
{
	mJoints[index] = transform;
}

Transform Pose::GetGlobalTransform(std::size_t index) const
{
	auto result = mJoints[index];

	for (auto parent = mParents[index]; parent; parent = mParents[*parent])
	{
		result = get_combined(mJoints[*parent], result);
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
	return mParents[index];
}

void Pose::SetParent(std::size_t index, std::optional<std::size_t> parent)
{
	mParents[index] = parent;
}

/*
bool Pose::operator==(const Pose& other)
{
	if (mJoints.size() != other.mJoints.size())
	{
		return false;
	}
	if (mParents.size() != other.mParents.size())
	{
		return false;
	}
	unsigned int size = (unsigned int) mJoints.size();
	for (unsigned int i = 0; i < size; ++i)
	{
		Transform thisLocal = mJoints[i];
		Transform otherLocal = other.mJoints[i];

		int thisParent = mParents[i];
		int otherParent = other.mParents[i];

		if (thisParent != otherParent)
		{
			return false;
		}

		if (thisLocal != otherLocal)
		{
			return false;
		}
	}
	return true;
}

bool Pose::operator!=(const Pose& other)
{
	return ! (*this == other);
}
*/
