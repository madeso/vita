#pragma once

#include <vector>
#include <optional>

#include "vita/anim/transform.h"

// why is parent part of the Pose/joint???

struct Joint
{
	std::optional<std::size_t> parent;
	Transform local;
};

using Pose = std::vector<Joint>;

Transform calc_global_transform(const Pose& pose, std::size_t index);

/// this seems what we used to called a compile pose
std::vector<mat4> calc_matrix_palette();
