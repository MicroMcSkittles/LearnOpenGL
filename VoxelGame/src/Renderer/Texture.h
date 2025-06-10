#pragma once
#include "Renderer/RendererEnum.h"
#include <glm/glm.hpp>

struct TextureConfig {
	// Used when loading a texture from file
	TexWrapMode sWrapMode = TexWrapMode::Repeat; // X wrap mode
	TexWrapMode tWrapMode = TexWrapMode::Repeat; // Y wrap mode
	TexFilterMode minFilter = TexFilterMode::Nearest;
	TexFilterMode magFilter = TexFilterMode::Linear;
	glm::vec3 borderColor = { 1.0f, 0.0f, 1.0f };
	bool flipY = false; 


	// Not used when loading a texture from file
	DataType dataType = DataType::UnsignedByte;
	TexFormat format = TexFormat::RGB;
	TexFormat internalFormat = TexFormat::RGB;
	void* data = nullptr;
	uint32_t width = 0;
	uint32_t height = 0;

	// Used both when loading from file and not loading from file
	uint32_t slot = 0; // the slot the texture will bind to when passing to the GPU
};

class Texture {
public:

	Texture(const std::string& path, const TextureConfig& config = {});
	Texture(const TextureConfig& config = {});

	void Resize(uint32_t width, uint32_t height, void* data = (void*)NULL);

	void SetSlot(uint32_t slot) { m_CurrentSlot = slot; }
	uint32_t GetSlot() { return m_CurrentSlot; }

	void Bind();
	void Unbind();

	unsigned int GetHandle() { return m_Handle; }

	uint32_t GetWidth() { return m_Width; }
	uint32_t GetHeight() { return m_Height; }

private:
	uint32_t m_Handle;
	uint32_t m_CurrentSlot;

	TextureConfig m_Config;
	int m_Width;
	int m_Height;
};