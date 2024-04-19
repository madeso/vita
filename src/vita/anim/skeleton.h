#pragma once

#include <vector>
#include <string>

#include "vita/anim/mat4.h"
#include "vita/anim/pose.h"

struct Skeleton
{
	Pose mRestPose;
	Pose mBindPose;
	std::vector<mat4> mInvBindPose;
	std::vector<std::string> mJointNames;

	void UpdateInverseBindPose();

	Skeleton();
	Skeleton(const Pose& rest, const Pose& bind, const std::vector<std::string>& names);

	void Set(const Pose& rest, const Pose& bind, const std::vector<std::string>& names);
};
