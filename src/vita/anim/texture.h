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
   protected:

	unsigned int mWidth;
	unsigned int mHeight;
	unsigned int mChannels;
	unsigned int mHandle;

   private:

	Texture(Texture&& other) = delete;
	void operator=(Texture&& other) = delete;
	Texture(const Texture& other) = delete;
	void operator=(const Texture& other) = delete;

   public:

	Texture();
	Texture(const TextureFromFile& path);
	Texture(const TextureData& data);
	~Texture();

	void LoadFromFile(const TextureFromFile& path);
	void LoadFromMemory(const TextureData& data);

	void CompleteLoad(
		const std::string& name, int width, int height, int channels, unsigned char* data
	);

	void Set(int uniformIndex, unsigned int textureIndex);
	void UnSet(unsigned int textureIndex);
	unsigned int GetHandle();
};
