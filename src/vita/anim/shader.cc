#include "vita/anim/shader.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>

constexpr std::ptrdiff_t STRING_LENGTH = 128;
constexpr std::ptrdiff_t LONG_STRING_LENGTH = 256;
constexpr std::ptrdiff_t LOG_LENGTH = 512;

Shader::Shader()
{
	mHandle = glCreateProgram();
}

Shader::Shader(const Source& vertex, const Source& fragment)
{
	mHandle = glCreateProgram();
	Load(vertex, fragment);
}

Shader::~Shader()
{
	glDeleteProgram(mHandle);
}

Source ReadStringFile(const std::string& path)
{
	std::ifstream file(path);
	std::stringstream contents;
	contents << file.rdbuf();
	return {contents.str()};
}

unsigned int Shader::CompileVertexShader(const std::string& vertex)
{
	unsigned int v_shader = glCreateShader(GL_VERTEX_SHADER);
	const char* v_source = vertex.c_str();
	glShaderSource(v_shader, 1, &v_source, NULL);
	glCompileShader(v_shader);
	int success = 0;
	glGetShaderiv(v_shader, GL_COMPILE_STATUS, &success);
	if (! success)
	{
		char infoLog[LOG_LENGTH];
		glGetShaderInfoLog(v_shader, LOG_LENGTH, NULL, infoLog);
		std::cout << "ERROR: Vertex compilation failed.\n";
		std::cout << "\t" << infoLog << "\n";
		glDeleteShader(v_shader);
		return 0;
	};
	return v_shader;
}

unsigned int Shader::CompileFragmentShader(const std::string& fragment)
{
	unsigned int f_shader = glCreateShader(GL_FRAGMENT_SHADER);
	const char* f_source = fragment.c_str();
	glShaderSource(f_shader, 1, &f_source, NULL);
	glCompileShader(f_shader);
	int success = 0;
	glGetShaderiv(f_shader, GL_COMPILE_STATUS, &success);
	if (! success)
	{
		char infoLog[LOG_LENGTH];
		glGetShaderInfoLog(f_shader, LOG_LENGTH, NULL, infoLog);
		std::cout << "ERROR: Fragment compilation failed.\n";
		std::cout << "\t" << infoLog << "\n";
		glDeleteShader(f_shader);
		return 0;
	};
	return f_shader;
}

bool Shader::LinkShaders(unsigned int vertex, unsigned int fragment)
{
	glAttachShader(mHandle, vertex);
	glAttachShader(mHandle, fragment);
	glLinkProgram(mHandle);
	int success = 0;
	glGetProgramiv(mHandle, GL_LINK_STATUS, &success);
	if (! success)
	{
		char infoLog[LOG_LENGTH];
		glGetProgramInfoLog(mHandle, LOG_LENGTH, NULL, infoLog);
		std::cout << "ERROR: Shader linking failed.\n";
		std::cout << "\t" << infoLog << "\n";
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		return false;
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);

	return true;
}

void Shader::PopulateAttributes()
{
	int count = -1;
	int length;
	char name[STRING_LENGTH];
	int size;
	GLenum type;

	glUseProgram(mHandle);
	glGetProgramiv(mHandle, GL_ACTIVE_ATTRIBUTES, &count);

	for (int i = 0; i < count; ++i)
	{
		std::memset(name, 0, sizeof(char) * STRING_LENGTH);
		glGetActiveAttrib(mHandle, (GLuint) i, STRING_LENGTH, &length, &size, &type, name);
		int attrib = glGetAttribLocation(mHandle, name);
		if (attrib >= 0)
		{
			mAttributes[name] = attrib;
		}
	}

	glUseProgram(0);
}

void Shader::PopulateUniforms()
{
	int count = -1;
	int length;
	char name[STRING_LENGTH];
	int size;
	GLenum type;
	char testName[LONG_STRING_LENGTH];

	glUseProgram(mHandle);
	glGetProgramiv(mHandle, GL_ACTIVE_UNIFORMS, &count);

	for (int i = 0; i < count; ++i)
	{
		std::memset(name, 0, sizeof(char) * STRING_LENGTH);
		glGetActiveUniform(mHandle, (GLuint) i, STRING_LENGTH, &length, &size, &type, name);

		int uniform = glGetUniformLocation(mHandle, name);
		if (uniform >= 0)
		{
			std::string uniformName = name;
			std::size_t found = uniformName.find('[');
			if (found != std::string::npos)
			{
				uniformName.erase(uniformName.begin() + found, uniformName.end());
				// Populate subscripted names too
				unsigned int uniformIndex = 0;
				while (true)
				{
					std::memset(testName, 0, sizeof(char) * LONG_STRING_LENGTH);
					sprintf(testName, "%s[%d]", uniformName.c_str(), uniformIndex++);
					int uniformLocation = glGetUniformLocation(mHandle, testName);
					if (uniformLocation < 0)
					{
						break;
					}
					mUniforms[testName] = uniformLocation;
				}
			}
			mUniforms[uniformName] = uniform;
		}
	}

	glUseProgram(0);
}

void Shader::Load(const Source& vertex, const Source& fragment)
{
	unsigned int v_shader = CompileVertexShader(vertex.source);
	unsigned int f_shader = CompileFragmentShader(fragment.source);
	if (LinkShaders(v_shader, f_shader))
	{
		PopulateAttributes();
		PopulateUniforms();
	}
}

void Shader::Bind()
{
	glUseProgram(mHandle);
}

void Shader::UnBind()
{
	glUseProgram(0);
}

unsigned int Shader::GetHandle()
{
	return mHandle;
}

unsigned int Shader::GetAttribute(const std::string& name)
{
	auto it = mAttributes.find(name);
	if (it == mAttributes.end())
	{
		std::cout << "Retrieving bad attribute index: " << name << "\n";
		mAttributes[name] = 0;
		return 0;
	}
	return it->second;
}

unsigned int Shader::GetUniform(const std::string& name)
{
	auto it = mUniforms.find(name);
	if (it == mUniforms.end())
	{
		std::cout << "Retrieving bad uniform index: " << name << "\n";
		mUniforms[name] = 0;
		return 0;
	}
	return it->second;
}
