#pragma once

struct Source
{
	std::string source;
};

Source ReadFile(const std::string& path);

class Shader
{
	unsigned int mHandle;
	std::unordered_map<std::string, unsigned int> mAttributes;
	std::unordered_map<std::string, unsigned int> mUniforms;

	unsigned int CompileVertexShader(const std::string& vertex);
	unsigned int CompileFragmentShader(const std::string& fragment);
	bool LinkShaders(unsigned int vertex, unsigned int fragment);

	void PopulateAttributes();
	void PopulateUniforms();

	Shader(Shader&&) = delete;
	void operator=(Shader&&) = delete;
	Shader(const Shader&) = delete;
	void operator=(const Shader&) = delete;
	Shader(Shader&) = delete;

   public:

	Shader();
	Shader(const Source& vertex, const Source& fragment);
	~Shader();

	void Load(const Source& vertex, const Source& fragment);

	void Bind();
	void UnBind();

	unsigned int GetAttribute(const std::string& name);
	unsigned int GetUniform(const std::string& name);
	unsigned int GetHandle();
};
