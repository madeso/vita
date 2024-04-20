#pragma once

#include <vector>
#include <string>

#include "vita/anim/mat4.h"
#include "vita/anim/pose.h"

struct Skeleton
{
	Pose rest_pose;
	Pose bind_pose;
	std::vector<mat4> inverse_bind_pose;
	std::vector<std::string> joint_names;

	Skeleton();
	Skeleton(const Pose& rest, const Pose& bind, const std::vector<std::string>& names);
};
