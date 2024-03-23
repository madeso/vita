#include "vita/anim/texture.h"

#include "stb_image.h"

#include <iostream>

void CompleteLoad(
	Texture* out, const std::string& name, int width, int height, int channels, unsigned char* data
)
{
	if (data == nullptr)
	{
		std::cerr << "Failed to load image " << name << "\n";
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	out->width = static_cast<unsigned int>(width);
	out->height = static_cast<unsigned int>(height);
	out->channels = static_cast<unsigned int>(channels);
}

void LoadFromFile(Texture* out, const TextureFromFile& path)
{
	glBindTexture(GL_TEXTURE_2D, out->handle);

	int width, height, channels;
	auto data = stbi_load(path.path.c_str(), &width, &height, &channels, 4);

	CompleteLoad(out, path.path, width, height, channels, data);
}

void LoadFromMemory(Texture* out, const TextureData& tex)
{
	glBindTexture(GL_TEXTURE_2D, out->handle);

	int width, height, channels;

	auto data = stbi_load_from_memory(
		static_cast<const stbi_uc*>(tex.buffer), tex.length, &width, &height, &channels, 4
	);
	CompleteLoad(out, tex.name, width, height, channels, data);
}

Texture::Texture(const TextureFromFile& path)
{
	glGenTextures(1, &handle);
	LoadFromFile(this, path);
}

Texture::Texture(const TextureData& data)
{
	glGenTextures(1, &handle);
	LoadFromMemory(this, data);
}

Texture::~Texture()
{
	glDeleteTextures(1, &handle);
	handle = 0;
}

void Texture::bind(int uniformIndex, unsigned int textureIndex)
{
	glActiveTexture(GL_TEXTURE0 + textureIndex);
	glBindTexture(GL_TEXTURE_2D, handle);
	glUniform1i(uniformIndex, textureIndex);
}

void Texture::unbind(unsigned int textureIndex)
{
	glActiveTexture(GL_TEXTURE0 + textureIndex);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
}
