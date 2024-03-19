#pragma once

#include "vita/anim/indexbuffer.h"

enum struct DrawMode
{
	Points,
	LineStrip,
	LineLoop,
	Lines,
	Triangles,
	TriangleStrip,
	TriangleFan
};

void Draw(IndexBuffer& inIndexBuffer, DrawMode mode);
void Draw(unsigned int vertexCount, DrawMode mode);

void DrawInstanced(IndexBuffer& inIndexBuffer, DrawMode mode, unsigned int instanceCount);
void DrawInstanced(unsigned int vertexCount, DrawMode mode, unsigned int numInstances);
