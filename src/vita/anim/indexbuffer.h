#pragma once

struct IndexBuffer
{
	unsigned int handle;
	unsigned int count;

	IndexBuffer();
	~IndexBuffer();

	IndexBuffer(const IndexBuffer& other) = delete;
	void operator=(const IndexBuffer& other) = delete;
	IndexBuffer(IndexBuffer&& other) = delete;
	void operator=(IndexBuffer&& other) = delete;

	void Set(const unsigned int* inputArray, unsigned int arrayLength);
	void Set(const std::vector<unsigned int>& input);
};
