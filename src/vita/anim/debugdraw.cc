#include "vita/anim/debugdraw.h"

#include "vita/anim/uniform.h"
#include "vita/anim/draw.h"

DebugDraw::DebugDraw()
{
	mAttribs = new Attribute<vec3>();

	mShader = new Shader(
		ShaderSource{"#version 330 core\n"
					 "uniform mat4 mvp;\n"
					 "in vec3 position;\n"
					 "void main() {\n"
					 "	gl_Position = mvp * vec4(position, 1.0);\n"
					 "}"},
		ShaderSource{"#version 330 core\n"
					 "uniform vec3 color;\n"
					 "out vec4 FragColor;\n"
					 "void main() {\n"
					 "	FragColor = vec4(color, 1);\n"
					 "}"}
	);
}

DebugDraw::DebugDraw(unsigned int size)
{
	mAttribs = new Attribute<vec3>();

	mShader = new Shader(
		ShaderSource{"#version 330 core\n"
					 "uniform mat4 mvp;\n"
					 "in vec3 position;\n"
					 "void main() {\n"
					 "	gl_Position = mvp * vec4(position, 1.0);\n"
					 "}"},
		ShaderSource{"#version 330 core\n"
					 "uniform vec3 color;\n"
					 "out vec4 FragColor;\n"
					 "void main() {\n"
					 "	FragColor = vec4(color, 1);\n"
					 "}"}
	);

	Resize(size);
}

DebugDraw::~DebugDraw()
{
	delete mAttribs;
	delete mShader;
}

unsigned int DebugDraw::Size()
{
	return static_cast<unsigned int>(mPoints.size());
}

void DebugDraw::Resize(unsigned int newSize)
{
	mPoints.resize(newSize);
}

vec3& DebugDraw::operator[](unsigned int index)
{
	return mPoints[index];
}

void DebugDraw::Push(const vec3& v)
{
	mPoints.push_back(v);
}

void DebugDraw::UpdateOpenGLBuffers()
{
	mAttribs->set(mPoints);
}

void DebugDraw::Draw(DebugDrawMode mode, const vec3& color, const mat4& mvp)
{
	mShader->bind();
	Uniform<mat4>::Set(mShader->get_uniform("mvp"), mvp);
	Uniform<vec3>::Set(mShader->get_uniform("color"), color);
	mAttribs->bind_to(mShader->get_attribute("position"));
	if (mode == DebugDrawMode::Lines)
	{
		::draw(Size(), DrawMode::Lines);
	}
	else if (mode == DebugDrawMode::Loop)
	{
		::draw(Size(), DrawMode::LineLoop);
	}
	else if (mode == DebugDrawMode::Strip)
	{
		::draw(Size(), DrawMode::LineStrip);
	}
	else
	{
		::draw(Size(), DrawMode::Points);
	}
	mAttribs->unbind_from(mShader->get_attribute("position"));
	mShader->unbind();
}
