#include "Texture.h"
#include <stb/stb_image.h>
#include <glad/glad.h>

Texture::Texture(const std::string& path, const TextureConfig& config) {
	m_Config = config;
	m_CurrentSlot = m_Config.slot;

	stbi_set_flip_vertically_on_load(!m_Config.flipY);
	int nrChannels;
	unsigned char* data = stbi_load(path.c_str(), &m_Width, &m_Height, &nrChannels, 0);

	if (!data) {
		SOFT_ERROR("Failed to load texture ( " + path + " )");
		return;
	}

	glGenTextures(1, &m_Handle);
	glBindTexture(GL_TEXTURE_2D, m_Handle);

	if (m_Config.sWrapMode == TexWrapMode::None || m_Config.tWrapMode == TexWrapMode::None) {
		SOFT_ERROR("Failed to load texture ( " + path + " ) because it's wrap mode can't equal None");
		return;
	}
	if (m_Config.minFilter == TexFilterMode::None || m_Config.magFilter == TexFilterMode::None) {
		SOFT_ERROR("Failed to load texture ( " + path + " ) because it's filter mode can't equal None");
		return;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_TexWrapMode(m_Config.sWrapMode));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_TexWrapMode(m_Config.tWrapMode));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_TexFilterMode(m_Config.minFilter));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_TexFilterMode(m_Config.magFilter));

	uint32_t format = 0;
	if (nrChannels == 3) format = GL_RGB;
	else if (nrChannels == 4) format = GL_RGBA;
	else {
		SOFT_ERROR("Failed to load texture ( " + path + " ) because it's a unknown image format");
		return;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, format, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);

	LOW_MESSAGE("Successfully loaded texture ( " + path + " )");
}
Texture::Texture(const TextureConfig& config) {
	m_Config = config;
	m_Width = m_Config.width;
	m_Height = m_Config.height;
	m_CurrentSlot = m_Config.slot;

	unsigned char* image_data = (unsigned char*)m_Config.data;

	glGenTextures(1, &m_Handle);
	glBindTexture(GL_TEXTURE_2D, m_Handle);

	if (m_Config.sWrapMode == TexWrapMode::None || m_Config.tWrapMode == TexWrapMode::None) {
		SOFT_ERROR("Failed to load texture because it's wrap mode can't equal None");
		return;
	}
	if (m_Config.minFilter == TexFilterMode::None || m_Config.magFilter == TexFilterMode::None) {
		SOFT_ERROR("Failed to load texture because it's wrap mode can't equal None");
		return;
	}
	if (m_Config.format == TexFormat::None || m_Config.internalFormat == TexFormat::None) {
		SOFT_ERROR("Failed to load texture because it's format can't equal None");
		return;
	}
	if (m_Config.dataType == DataType::None) {
		SOFT_ERROR("Failed to load texture because it's data type can't equal None");
		return;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_TexWrapMode(m_Config.sWrapMode));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_TexWrapMode(m_Config.tWrapMode));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_TexFilterMode(m_Config.minFilter));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_TexFilterMode(m_Config.magFilter));

	glTexImage2D(GL_TEXTURE_2D, 0, GL_TexFormat(m_Config.internalFormat), m_Width, m_Height, 0,
		GL_TexFormat(m_Config.format), GL_DataType(m_Config.dataType), image_data);
	glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::Resize(uint32_t width, uint32_t height, void* data) {
	m_Width = width;
	m_Height = height;
	m_Config.data = data;

	unsigned char* image_data = (unsigned char*)m_Config.data;

	glBindTexture(GL_TEXTURE_2D, m_Handle);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_TexWrapMode(m_Config.sWrapMode));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_TexWrapMode(m_Config.tWrapMode));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_TexFilterMode(m_Config.minFilter));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_TexFilterMode(m_Config.magFilter));

	glTexImage2D(GL_TEXTURE_2D, 0, GL_TexFormat(m_Config.internalFormat), m_Width, m_Height, 0,
		GL_TexFormat(m_Config.format), GL_DataType(m_Config.dataType), image_data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Bind()
{
	glActiveTexture(GL_TEXTURE0 + m_CurrentSlot);
	glBindTexture(GL_TEXTURE_2D, m_Handle);
}

void Texture::Unbind()
{
	glActiveTexture(GL_TEXTURE0 + m_CurrentSlot);
	glBindTexture(GL_TEXTURE_2D, 0);
}
