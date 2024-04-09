#pragma once

#include <vector>
#include <string>

#include "cgltf.h"

#include "vita/anim/pose.h"
#include "vita/anim/clip.h"

cgltf_data* LoadGLTFFile(const char* path);
void FreeGLTFFile(cgltf_data* handle);

Pose LoadRestPose(cgltf_data* data);
std::vector<std::string> LoadJointNames(cgltf_data* data);
std::vector<Clip> LoadAnimationClips(cgltf_data* data);
