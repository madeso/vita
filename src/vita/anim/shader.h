#pragma once

struct Source
{
	std::string source;
};

Source ReadStringFile(const std::string& path);

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
	Shader(const Source& vertex, const Source& fragment);
	~Shader();

	void Load(const Source& vertex, const Source& fragment);

	void Bind();
	void UnBind();

	unsigned int GetAttribute(const std::string& name);
	int GetUniform(const std::string& name);
};
