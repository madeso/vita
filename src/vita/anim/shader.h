#pragma once

struct ShaderSource
{
	std::string source;
};

ShaderSource read_shader_file(const std::string& path);

struct Shader
{
	unsigned int handle;
	std::unordered_map<std::string, unsigned int> attributes;
	std::unordered_map<std::string, int> uniforms;

	Shader(Shader&&) = delete;
	void operator=(Shader&&) = delete;
	Shader(const Shader&) = delete;
	void operator=(const Shader&) = delete;
	Shader(Shader&) = delete;

	Shader();
	Shader(const ShaderSource& vertex, const ShaderSource& fragment);
	~Shader();

	void Load(const ShaderSource& vertex, const ShaderSource& fragment);

	void bind();
	void unbind();

	unsigned int get_attribute(const std::string& name);
	int get_uniform(const std::string& name);
};
