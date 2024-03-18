#include "vita/anim/uniform.h"

#include "vita/anim/vec2.h"
#include "vita/anim/vec3.h"
#include "vita/anim/vec4.h"
#include "vita/anim/quat.h"
#include "vita/anim/mat4.h"

#define UNIFORM_IMPL(gl_func, tType, dType) \
	template<> \
	void Uniform<tType>::Set(int slot, const tType* data, unsigned int length) \
	{ \
		gl_func(slot, static_cast<GLsizei>(length), static_cast<const dType*>(&data[0])); \
	}
UNIFORM_IMPL(glUniform1iv, int, int)
UNIFORM_IMPL(glUniform1fv, float, float)
#undef UNIFORM_IMPL

#define UNIFORM_IMPL(gl_func, tType, dType) \
	template<> \
	void Uniform<tType>::Set(int slot, const tType* data, unsigned int length) \
	{ \
		gl_func(slot, static_cast<GLsizei>(length), data->data_ptr()); \
	}
UNIFORM_IMPL(glUniform4iv, ivec4, int)
UNIFORM_IMPL(glUniform2iv, ivec2, int)
UNIFORM_IMPL(glUniform2fv, vec2, float)
UNIFORM_IMPL(glUniform3fv, vec3, float)
UNIFORM_IMPL(glUniform4fv, vec4, float)
UNIFORM_IMPL(glUniform4fv, quat, float)
#undef UNIFORM_IMPL

template<>
void Uniform<mat4>::Set(int slot, const mat4* inputArray, unsigned int arrayLength)
{
	glUniformMatrix4fv(slot, static_cast<GLsizei>(arrayLength), false, inputArray->data_ptr());
}

template<typename T>
void Uniform<T>::Set(int slot, const T& value)
{
	Set(slot, &value, 1);
}

template<typename T>
void Uniform<T>::Set(int slot, const std::vector<T>& value)
{
	Set(slot, &value[0], static_cast<unsigned int>(value.size()));
}

template class Uniform<int>;
template class Uniform<ivec4>;
template class Uniform<ivec2>;
template class Uniform<float>;
template class Uniform<vec2>;
template class Uniform<vec3>;
template class Uniform<vec4>;
template class Uniform<quat>;
template class Uniform<mat4>;
