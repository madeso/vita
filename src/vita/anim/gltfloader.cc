#include "vita/anim/gltfloader.h"

#include <iostream>
#include <cstring>

#include "vita/anim/transform.h"

namespace GLTFHelpers
{
Transform local_transform_from_node(cgltf_node& node)
{
	Transform result;

	if (node.has_matrix)
	{
		mat4 mat(&node.matrix[0]);
		result = transform_from_mat4(mat);
	}

	if (node.has_translation)
	{
		result.position = vec3(node.translation[0], node.translation[1], node.translation[2]);
	}

	if (node.has_rotation)
	{
		result.rotation
			= quat(node.rotation[0], node.rotation[1], node.rotation[2], node.rotation[3]);
	}

	if (node.has_scale)
	{
		result.scale = vec3(node.scale[0], node.scale[1], node.scale[2]);
	}

	return result;
}

std::optional<std::size_t> find_node_index(
	cgltf_node* target, cgltf_node* allNodes, std::size_t numNodes
)
{
	if (target == 0)
	{
		return std::nullopt;
	}
	for (std::size_t i = 0; i < numNodes; ++i)
	{
		if (target == &allNodes[i])
		{
			return i;
		}
	}
	return std::nullopt;
}

std::vector<float> scalar_values_from_accessor(
	std::size_t component_count, const cgltf_accessor& accessor
)
{
	std::vector<float> outScalars;
	outScalars.resize(accessor.count * component_count);

	for (cgltf_size i = 0; i < accessor.count; ++i)
	{
		cgltf_accessor_read_float(&accessor, i, &outScalars[i * component_count], component_count);
	}
	return outScalars;
}

Interpolation interpolation_from_gltf(cgltf_interpolation_type x)
{
	if (x == cgltf_interpolation_type_linear)
	{
		return Interpolation::Linear;
	}
	else if (x == cgltf_interpolation_type_cubic_spline)
	{
		return Interpolation::Cubic;
	}
	else
	{
		return Interpolation::Constant;
	}
}

template<typename T, std::size_t N>
Track<T> TrackFromChannel(const cgltf_animation_channel& inChannel)
{
	cgltf_animation_sampler& sampler = *inChannel.sampler;


	const auto timelineFloats = scalar_values_from_accessor(1, *sampler.input);
	const auto valueFloats = scalar_values_from_accessor(N, *sampler.output);

	const auto numFrames = sampler.input->count;
	const auto numberOfValuesPerFrame = valueFloats.size() / timelineFloats.size();

	const auto interpolation = interpolation_from_gltf(sampler.interpolation);
	const auto isSamplerCubic = interpolation == Interpolation::Cubic;

	Track<T> ret{{}, interpolation};
	ret.frames.resize(numFrames);

	for (unsigned int i = 0; i < numFrames; ++i)
	{
		const auto baseIndex = i * numberOfValuesPerFrame;
		auto& frame = ret.frames[i];
		std::size_t offset = 0;

		frame.time = timelineFloats[i];

		for (std::size_t component = 0; component < N; ++component)
		{
			frame.in[component] = isSamplerCubic ? valueFloats[baseIndex + offset++] : 0.0f;
		}

		for (std::size_t component = 0; component < N; ++component)
		{
			frame.value[component] = valueFloats[baseIndex + offset++];
		}

		for (std::size_t component = 0; component < N; ++component)
		{
			frame.out[component] = isSamplerCubic ? valueFloats[baseIndex + offset++] : 0.0f;
		}
	}

	return ret;
}
}  //  namespace GLTFHelpers

cgltf_data* load_gltf_file(const GltfFile& path)
{
	cgltf_options options;
	std::memset(&options, 0, sizeof(cgltf_options));
	cgltf_data* data = NULL;

	// cgltf_result result = cgltf_parse_file(&options, path, &data);
	cgltf_result result = cgltf_parse(&options, path.data, path.size, &data);
	if (result != cgltf_result_success)
	{
		std::cerr << "Could not load input file: " << path.name << "\n";
		return nullptr;
	}
	result = cgltf_load_buffers(&options, data, path.name.c_str());
	if (result != cgltf_result_success)
	{
		cgltf_free(data);
		std::cerr << "Could not load buffers for: " << path.name << "\n";
		return nullptr;
	}
	result = cgltf_validate(data);
	if (result != cgltf_result_success)
	{
		cgltf_free(data);
		std::cerr << "GLTF file validation FAILED: " << path.name << "\n";
		return nullptr;
	}
	return data;
}

void free_gltf_file(cgltf_data* data)
{
	if (data == nullptr)
	{
		std::cerr << "WARNING: Can't free null data\n";
	}
	else
	{
		cgltf_free(data);
	}
}

Pose get_rest_pose(cgltf_data* data)
{
	const auto boneCount = data->nodes_count;
	Pose result(boneCount);

	for (std::size_t i = 0; i < boneCount; ++i)
	{
		cgltf_node* node = &(data->nodes[i]);

		Transform transform = GLTFHelpers::local_transform_from_node(data->nodes[i]);
		result.SetLocalTransform(i, transform);

		const auto parent = GLTFHelpers::find_node_index(node->parent, data->nodes, boneCount);
		result.SetParent(i, parent);
	}

	return result;
}

std::vector<std::string> get_joint_names(cgltf_data* data)
{
	const auto boneCount = data->nodes_count;
	std::vector<std::string> result(boneCount, "Not Set");

	for (std::size_t i = 0; i < boneCount; ++i)
	{
		cgltf_node* node = &(data->nodes[i]);

		if (node->name == 0)
		{
			result[i] = "EMPTY NODE";
		}
		else
		{
			result[i] = node->name;
		}
	}

	return result;
}

std::vector<Clip> get_animation_clips(cgltf_data* data)
{
	const auto numClips = data->animations_count;
	const auto numNodes = data->nodes_count;

	std::vector<Clip> result;
	result.resize(numClips);

	for (std::size_t i = 0; i < numClips; ++i)
	{
		result[i].mName = data->animations[i].name;

		const auto numChannels = data->animations[i].channels_count;
		for (std::size_t j = 0; j < numChannels; ++j)
		{
			cgltf_animation_channel& channel = data->animations[i].channels[j];
			cgltf_node* target = channel.target_node;
			const auto nodeId = GLTFHelpers::find_node_index(target, data->nodes, numNodes);
			if (! nodeId)
			{
				std::cerr << "Invalid node id\n";
				continue;
			}

			if (channel.target_path == cgltf_animation_path_type_translation)
			{
				result[i][*nodeId].position = GLTFHelpers::TrackFromChannel<vec3, 3>(channel);
			}
			else if (channel.target_path == cgltf_animation_path_type_scale)
			{
				result[i][*nodeId].scale = GLTFHelpers::TrackFromChannel<vec3, 3>(channel);
			}
			else if (channel.target_path == cgltf_animation_path_type_rotation)
			{
				result[i][*nodeId].rotation = GLTFHelpers::TrackFromChannel<quat, 4>(channel);
			}
		}
		result[i].RecalculateDuration();
	}

	return result;
}
