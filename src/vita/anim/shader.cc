#include "vita/anim/shader.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>

constexpr std::ptrdiff_t STRING_LENGTH = 128;
constexpr std::ptrdiff_t LONG_STRING_LENGTH = 256;
constexpr std::ptrdiff_t LOG_LENGTH = 512;

Shader::Shader()
	: handle(glCreateProgram())
{
}

Shader::Shader(const ShaderSource& vertex, const ShaderSource& fragment)
	: handle(glCreateProgram())
{
	Load(vertex, fragment);
}

Shader::~Shader()
{
	glDeleteProgram(handle);
}

ShaderSource read_shader_file(const std::string& path)
{
	std::ifstream file(path);
	std::stringstream contents;
	contents << file.rdbuf();
	return {contents.str()};
}

unsigned int CompileVertexShader(const std::string& vertex)
{
	const auto v_shader = glCreateShader(GL_VERTEX_SHADER);
	const auto v_source = vertex.c_str();
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

unsigned int CompileFragmentShader(const std::string& fragment)
{
	const auto f_shader = glCreateShader(GL_FRAGMENT_SHADER);
	const auto f_source = fragment.c_str();
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

bool LinkShaders(Shader* shader, unsigned int vertex, unsigned int fragment)
{
	glAttachShader(shader->handle, vertex);
	glAttachShader(shader->handle, fragment);
	glLinkProgram(shader->handle);

	int success = 0;
	glGetProgramiv(shader->handle, GL_LINK_STATUS, &success);
	if (! success)
	{
		char infoLog[LOG_LENGTH];
		glGetProgramInfoLog(shader->handle, LOG_LENGTH, NULL, infoLog);
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

std::unordered_map<std::string, unsigned int> PopulateAttributes(Shader* shader)
{
	int count = -1;
	int length;
	char name[STRING_LENGTH];
	int size;
	GLenum type;

	std::unordered_map<std::string, unsigned int> attributes;

	glUseProgram(shader->handle);
	glGetProgramiv(shader->handle, GL_ACTIVE_ATTRIBUTES, &count);

	for (int i = 0; i < count; ++i)
	{
		std::memset(name, 0, sizeof(char) * STRING_LENGTH);
		glGetActiveAttrib(
			shader->handle, static_cast<GLuint>(i), STRING_LENGTH, &length, &size, &type, name
		);
		const auto attrib = glGetAttribLocation(shader->handle, name);
		if (attrib >= 0)
		{
			attributes[name] = static_cast<unsigned int>(attrib);
		}
	}

	glUseProgram(0);

	return attributes;
}

std::unordered_map<std::string, int> PopulateUniforms(Shader* shader)
{
	int count = 0;
	int length;
	char name[STRING_LENGTH];
	int size;
	GLenum type;
	char testName[LONG_STRING_LENGTH];
	std::unordered_map<std::string, int> uniforms;

	glUseProgram(shader->handle);
	glGetProgramiv(shader->handle, GL_ACTIVE_UNIFORMS, &count);

	for (int i = 0; i < count; ++i)
	{
		std::memset(name, 0, sizeof(char) * STRING_LENGTH);
		glGetActiveUniform(
			shader->handle, static_cast<GLuint>(i), STRING_LENGTH, &length, &size, &type, name
		);

		int uniform = glGetUniformLocation(shader->handle, name);
		if (uniform >= 0)
		{
			std::string uniformName = name;
			std::size_t found = uniformName.find('[');
			if (found != std::string::npos)
			{
				uniformName.erase(
					uniformName.begin() + static_cast<std::ptrdiff_t>(found), uniformName.end()
				);
				// Populate subscripted names too
				unsigned int uniformIndex = 0;
				while (true)
				{
					std::memset(testName, 0, sizeof(char) * LONG_STRING_LENGTH);
					sprintf(testName, "%s[%d]", uniformName.c_str(), uniformIndex++);
					int uniformLocation = glGetUniformLocation(shader->handle, testName);
					if (uniformLocation < 0)
					{
						break;
					}
					uniforms[testName] = uniformLocation;
				}
			}
			uniforms[uniformName] = uniform;
		}
	}

	glUseProgram(0);

	return uniforms;
}

void Shader::Load(const ShaderSource& vertex, const ShaderSource& fragment)
{
	const auto v_shader = CompileVertexShader(vertex.source);
	const auto f_shader = CompileFragmentShader(fragment.source);
	if (LinkShaders(this, v_shader, f_shader))
	{
		attributes = PopulateAttributes(this);
		uniforms = PopulateUniforms(this);
	}
}

void Shader::bind()
{
	glUseProgram(handle);
}

void Shader::unbind()
{
	glUseProgram(0);
}

unsigned int Shader::get_attribute(const std::string& name)
{
	const auto it = attributes.find(name);
	if (it == attributes.end())
	{
		std::cout << "Retrieving bad attribute index: " << name << "\n";
		attributes[name] = 0;
		return 0;
	}
	return it->second;
}

int Shader::get_uniform(const std::string& name)
{
	const auto it = uniforms.find(name);
	if (it == uniforms.end())
	{
		std::cout << "Retrieving bad uniform index: " << name << "\n";
		uniforms[name] = -1;
		return -1;
	}
	return it->second;
}
