#include "Bullet.h"
#include "Renderer/Renderer.h"

Bullet::Bullet(const glm::vec2& position, const glm::vec2& direction, Texture* atlas) {
	m_Position = position;
	m_Direction = { direction.x, -direction.y };
	m_Atlas = atlas;

	m_Speed = 6.0f;
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
	Renderer::DrawQuadAtlas(m_Position, { 0.1, 0.1 }, rotation, m_Atlas, { 3,2 }, { 0,1 });
}