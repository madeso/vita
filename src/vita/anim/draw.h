#pragma once

#include "vita/anim/indexbuffer.h"

enum class DrawMode
{
	Points,
	LineStrip,
	LineLoop,
	Lines,
	Triangles,
	TriangleStrip,
	TriangleFan
};

void draw(IndexBuffer& inIndexBuffer, DrawMode mode);
void draw(unsigned int vertexCount, DrawMode mode);

void draw_instanced(IndexBuffer& inIndexBuffer, DrawMode mode, unsigned int instanceCount);
void draw_instanced(unsigned int vertexCount, DrawMode mode, unsigned int numInstances);
