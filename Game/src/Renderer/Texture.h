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

struct TextureConfig
{
	TexWrapMode SWrapMode = TexWrapMode::Repeat; // X wrap mode
	TexWrapMode TWrapMode = TexWrapMode::Repeat; // Y wrap mode
	glm::vec3 BorderColor = { 1.0f, 0.0f, 1.0f };
	TexFilterMode MinFilter = TexFilterMode::Nearest;
	TexFilterMode MagFilter = TexFilterMode::Linear;

	void* Data = nullptr; // Not used when loading a texture from file
	uint32_t Width = 0; // Not used when loading a texture from file
	uint32_t Height = 0; // Not used when loading a texture from file

	bool FlipY = false; // Used when loading a texture from file
};

class Texture {
public:

	Texture(const std::string& path, const TextureConfig& config = {});
	Texture(const TextureConfig& config = {});

	unsigned int GetHandle() { return m_Handle; }

	uint32_t GetWidth() { return m_Width; }
	uint32_t GetHeight() { return m_Height; }

private:
	unsigned int m_Handle;

	TextureConfig m_Config;
	int m_Width;
	int m_Height;
};