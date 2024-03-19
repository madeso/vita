#pragma once

struct TextureFromFile
{
	std::string path;
};

struct TextureData
{
	const void* buffer;
	int length;
	std::string name;
};

struct Texture
{
	unsigned int width;
	unsigned int height;
	unsigned int channels;

	unsigned int handle;

	Texture(const TextureFromFile& path);
	Texture(const TextureData& data);
	~Texture();

	Texture() = delete;
	Texture(Texture&& other) = delete;
	void operator=(Texture&& other) = delete;
	Texture(const Texture& other) = delete;
	void operator=(const Texture& other) = delete;

	void Set(int uniformIndex, unsigned int textureIndex);
	void UnSet(unsigned int textureIndex);
};
