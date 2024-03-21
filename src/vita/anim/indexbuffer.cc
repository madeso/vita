#include "vita/anim/indexbuffer.h"

IndexBuffer::IndexBuffer()
	: handle(0)
	, count(0)
{
	glGenBuffers(1, &handle);
}

IndexBuffer::~IndexBuffer()
{
	glDeleteBuffers(1, &handle);
}

void IndexBuffer::Set(const unsigned int* inputArray, unsigned int arrayLength)
{
	count = arrayLength;
	unsigned int size = sizeof(unsigned int);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * count, inputArray, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBuffer::Set(const std::vector<unsigned int>& input)
{
	Set(&input[0], static_cast<unsigned int>(input.size()));
}
