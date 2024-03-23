#include "vita/anim/draw.h"

#include <iostream>

static GLenum C(DrawMode input)
{
	switch (input)
	{
	case DrawMode::Points: return GL_POINTS;
	case DrawMode::LineStrip: return GL_LINE_STRIP;
	case DrawMode::LineLoop: return GL_LINE_LOOP;
	case DrawMode::Lines: return GL_LINES;
	case DrawMode::Triangles: return GL_TRIANGLES;
	case DrawMode::TriangleStrip: return GL_TRIANGLE_STRIP;
	case DrawMode::TriangleFan: return GL_TRIANGLE_FAN;
	default: std::cout << "C unreachable code hit\n"; return 0;
	}
}

void draw(unsigned int vertexCount, DrawMode mode)
{
	glDrawArrays(C(mode), 0, vertexCount);
}

void draw_instanced(unsigned int vertexCount, DrawMode mode, unsigned int numInstances)
{
	glDrawArraysInstanced(C(mode), 0, vertexCount, numInstances);
}

void draw(IndexBuffer& inIndexBuffer, DrawMode mode)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, inIndexBuffer.handle);
	glDrawElements(C(mode), inIndexBuffer.count, GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void draw_instanced(IndexBuffer& inIndexBuffer, DrawMode mode, unsigned int instanceCount)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, inIndexBuffer.handle);
	glDrawElementsInstanced(C(mode), inIndexBuffer.count, GL_UNSIGNED_INT, 0, instanceCount);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
