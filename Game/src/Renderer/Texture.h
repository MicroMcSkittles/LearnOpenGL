#pragma once
#include <glm/glm.hpp>

enum class TexWrapMode
{
	None = 0,
	Repeat,
	MirroredRepeat,
	ClampToEdge,
	ClampToBorder
};
enum class TexFilterMode
{
	None = 0,
	Nearest,
	Linear
};
enum class TexFormat 
{
	None = 0,
	RED,
	RG,
	RGB,
	RGBA,
	DEPTH24_STENCIL8,
	DEPTH_STENCIL,
};
enum class TexDataType
{
	None = 0,
	UNSIGNED_BYTE,
	UNSIGNED_INT,
	UNSIGNED_INT_24_8,
};

struct TextureConfig
{
	TexWrapMode SWrapMode = TexWrapMode::Repeat; // X wrap mode
	TexWrapMode TWrapMode = TexWrapMode::Repeat; // Y wrap mode
	glm::vec3 BorderColor = { 1.0f, 0.0f, 1.0f };
	TexFilterMode MinFilter = TexFilterMode::Nearest;
	TexFilterMode MagFilter = TexFilterMode::Linear;

	TexDataType DataType = TexDataType::UNSIGNED_BYTE; // Not used when loading a texture from file
	TexFormat Format = TexFormat::RGB; // Not used when loading a texture from file
	TexFormat InternalFormat = TexFormat::RGB; // Not used when loading a texture from file
	void* Data = nullptr; // Not used when loading a texture from file
	uint32_t Width = 0; // Not used when loading a texture from file
	uint32_t Height = 0; // Not used when loading a texture from file

	bool FlipY = false; // Used when loading a texture from file
};

class Texture {
public:

	Texture(const std::string& path, const TextureConfig& config = {});
	Texture(const TextureConfig& config = {});

	void Resize(uint32_t width, uint32_t height, void* data = (void*)NULL);

	unsigned int GetHandle() { return m_Handle; }

	uint32_t GetWidth() { return m_Width; }
	uint32_t GetHeight() { return m_Height; }

private:
	unsigned int m_Handle;

	TextureConfig m_Config;
	int m_Width;
	int m_Height;
};