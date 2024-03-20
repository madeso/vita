#include "vita/anim/attribute.h"

#include "vita/anim/vec2.h"
#include "vita/anim/vec3.h"
#include "vita/anim/vec4.h"
#include "vita/anim/quat.h"

template<typename T>
void set_attibute_pointer(unsigned int slot);

template<>
void set_attibute_pointer<int>(unsigned int slot)
{
	glVertexAttribIPointer(slot, 1, GL_INT, 0, nullptr);
}

template<>
void set_attibute_pointer<ivec4>(unsigned int slot)
{
	glVertexAttribIPointer(slot, 4, GL_INT, 0, nullptr);
}

template<>
void set_attibute_pointer<float>(unsigned int slot)
{
	glVertexAttribPointer(slot, 1, GL_FLOAT, GL_FALSE, 0, nullptr);
}

template<>
void set_attibute_pointer<vec2>(unsigned int slot)
{
	glVertexAttribPointer(slot, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
}

template<>
void set_attibute_pointer<vec3>(unsigned int slot)
{
	glVertexAttribPointer(slot, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
}

template<>
void set_attibute_pointer<vec4>(unsigned int slot)
{
	glVertexAttribPointer(slot, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
}

template<>
void set_attibute_pointer<quat>(unsigned int slot)
{
	glVertexAttribPointer(slot, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
}

template<typename T>
Attribute<T>::Attribute()
{
	glGenBuffers(1, &handle);
	count = 0;
}

template<typename T>
Attribute<T>::~Attribute()
{
	glDeleteBuffers(1, &handle);
}

template<typename T>
void Attribute<T>::set_ptr(T* inputArray, unsigned int arrayLength)
{
	count = arrayLength;
	unsigned int size = sizeof(T);

	glBindBuffer(GL_ARRAY_BUFFER, handle);
	glBufferData(GL_ARRAY_BUFFER, size * count, inputArray, GL_STREAM_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

template<typename T>
void Attribute<T>::set(std::vector<T>& input)
{
	set_ptr(&input[0], static_cast<unsigned int>(input.size()));
}

template<typename T>
void Attribute<T>::bind_to(unsigned int slot)
{
	glBindBuffer(GL_ARRAY_BUFFER, handle);
	glEnableVertexAttribArray(slot);
	set_attibute_pointer<T>(slot);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

template<typename T>
void Attribute<T>::unbind_from(unsigned int slot)
{
	glBindBuffer(GL_ARRAY_BUFFER, handle);
	glDisableVertexAttribArray(slot);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

template struct Attribute<int>;
template struct Attribute<float>;
template struct Attribute<vec2>;
template struct Attribute<vec3>;
template struct Attribute<vec4>;
template struct Attribute<ivec4>;
template struct Attribute<quat>;
