#pragma once

#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "mat4.h"
#include <vector>
#include "vita/anim/attribute.h"
#include "vita/anim/indexbuffer.h"
#include "vita/anim/skeleton.h"
#include "vita/anim/pose.h"

struct Mesh
{
	std::vector<vec3> position;
	std::vector<vec3> normal;
	std::vector<vec2> texcoord;
	std::vector<vec4> weights;
	std::vector<ivec4> influences;

	std::vector<unsigned int> indices;

	Attribute<vec3>* attribute_position;
	Attribute<vec3>* attribute_normal;
	Attribute<vec2>* attribute_textcoord;
	Attribute<vec4>* attribute_weights;
	Attribute<ivec4>* attribute_influences;

	IndexBuffer* index_buffer;

	std::vector<vec3> skinned_position;
	std::vector<vec3> skinned_normal;
	std::vector<mat4> pose_palette;

	Mesh();
	Mesh(const Mesh&);

	Mesh& operator=(const Mesh&);

	~Mesh();

	void CPUSkin(Skeleton& skeleton, Pose& pose);
	void UpdateOpenGLBuffers();

	void Bind(int position, int normal, int texCoord, int weight, int influcence);
	void Draw();
	void DrawInstanced(unsigned int numInstances);
	void UnBind(int position, int normal, int texCoord, int weight, int influcence);
};
