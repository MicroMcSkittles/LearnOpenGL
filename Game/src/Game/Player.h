#pragma once
#include <glm/glm.hpp>
#include "Renderer/Texture.h"
#include "Renderer/Camera.h"

class Player {
public:
	Player(Texture* atlas, Camera* camera);

	void OnUpdate(float deltaTime);

	void Render();

	const glm::vec2& GetPosition() { return m_Position; }
	const glm::vec2& GetDirection() { return m_Direction; }
	float GetRotation() { return m_Rotation; }
	bool IsMoving() { return m_IsMoving; }

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
};