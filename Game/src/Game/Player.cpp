#include "Player.h"
#include <imgui.h>
#include "Core/Application.h"
#include "Core/Input.h"
#include "Renderer/Renderer.h"

Player::Player(Texture* atlas, Camera* camera) {
	m_Atlas = atlas;
	m_Camera = camera;
	m_Position = { 0,0 };
	m_Rotation = 0;
	m_IsMoving = false;

	m_Speed = 2.5;
	m_CameraLeadIn = 3;
	m_CameraLeadOut = 2.5f;

	m_Health = 10;
}

void Player::OnUpdate(std::vector<Bullet>* enemyBullets, float deltaTime) {
	for (int i = 0; i < enemyBullets->size(); ++i) {
		Bullet& b = enemyBullets->at(i);
		if (glm::distance(m_Position, b.GetPosition()) <= (0.25f / 2.0f)) {
			enemyBullets->erase(enemyBullets->begin() + i);
			i -= 1;
			Damage(1);
		}
	}
	
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

void Player::OnImGui()
{
	ImGui::BeginChild("Player", { 0,0 }, ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Border);
	ImGui::InputFloat2("Position", &m_Position.x);
	ImGui::InputFloat2("Direction", &m_Direction.x);
	ImGui::InputFloat("Rotation", &m_Rotation);
	ImGui::Separator();
	ImGui::Checkbox("Is Moving", &m_IsMoving);
	ImGui::InputFloat("Speed", &m_Speed);
	ImGui::InputFloat("Camera Lead In", &m_CameraLeadIn);
	ImGui::InputFloat("Camera Lead Out", &m_CameraLeadOut);
	ImGui::Separator();
	ImGui::InputInt("Health", &m_Health);
	ImGui::EndChild();
	ImGui::SeparatorText("Camera");
	ImGui::BeginChild("Camera", { 0,0 }, ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Border);
	glm::vec2 cPos = m_Camera->GetPosition();
	float cRot = m_Camera->GetRotation();
	ImGui::InputFloat2("Position", &cPos.x);
	ImGui::InputFloat("Rotation", &cRot);
	auto frustum = m_Camera->GetZoomFrustum();
	ImGui::InputFloat("Left", &frustum.left);
	ImGui::InputFloat("Right", &frustum.right);
	ImGui::InputFloat("Bottom", &frustum.bottom);
	ImGui::InputFloat("Top", &frustum.top);
	ImGui::InputFloat("Near", &frustum.near);
	ImGui::InputFloat("Far", &frustum.far);
	ImGui::EndChild();
}

void Player::Damage(int damage)
{
	m_Health -= damage;
}
