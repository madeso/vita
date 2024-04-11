#pragma once

#include <vector>
#include <string>

#include "cgltf.h"

#include "vita/anim/pose.h"
#include "vita/anim/clip.h"

struct GltfFile
{
	std::string name;
	const void* data;
	std::size_t size;
};

cgltf_data* load_gltf_file(const GltfFile& file);
void free_gltf_file(cgltf_data* handle);

Pose get_rest_pose(cgltf_data* data);
std::vector<std::string> get_joint_names(cgltf_data* data);
std::vector<Clip> get_animation_clips(cgltf_data* data);
