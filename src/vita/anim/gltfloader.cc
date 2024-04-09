#include "vita/anim/gltfloader.h"

#include <iostream>
#include <cstring>

#include "vita/anim/transform.h"

namespace GLTFHelpers
{
Transform GetLocalTransform(cgltf_node& node)
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

std::optional<std::size_t> GetNodeIndex(
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

std::vector<float> GetScalarValues(std::size_t inComponentCount, const cgltf_accessor& inAccessor)
{
	std::vector<float> outScalars;
	outScalars.resize(inAccessor.count * inComponentCount);

	for (cgltf_size i = 0; i < inAccessor.count; ++i)
	{
		cgltf_accessor_read_float(
			&inAccessor, i, &outScalars[i * inComponentCount], inComponentCount
		);
	}
	return outScalars;
}

template<typename T, std::size_t N>
void TrackFromChannel(Track<T>& inOutTrack, const cgltf_animation_channel& inChannel)
{
	cgltf_animation_sampler& sampler = *inChannel.sampler;

	Interpolation interpolation = Interpolation::Constant;
	if (inChannel.sampler->interpolation == cgltf_interpolation_type_linear)
	{
		interpolation = Interpolation::Linear;
	}
	else if (inChannel.sampler->interpolation == cgltf_interpolation_type_cubic_spline)
	{
		interpolation = Interpolation::Cubic;
	}
	const auto isSamplerCubic = interpolation == Interpolation::Cubic;
	inOutTrack.interpolation = interpolation;

	const auto timelineFloats = GetScalarValues(1, *sampler.input);
	const auto valueFloats = GetScalarValues(N, *sampler.output);

	const auto numFrames = sampler.input->count;
	const auto numberOfValuesPerFrame = valueFloats.size() / timelineFloats.size();
	inOutTrack.frames.resize(numFrames);
	for (unsigned int i = 0; i < numFrames; ++i)
	{
		const auto baseIndex = i * numberOfValuesPerFrame;
		auto& frame = inOutTrack.frames[i];
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
}
}  //  namespace GLTFHelpers

cgltf_data* LoadGLTFFile(const char* path)
{
	cgltf_options options;
	std::memset(&options, 0, sizeof(cgltf_options));
	cgltf_data* data = NULL;
	cgltf_result result = cgltf_parse_file(&options, path, &data);
	if (result != cgltf_result_success)
	{
		std::cerr << "Could not load input file: " << path << "\n";
		return nullptr;
	}
	result = cgltf_load_buffers(&options, data, path);
	if (result != cgltf_result_success)
	{
		cgltf_free(data);
		std::cerr << "Could not load buffers for: " << path << "\n";
		return nullptr;
	}
	result = cgltf_validate(data);
	if (result != cgltf_result_success)
	{
		cgltf_free(data);
		std::cerr << "Invalid gltf file: " << path << "\n";
		return nullptr;
	}
	return data;
}

void FreeGLTFFile(cgltf_data* data)
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

Pose LoadRestPose(cgltf_data* data)
{
	const auto boneCount = data->nodes_count;
	Pose result(boneCount);

	for (std::size_t i = 0; i < boneCount; ++i)
	{
		cgltf_node* node = &(data->nodes[i]);

		Transform transform = GLTFHelpers::GetLocalTransform(data->nodes[i]);
		result.SetLocalTransform(i, transform);

		const auto parent = GLTFHelpers::GetNodeIndex(node->parent, data->nodes, boneCount);
		result.SetParent(i, parent);
	}

	return result;
}

std::vector<std::string> LoadJointNames(cgltf_data* data)
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

std::vector<Clip> LoadAnimationClips(cgltf_data* data)
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
			const auto nodeId = GLTFHelpers::GetNodeIndex(target, data->nodes, numNodes);
			if (! nodeId)
			{
				std::cerr << "Invalid node id\n";
				continue;
			}

			if (channel.target_path == cgltf_animation_path_type_translation)
			{
				auto& track = result[i][*nodeId].position;
				GLTFHelpers::TrackFromChannel<vec3, 3>(track, channel);
			}
			else if (channel.target_path == cgltf_animation_path_type_scale)
			{
				auto& track = result[i][*nodeId].scale;
				GLTFHelpers::TrackFromChannel<vec3, 3>(track, channel);
			}
			else if (channel.target_path == cgltf_animation_path_type_rotation)
			{
				auto& track = result[i][*nodeId].rotation;
				GLTFHelpers::TrackFromChannel<quat, 4>(track, channel);
			}
		}
		result[i].RecalculateDuration();
	}

	return result;
}
