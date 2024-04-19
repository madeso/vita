#pragma once

#include <vector>
#include <string>

#include "cgltf.h"

#include "vita/anim/pose.h"
#include "vita/anim/clip.h"

#include "vita/anim/skeleton.h"
#include "vita/anim/mesh.h"

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

Pose LoadBindPose(cgltf_data* data);
Skeleton LoadSkeleton(cgltf_data* data);
std::vector<Mesh> LoadMeshes(cgltf_data* data);
