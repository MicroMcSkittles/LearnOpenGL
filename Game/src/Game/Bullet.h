#pragma once
#include <glm/glm.hpp>
#include "Renderer/Texture.h"

class Bullet {
public:
	Bullet(const glm::vec2& position, const glm::vec2& direction, const glm::vec2& texID, Texture* atlas);

	void OnUpdate(float deltaTime);
	bool ShouldKill();
	void Render();

	const glm::vec2& GetPosition() { return m_Position; }

private:
	Texture* m_Atlas;
	glm::vec2 m_TexID;

	glm::vec2 m_Position;
	glm::vec2 m_Direction;

	float m_Speed;
	float m_LifeSpan;
	float m_Time;
};