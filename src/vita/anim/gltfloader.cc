#include "vita/anim/gltfloader.h"

#include <iostream>
#include <cstring>

#include "vita/anim/transform.h"

namespace gltf_helpers
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
Track<T> track_from_channel(const cgltf_animation_channel& inChannel)
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

void MeshFromAttribute(
	Mesh& outMesh,
	cgltf_attribute& attribute,
	cgltf_skin* skin,
	cgltf_node* nodes,
	unsigned int nodeCount
)
{
	cgltf_attribute_type attribType = attribute.type;
	cgltf_accessor& accessor = *attribute.data;

	unsigned int componentCount = 0;
	if (accessor.type == cgltf_type_vec2)
	{
		componentCount = 2;
	}
	else if (accessor.type == cgltf_type_vec3)
	{
		componentCount = 3;
	}
	else if (accessor.type == cgltf_type_vec4)
	{
		componentCount = 4;
	}
	const auto values = scalar_values_from_accessor(componentCount, accessor);
	const auto acessorCount = accessor.count;

	for (std::size_t i = 0; i < acessorCount; ++i)
	{
		const auto index = i * componentCount;
		switch (attribType)
		{
		case cgltf_attribute_type_position:
			outMesh.position.push_back(vec3(values[index + 0], values[index + 1], values[index + 2])
			);
			break;
		case cgltf_attribute_type_texcoord:
			outMesh.texcoord.push_back(vec2(values[index + 0], values[index + 1]));
			break;
		case cgltf_attribute_type_weights:
			outMesh.weights.push_back(
				vec4(values[index + 0], values[index + 1], values[index + 2], values[index + 3])
			);
			break;
		case cgltf_attribute_type_normal:
			{
				vec3 normal = vec3(values[index + 0], values[index + 1], values[index + 2]);
				if (get_length_sq(normal) < 0.000001f)
				{
					normal = vec3(0, 1, 0);
				}
				outMesh.normal.push_back(get_normalized(normal));
			}
			break;
		case cgltf_attribute_type_joints:
			{
				// These indices are skin relative. This function has no information about the
				// skin that is being parsed. Add +0.5f to round, since we can't read ints
				ivec4 joints(
					static_cast<int>(values[index + 0] + 0.5f),
					static_cast<int>(values[index + 1] + 0.5f),
					static_cast<int>(values[index + 2] + 0.5f),
					static_cast<int>(values[index + 3] + 0.5f)
				);

				joints.x = static_cast<int>(std::max<std::size_t>(
					0, find_node_index(skin->joints[joints.x], nodes, nodeCount).value_or(0)
				));
				joints.y = static_cast<int>(std::max<std::size_t>(
					0, find_node_index(skin->joints[joints.y], nodes, nodeCount).value_or(0)
				));
				joints.z = static_cast<int>(std::max<std::size_t>(
					0, find_node_index(skin->joints[joints.z], nodes, nodeCount).value_or(0)
				));
				joints.w = static_cast<int>(std::max<std::size_t>(
					0, find_node_index(skin->joints[joints.w], nodes, nodeCount).value_or(0)
				));

				outMesh.influences.push_back(joints);
			}
			break;
		default:
			//ignore
			break;
		}
	}
}

}  //  namespace gltf_helpers

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

		result[i].local = gltf_helpers::local_transform_from_node(data->nodes[i]);
		result[i].parent = gltf_helpers::find_node_index(node->parent, data->nodes, boneCount);
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
		auto& clip = result[i];

		clip.name = data->animations[i].name;

		const auto numChannels = data->animations[i].channels_count;
		for (std::size_t j = 0; j < numChannels; ++j)
		{
			cgltf_animation_channel& channel = data->animations[i].channels[j];
			cgltf_node* target = channel.target_node;
			const auto nodeId = gltf_helpers::find_node_index(target, data->nodes, numNodes);
			if (! nodeId)
			{
				std::cerr << "Invalid node id\n";
				continue;
			}

			auto& track = clip[*nodeId];

			if (channel.target_path == cgltf_animation_path_type_translation)
			{
				track.position = gltf_helpers::track_from_channel<vec3, 3>(channel);
			}
			else if (channel.target_path == cgltf_animation_path_type_scale)
			{
				track.scale = gltf_helpers::track_from_channel<vec3, 3>(channel);
			}
			else if (channel.target_path == cgltf_animation_path_type_rotation)
			{
				track.rotation = gltf_helpers::track_from_channel<quat, 4>(channel);
			}
		}
		clip.duration = duration_from_tracks(clip.tracks);
	}

	return result;
}

