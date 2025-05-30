#pragma once
#include <glm/glm.hpp>
#include "Renderer/Texture.h"
#include "Renderer/Camera.h"
#include "Bullet.h"

class Player {
public:
	Player(Texture* atlas, Camera* camera);

	void OnUpdate(std::vector<Bullet>* enemyBullets, float deltaTime);

	void Render();
	void OnImGui();

	void SetPosition(const glm::vec2& pos) { m_Position = pos; }
	const glm::vec2& GetPosition() { return m_Position; }
	const glm::vec2& GetDirection() { return m_Direction; }
	float GetRotation() { return m_Rotation; }
	bool IsMoving() { return m_IsMoving; }

	void Damage(int damage);
	int GetHealth() { return m_Health; }

private:
	Texture* m_Atlas;
	Camera* m_Camera;
	glm::vec2 m_Position;
	glm::vec2 m_Direction;
	float m_Rotation;
	bool m_IsMoving;

	float m_Speed;
	float m_CameraLeadIn;
	float m_CameraLeadOut;

	int m_Health;
};