#include "Bullet.h"
#include "Renderer/Renderer.h"

Bullet::Bullet(const glm::vec2& position, const glm::vec2& direction, const glm::vec2& texID, Texture* atlas) {
	m_Position = position;
	m_Direction = { direction.x, -direction.y };
	m_Atlas = atlas;
	m_TexID = texID;

	m_Speed = 5.0f;
	m_LifeSpan = 5.0f;
	m_Time = 0.0f;
}

void Bullet::OnUpdate(float deltaTime) {
	m_Time += deltaTime;
	m_Position += m_Direction * (deltaTime * m_Speed);
}
bool Bullet::ShouldKill()
{
	return (m_Time >= m_LifeSpan);
}

void Bullet::Render() {
	float rotation = atan2(m_Direction.x, -m_Direction.y);
	Renderer::DrawQuadAtlas(m_Position, { 0.125, 0.125 }, rotation, m_Atlas, { 3,2 }, m_TexID);
}