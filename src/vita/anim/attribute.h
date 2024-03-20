#pragma once

template<typename T>
struct Attribute
{
	unsigned int handle;
	unsigned int count;

	Attribute(const Attribute& other) = delete;
	void operator=(const Attribute& other) = delete;

	Attribute(Attribute&& other) = delete;
	void operator=(Attribute&& other) = delete;

	Attribute();
	~Attribute();

	void set_ptr(T* inputArray, unsigned int arrayLength);
	void set(std::vector<T>& input);

	void bind_to(unsigned int slot);
	void unbind_from(unsigned int slot);
};
