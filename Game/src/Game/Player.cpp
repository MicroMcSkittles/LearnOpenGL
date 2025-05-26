#include "Player.h"
#include "Core/Application.h"
#include "Renderer/Renderer.h"
#include "Core/Input.h"

Player::Player(Texture* atlas, Camera* camera) {
	m_Atlas = atlas;
	m_Camera = camera;
	m_Position = { 0,0 };
	m_Rotation = 0;
	m_IsMoving = false;

	m_Speed = 2.5;
	m_CameraLeadIn = 3;
	m_CameraLeadOut = 2.5f;
}

void Player::OnUpdate(float deltaTime) {
	// Camera
	glm::vec2 cameraPos = m_Camera->GetPosition();
	float dist = glm::distance(m_Position, cameraPos);
	if (dist >= 0.4) {
		glm::vec2 dir = m_Position - cameraPos;
		dir = glm::normalize(dir);
		cameraPos += dir * (deltaTime * m_CameraLeadIn * dist);
		m_Camera->SetPosition(cameraPos);
	}
	else if (dist >= 0.02 && !m_IsMoving) {
		glm::vec2 dir = m_Position - cameraPos;
		dir = glm::normalize(dir);
		cameraPos += dir * (deltaTime * (m_CameraLeadOut * dist));
		m_Camera->SetPosition(cameraPos);
	}

	// Player
	glm::vec2 move = { 0, 0 };
	if (Input::IsKeyPressed(KEY_W)) move.y = 1;
	if (Input::IsKeyPressed(KEY_S)) move.y = -1;
	if (Input::IsKeyPressed(KEY_A)) move.x = -1;
	if (Input::IsKeyPressed(KEY_D)) move.x = 1;

	if (move.x != 0 || move.y != 0) {
		move = glm::normalize(move);
		move *= deltaTime * m_Speed;
		m_Position += move;
		m_IsMoving = true;
	}
	else m_IsMoving = false;

	// Player rotation
	glm::vec2 mousePos = {
		Input::GetMouseX(),
		Input::GetMouseY()
	};
	glm::vec2 playerScreenPos = m_Camera->ScreenSpacePos(m_Position);
	glm::vec2 mouseDir = mousePos - playerScreenPos;
	if (glm::length(mouseDir) == 0.0f) mouseDir = { 1, 0 };
	else mouseDir = glm::normalize(mouseDir);
	m_Direction = mouseDir;
	m_Rotation = atan2(mouseDir.x, mouseDir.y);
}

void Player::Render() {
	Renderer::DrawQuadAtlas(m_Position, { 0.25f,0.25f }, m_Rotation, m_Atlas, { 5, 2 }, { 1, 1 });
}
