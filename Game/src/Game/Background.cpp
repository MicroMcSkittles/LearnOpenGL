#include "Background.h"
#include "Renderer/Renderer.h"

Background::Background(const std::string& texture, float paralaxStrength, Camera* camera) {
	m_Camera = camera;
	m_ParalaxStrength = paralaxStrength;

	TextureConfig config;
	config.MinFilter = TexFilterMode::Nearest;
	config.MagFilter = TexFilterMode::Nearest;
	m_Texture = new Texture(texture, config);

	m_TileSize = 10;
}

void Background::Render(const glm::vec2& position) {
	glm::vec2 scale = { m_TileSize, m_TileSize };

	glm::vec2 paralaxDist = m_Camera->GetPosition();
	paralaxDist *= -m_ParalaxStrength;

	for (int y = -1; y <= 1; ++y) {
		for (int x = -1; x <= 1; ++x) {
			glm::vec2 pos;
			pos.x = x + int((position.x - paralaxDist.x) / m_TileSize);
			pos.y = y + int((position.y - paralaxDist.y) / m_TileSize);
			pos *= m_TileSize;
			Renderer::DrawQuad(pos + paralaxDist, scale, m_Texture);
		}
	}
}