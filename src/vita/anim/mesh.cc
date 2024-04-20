#include "vita/anim/mesh.h"

#include "vita/anim/draw.h"
#include "vita/anim/transform.h"

Mesh::Mesh()
{
	attribute_position = new Attribute<vec3>();
	attribute_normal = new Attribute<vec3>();
	attribute_textcoord = new Attribute<vec2>();
	attribute_weights = new Attribute<vec4>();
	attribute_influences = new Attribute<ivec4>();
	index_buffer = new IndexBuffer();
}

Mesh::Mesh(const Mesh& other)
{
	attribute_position = new Attribute<vec3>();
	attribute_normal = new Attribute<vec3>();
	attribute_textcoord = new Attribute<vec2>();
	attribute_weights = new Attribute<vec4>();
	attribute_influences = new Attribute<ivec4>();
	index_buffer = new IndexBuffer();
	*this = other;
}

Mesh& Mesh::operator=(const Mesh& other)
{
	if (this == &other)
	{
		return *this;
	}
	position = other.position;
	normal = other.normal;
	texcoord = other.texcoord;
	weights = other.weights;
	influences = other.influences;
	indices = other.indices;
	UpdateOpenGLBuffers();
	return *this;
}

Mesh::~Mesh()
{
	delete attribute_position;
	delete attribute_normal;
	delete attribute_textcoord;
	delete attribute_weights;
	delete attribute_influences;
	delete index_buffer;
}

void Mesh::UpdateOpenGLBuffers()
{
	if (position.size() > 0)
	{
		attribute_position->set(position);
	}
	if (normal.size() > 0)
	{
		attribute_normal->set(normal);
	}
	if (texcoord.size() > 0)
	{
		attribute_textcoord->set(texcoord);
	}
	if (weights.size() > 0)
	{
		attribute_weights->set(weights);
	}
	if (influences.size() > 0)
	{
		attribute_influences->set(influences);
	}
	if (indices.size() > 0)
	{
		index_buffer->set(indices);
	}
}

void Mesh::Bind(
	int position_slot, int normal_slot, int texcoord_slot, int weights_slot, int influcences_slot
)
{
	if (position_slot >= 0)
	{
		attribute_position->bind_to(static_cast<unsigned int>(position_slot));
	}
	if (normal_slot >= 0)
	{
		attribute_normal->bind_to(static_cast<unsigned int>(normal_slot));
	}
	if (texcoord_slot >= 0)
	{
		attribute_textcoord->bind_to(static_cast<unsigned int>(texcoord_slot));
	}
	if (weights_slot >= 0)
	{
		attribute_weights->bind_to(static_cast<unsigned int>(weights_slot));
	}
	if (influcences_slot >= 0)
	{
		attribute_influences->bind_to(static_cast<unsigned int>(influcences_slot));
	}
}

void Mesh::Draw()
{
	if (indices.size() > 0)
	{
		::draw(*index_buffer, DrawMode::Triangles);
	}
	else
	{
		::draw(static_cast<unsigned int>(position.size()), DrawMode::Triangles);
	}
}

void Mesh::DrawInstanced(unsigned int numInstances)
{
	if (indices.size() > 0)
	{
		::draw_instanced(*index_buffer, DrawMode::Triangles, numInstances);
	}
	else
	{
		::draw_instanced(
			static_cast<unsigned int>(position.size()), DrawMode::Triangles, numInstances
		);
	}
}

void Mesh::UnBind(
	int position_slot, int normal_slot, int texcoord_slot, int weights_slot, int influcences_slot
)
{
	if (position_slot >= 0)
	{
		attribute_position->unbind_from(static_cast<unsigned int>(position_slot));
	}
	if (normal_slot >= 0)
	{
		attribute_normal->unbind_from(static_cast<unsigned int>(normal_slot));
	}
	if (texcoord_slot >= 0)
	{
		attribute_textcoord->unbind_from(static_cast<unsigned int>(texcoord_slot));
	}
	if (weights_slot >= 0)
	{
		attribute_weights->unbind_from(static_cast<unsigned int>(weights_slot));
	}
	if (influcences_slot >= 0)
	{
		attribute_influences->unbind_from(static_cast<unsigned int>(influcences_slot));
	}
}

#if 1
void Mesh::CPUSkin(Skeleton& skeleton, Pose& pose)
{
	const auto numVerts = position.size();
	if (numVerts == 0)
	{
		return;
	}

	skinned_position.resize(numVerts);
	skinned_normal.resize(numVerts);

	pose_palette = calc_matrix_palette(pose);
	std::vector<mat4> invPosePalette = skeleton.inverse_bind_pose;

	for (std::size_t i = 0; i < numVerts; ++i)
	{
		const auto& j = influences[i];
		const auto& w = weights[i];

		mat4 m0 = (pose_palette[static_cast<std::size_t>(j.x)]
				   * invPosePalette[static_cast<std::size_t>(j.x)])
				* w.x;
		mat4 m1 = (pose_palette[static_cast<std::size_t>(j.y)]
				   * invPosePalette[static_cast<std::size_t>(j.y)])
				* w.y;
		mat4 m2 = (pose_palette[static_cast<std::size_t>(j.z)]
				   * invPosePalette[static_cast<std::size_t>(j.z)])
				* w.z;
		mat4 m3 = (pose_palette[static_cast<std::size_t>(j.w)]
				   * invPosePalette[static_cast<std::size_t>(j.w)])
				* w.w;

		mat4 skin = m0 + m1 + m2 + m3;

		skinned_position[i] = get_transformed_point(skin, position[i]);
		skinned_normal[i] = get_transformed_vector(skin, normal[i]);
	}

	attribute_position->set(skinned_position);
	attribute_normal->set(skinned_normal);
}
#else
void Mesh::CPUSkin(Skeleton& skeleton, Pose& pose)
{
	unsigned int numVerts = (unsigned int) position.size();
	if (numVerts == 0)
	{
		return;
	}

	skinned_position.resize(numVerts);
	skinned_normal.resize(numVerts);
	Pose& bindPose = skeleton.GetBindPose();

	for (unsigned int i = 0; i < numVerts; ++i)
	{
		ivec4& joint = influences[i];
		vec4& weight = weights[i];

		Transform skin0 = combine(pose[joint.x], inverse(bindPose[joint.x]));
		vec3 p0 = transformPoint(skin0, position[i]);
		vec3 n0 = transformVector(skin0, normal[i]);

		Transform skin1 = combine(pose[joint.y], inverse(bindPose[joint.y]));
		vec3 p1 = transformPoint(skin1, position[i]);
		vec3 n1 = transformVector(skin1, normal[i]);

		Transform skin2 = combine(pose[joint.z], inverse(bindPose[joint.z]));
		vec3 p2 = transformPoint(skin2, position[i]);
		vec3 n2 = transformVector(skin2, normal[i]);

		Transform skin3 = combine(pose[joint.w], inverse(bindPose[joint.w]));
		vec3 p3 = transformPoint(skin3, position[i]);
		vec3 n3 = transformVector(skin3, normal[i]);
		skinned_position[i] = p0 * weight.x + p1 * weight.y + p2 * weight.z + p3 * weight.w;
		skinned_normal[i] = n0 * weight.x + n1 * weight.y + n2 * weight.z + n3 * weight.w;
	}

	attribute_position->Set(skinned_position);
	attribute_normal->Set(skinned_normal);
}
#endif
