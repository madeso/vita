#pragma once

#include <vector>
#include <optional>

#include "vita/anim/transform.h"

struct Pose
{
	std::vector<std::optional<std::size_t>> mParents;
	std::vector<Transform> mJoints;

	Pose();
	Pose(const Pose& p) = default;
	Pose(std::size_t numJoints);
	Pose& operator=(const Pose& p) = default;

	void Resize(std::size_t size);
	std::size_t Size() const;

	Transform GetLocalTransform(std::size_t index) const;
	void SetLocalTransform(std::size_t index, const Transform& transform);

	Transform GetGlobalTransform(std::size_t index) const;
	Transform operator[](std::size_t index) const;
	void GetMatrixPalette(std::vector<mat4>& out);

	std::optional<std::size_t> GetParent(std::size_t index) const;
	void SetParent(std::size_t index, std::optional<std::size_t> parent);

	// bool operator==(const Pose& other);
	// bool operator!=(const Pose& other);
};
