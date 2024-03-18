#include "vita/anim/texture.h"

#include "stb_image.h"

#include <iostream>

Texture::Texture()
{
	mWidth = 0;
	mHeight = 0;
	mChannels = 0;
	glGenTextures(1, &mHandle);
}

Texture::Texture(const TextureFromFile& path)
{
	glGenTextures(1, &mHandle);
	LoadFromFile(path);
}

Texture::Texture(const TextureData& data)
{
	glGenTextures(1, &mHandle);
	LoadFromMemory(data);
}

Texture::~Texture()
{
	glDeleteTextures(1, &mHandle);
}

void Texture::LoadFromFile(const TextureFromFile& path)
{
	glBindTexture(GL_TEXTURE_2D, mHandle);

	int width, height, channels;
	unsigned char* data = stbi_load(path.path.c_str(), &width, &height, &channels, 4);

	CompleteLoad(path.path, width, height, channels, data);
}

void Texture::LoadFromMemory(const TextureData& tex)
{
	glBindTexture(GL_TEXTURE_2D, mHandle);

	int width, height, channels;

	auto* data = stbi_load_from_memory(
		static_cast<const stbi_uc*>(tex.buffer), tex.length, &width, &height, &channels, 4
	);
	CompleteLoad(tex.name, width, height, channels, data);
}

void Texture::CompleteLoad(
	const std::string& name, int width, int height, int channels, unsigned char* data
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

	mWidth = static_cast<unsigned int>(width);
	mHeight = static_cast<unsigned int>(height);
	mChannels = static_cast<unsigned int>(channels);
}

void Texture::Set(int uniformIndex, unsigned int textureIndex)
{
	glActiveTexture(GL_TEXTURE0 + textureIndex);
	glBindTexture(GL_TEXTURE_2D, mHandle);
	glUniform1i(uniformIndex, textureIndex);
}

void Texture::UnSet(unsigned int textureIndex)
{
	glActiveTexture(GL_TEXTURE0 + textureIndex);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
}

unsigned int Texture::GetHandle()
{
	return mHandle;
}