Pose LoadBindPose(cgltf_data* data)
{
	Pose restPose = get_rest_pose(data);
	const auto numBones = restPose.size();
	std::vector<Transform> worldBindPose(numBones);
	for (std::size_t i = 0; i < numBones; ++i)
	{
		worldBindPose[i] = calc_global_transform(restPose, i);
	}
	const auto numSkins = data->skins_count;
	for (std::size_t i = 0; i < numSkins; ++i)
	{
		cgltf_skin* skin = &(data->skins[i]);
		std::vector<float> invBindAccessor
			= gltf_helpers::scalar_values_from_accessor(16, *skin->inverse_bind_matrices);

		const auto numJoints = skin->joints_count;
		for (std::size_t j = 0; j < numJoints; ++j)
		{
			// Read the ivnerse bind matrix of the joint
			float* matrix = &(invBindAccessor[j * 16]);
			mat4 invBindMatrix = mat4(matrix);

			// invert, convert to transform
			mat4 bindMatrix = get_inverse(invBindMatrix);
			Transform bindTransform = transform_from_mat4(bindMatrix);

			// Set that transform in the worldBindPose.
			cgltf_node* jointNode = skin->joints[j];
			auto jointIndex = gltf_helpers::find_node_index(jointNode, data->nodes, numBones);
			if (! jointIndex)
			{
				std::cerr << "warn: failed to find node\n";
				continue;
			}
			worldBindPose[*jointIndex] = bindTransform;
		}
	}

	// Convert the world bind pose to a regular bind pose
	Pose bindPose = restPose;
	for (std::size_t i = 0; i < numBones; ++i)
	{
		Transform current = worldBindPose[i];
		auto p = bindPose[i].parent;
		if (p)
		{
			// Bring into parent space
			Transform parent = worldBindPose[*p];
			current = get_combined(get_inverse(parent), current);
		}
		bindPose[i].local = current;
	}

	return bindPose;
}

Skeleton LoadSkeleton(cgltf_data* data)
{
	return Skeleton(get_rest_pose(data), LoadBindPose(data), get_joint_names(data));
}

std::vector<Mesh> LoadMeshes(cgltf_data* data)
{
	std::vector<Mesh> result;
	cgltf_node* nodes = data->nodes;
	const auto nodeCount = data->nodes_count;

	for (std::size_t i = 0; i < nodeCount; ++i)
	{
		cgltf_node* node = &nodes[i];
		if (node->mesh == nullptr || node->skin == nullptr)
		{
			continue;
		}
		const auto numPrims = node->mesh->primitives_count;
		for (std::size_t j = 0; j < numPrims; ++j)
		{
			result.push_back(Mesh());
			Mesh& mesh = result[result.size() - 1];

			cgltf_primitive* primitive = &node->mesh->primitives[j];

			const auto numAttributes = primitive->attributes_count;
			for (unsigned int k = 0; k < numAttributes; ++k)
			{
				cgltf_attribute* attribute = &primitive->attributes[k];
				gltf_helpers::MeshFromAttribute(
					mesh, *attribute, node->skin, nodes, static_cast<unsigned int>(nodeCount)
				);
			}
			if (primitive->indices != 0)
			{
				const auto indexCount = primitive->indices->count;
				mesh.indices.resize(indexCount);

				for (unsigned int k = 0; k < indexCount; ++k)
				{
					mesh.indices[k]
						= static_cast<unsigned int>(cgltf_accessor_read_index(primitive->indices, k)
						);
				}
			}
			mesh.UpdateOpenGLBuffers();
		}
	}

	return result;
}
