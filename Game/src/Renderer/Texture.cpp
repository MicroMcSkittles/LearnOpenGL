#include "Texture.h"
#include <glad/glad.h>
#include <stb/stb_image.h>
#include "Core/Application.h"
#include "Core/Event/Event.h"

namespace {
	uint32_t GL_TexWrapMode(TexWrapMode mode) {
		switch (mode)
		{
		case TexWrapMode::None: return 0;
		case TexWrapMode::Repeat: return GL_REPEAT;
		case TexWrapMode::MirroredRepeat: return GL_MIRRORED_REPEAT;
		case TexWrapMode::ClampToEdge: return GL_CLAMP_TO_EDGE;
		case TexWrapMode::ClampToBorder: return GL_CLAMP_TO_BORDER;
		default: return 0;
		}
	}
	uint32_t GL_TexFilterMode(TexFilterMode mode) {
		switch (mode)
		{
		case TexFilterMode::None: return 0;
		case TexFilterMode::Nearest: return GL_NEAREST;
		case TexFilterMode::Linear: return GL_LINEAR;
		}
	}
}

Texture::Texture(const std::string& path, const TextureConfig& config)
{
	m_Config = config;

	stbi_set_flip_vertically_on_load(!m_Config.FlipY);
	int nrChannels;
	unsigned char* data = stbi_load(path.c_str(), &m_Width, &m_Height, &nrChannels, 0);

	if (!data) {
		ErrorEvent err("Failed to load texture");
		Application::Get()->ProcEvent(err);
	}

	glGenTextures(1, &m_Handle);
	glBindTexture(GL_TEXTURE_2D, m_Handle);

	if (m_Config.SWrapMode == TexWrapMode::None || m_Config.TWrapMode == TexWrapMode::None) {
		ErrorEvent err("Failed to load texture ( Wrap mode can't equal None )");
		Application::Get()->ProcEvent(err);
	}
	if (m_Config.MinFilter == TexFilterMode::None || m_Config.MagFilter == TexFilterMode::None) {
		ErrorEvent err("Failed to load texture ( Filter mode can't equal None )");
		Application::Get()->ProcEvent(err);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_TexWrapMode(m_Config.SWrapMode));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_TexWrapMode(m_Config.TWrapMode));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_TexFilterMode(m_Config.MinFilter));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_TexFilterMode(m_Config.MagFilter));

	uint32_t format = 0;
	if (nrChannels == 3) format = GL_RGB;
	else if (nrChannels == 4) format = GL_RGBA;
	else {
		ErrorEvent err("Unknown image format ( " + path + " )");
		Application::Get()->ProcEvent(err);
	}

	glTexImage2D(GL_TEXTURE_2D, 0, format, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);
}

Texture::Texture(const TextureConfig& config)
{
	m_Config = config;
	m_Width = m_Config.Width;
	m_Height = m_Config.Height;

	unsigned char* image_data = (unsigned char*)config.Data;

	if (!image_data) {
		ErrorEvent err("Failed to load texture");
		Application::Get()->ProcEvent(err);
	}

	glGenTextures(1, &m_Handle);
	glBindTexture(GL_TEXTURE_2D, m_Handle);

	if (m_Config.SWrapMode == TexWrapMode::None || m_Config.TWrapMode == TexWrapMode::None) {
		ErrorEvent err("Failed to load texture ( Wrap mode can't equal None )");
		Application::Get()->ProcEvent(err);
	}
	if (m_Config.MinFilter == TexFilterMode::None || m_Config.MagFilter == TexFilterMode::None) {
		ErrorEvent err("Failed to load texture ( Filter mode can't equal None )");
		Application::Get()->ProcEvent(err);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_TexWrapMode(m_Config.SWrapMode));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_TexWrapMode(m_Config.TWrapMode));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_TexFilterMode(m_Config.MinFilter));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_TexFilterMode(m_Config.MagFilter));

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
	glGenerateMipmap(GL_TEXTURE_2D);

	//stbi_image_free(data);
}
