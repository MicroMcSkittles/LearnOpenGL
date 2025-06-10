#include "Player.h"
#include "Core/Input/KeyCodes.h"
#include "Core/Input/Input.h"
#include "Game/VoxelRenderer.h"
#include "Game/Voxel.h"
#include "Game/Game.h"

#include <imgui.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

Player::Player(Camera* camera)
{
	m_SavePath = Game::GetConfig()["Game"]["WorldSettings"]["SaveDirectory"].GetValue();
	m_SavePath.append("save.json");
	json::LoadFile(m_SavePath, &m_Save);
	m_Position = { 
		(float)m_Save["Player"]["Position"].At(0).GetValue(),
		(float)m_Save["Player"]["Position"].At(1).GetValue(),
		(float)m_Save["Player"]["Position"].At(2).GetValue()
	};
	m_VoxelSelector = (int)m_Save["Player"]["VoxelSelector"].GetValue();

	m_EyeOffset = { 0,1,0 };
	m_ColliderSize = { 0.75,1.9,0.75 };

	m_Selector = { 0.5,0.5,0.5 };
	m_LastMousePosition = { 0,0 };

	m_Camera = camera;
	m_Direction = camera->GetDirection();
	camera->SetPosition(m_Position + m_EyeOffset);

	m_HasFocus = false;
	m_FocusLastFrame = false;

	DataTree& playerSettings = Game::GetConfig()["Game"]["PlayerSettings"];
	m_InteractLength   = playerSettings["InteractLength"].GetValue();
	m_Speed			   = playerSettings["Speed"].GetValue();
	m_MouseSensitivity = playerSettings["MouseSensitivity"].GetValue();

}

void Player::OnInput(Event::Event& e)
{
	Event::EventHandler handler(e);
	handler.Dispatch<Event::MouseButton>(CLASS_BIND_ARGS_1(Player::OnClick));
	handler.Dispatch<Event::MouseScroll>(CLASS_BIND_ARGS_1(Player::OnScroll));
	handler.Dispatch<Event::Keyboard>(CLASS_BIND_ARGS_1(Player::OnKey));
}
void Player::OnClick(Event::MouseButton& e)
{
	if (e.GetAction() == PRESS) {
		glm::vec3 eye = m_Position + m_EyeOffset + glm::vec3(0.5,0.5,0.5);
		
		glm::vec3 voxel;
		glm::vec3 normal;
		if (!Game::GetWorld()->CastRay(eye, m_Direction, m_InteractLength, voxel, normal)) return;

		if (e.GetButton() == MOUSE_BUTTON_LEFT) Game::GetWorld()->SetVoxel(voxel, 0);
		else if (e.GetButton() == MOUSE_BUTTON_RIGHT) Game::GetWorld()->SetVoxel(voxel + normal, m_VoxelSelector);
		else if (e.GetButton() == MOUSE_BUTTON_MIDDLE) m_VoxelSelector = Game::GetWorld()->GetVoxel(voxel);
	}
}
void Player::OnScroll(Event::MouseScroll& e)
{
	m_VoxelSelector += e.GetY();
	if (m_VoxelSelector < 1) m_VoxelSelector = s_VoxelData->BlockCount - 1;
	else if (m_VoxelSelector > s_VoxelData->BlockCount - 1) m_VoxelSelector = 1;
}
void Player::OnKey(Event::Keyboard& e)
{
	if (e.GetKey() == KEY_TAB && e.GetAction() == PRESS) {
		m_HasFocus = !m_HasFocus;
	}
}

glm::vec3 Player::FindNonCollision(const glm::vec3& position)
{
	std::vector<glm::vec3> voxels;
	if (!Game::GetWorld()->Collide(position, m_ColliderSize, voxels)) {
		return position;
	}
	


	return position;
}
void Player::UpdateController(float deltaTime)
{
	// Find direction vectors
	constexpr glm::vec3 upDirection(0.0f, 1.0f, 0.0f);
	glm::vec3 forwardDirection = glm::normalize(glm::vec3(m_Direction.x, 0, m_Direction.z));
	glm::vec3 rightDirection = glm::cross(forwardDirection, upDirection);

	// Process position inputs 
	bool moved = false;
	glm::vec3 newPosition = m_Position;

	glm::vec3 movementDirection = { 0,0,0 };
	glm::vec3 collitionDirection = { 0,0,0 };

	if (Input::KeyPressed(KEY_W)) {
		movementDirection += forwardDirection;

		moved = true;
	}
	if (Input::KeyPressed(KEY_S)) {
		movementDirection -= forwardDirection;
		moved = true;
	}
	if (Input::KeyPressed(KEY_D)) {
		movementDirection += rightDirection;
		moved = true;
	}
	if (Input::KeyPressed(KEY_A)) {
		movementDirection -= rightDirection;
		moved = true;
	}
	if (Input::KeyPressed(KEY_E)) {
		movementDirection += upDirection;
		moved = true;
	}
	if (Input::KeyPressed(KEY_Q)) {
		movementDirection -= upDirection;
		moved = true;
	}
	movementDirection = glm::normalize(movementDirection);

	std::vector<glm::vec3> voxels;
	if (moved) {
		newPosition = m_Position + movementDirection * (m_Speed * deltaTime);
		//m_Position = FindNonCollision(newPosition);
		m_Position = newPosition;
		m_Camera->SetPosition(m_Position + m_EyeOffset);
	}
	else {
		moved = false;
	}


	if (!m_FocusLastFrame) {
		m_LastMousePosition = Input::GetMousePos();
		m_FocusLastFrame = true;
	}
	glm::vec2 mousePos = Input::GetMousePos();
	glm::vec2 delta = (mousePos - m_LastMousePosition) * 0.002f;
	m_LastMousePosition = mousePos;

	// Process direction input
	if (delta.x != 0 || delta.y != 0) {
		float pitchDelta = delta.y * m_MouseSensitivity;
		float yawDelta = delta.x * m_MouseSensitivity;

		glm::quat q = glm::normalize(glm::cross(glm::angleAxis(-pitchDelta, rightDirection), glm::angleAxis(-yawDelta, upDirection)));
		m_Direction = glm::rotate(q, m_Direction);

		m_Camera->SetDirection(m_Direction);
	}
}

void Player::Update(float deltaTime)
{
	// Press tab to toggle player focus
	if (!m_HasFocus) {
		Input::HideCursor(false);
		m_FocusLastFrame = false;
		return;
	}
	Input::HideCursor(true);

	UpdateController(deltaTime);

	glm::vec3 eye = m_Position + m_EyeOffset + glm::vec3(0.5, 0.5, 0.5);
	glm::vec3 voxel, normal;
	if (Game::GetWorld()->CastRay(eye, m_Direction, m_InteractLength, voxel, normal)) {
		m_Selector = voxel;
	}
	// It's impossable for CastRay to return a decimal, so I set the
	// m_Selector to a decimal to tell if it's valid or not
	else m_Selector = { 0.5,0.5,0.5 };

	// TODO: add a totaly hip and cool and hip chunk debug view
}

void Player::Render() {
	if (m_Selector != glm::vec3(0.5, 0.5, 0.5))
		VoxelRenderer::DrawSelector(m_Selector);
}
void Player::RenderUI()
{
	VoxelRenderer::DrawUIElement({ 0, 0 }, AlignMiddle, { 0.25, 0.25 }, { 0,0 });
	VoxelRenderer::DrawUIVoxelPreview({ -1, -1 }, AlignTop | AlignRight, { 0.5, 0.5 }, m_VoxelSelector);
}

void Player::Save()
{
	m_Save["Player"]["Position"].At(0) = m_Position.x;
	m_Save["Player"]["Position"].At(1) = m_Position.y;
	m_Save["Player"]["Position"].At(2) = m_Position.z;
	m_Save["Player"]["VoxelSelector"] = m_VoxelSelector;

	json::Serialize(m_SavePath, &m_Save);
}

void Player::ImGui()
{
	if (!ImGui::TreeNode("Player")) return;

	auto& settings = VoxelRenderer::GetSettings();
	glm::vec2 chunkCoord = {
		floor(m_Position.x / settings.chunkSize),
		floor(m_Position.z / settings.chunkSize)
	};

	glm::vec2 viewChunkCoord = {
		settings.renderDistance,
		settings.renderDistance
	};

	glm::vec3 subCoord = Game::GetWorld()->GetSubChunkPosition(m_Position);

	ImGui::InputFloat3("World Position", &m_Position.x);
	ImGui::InputFloat2("Chunk Position", &chunkCoord.x);
	ImGui::InputFloat3("Sub Chunk Position", &subCoord.x);
	ImGui::InputFloat2("View Chunk Position", &viewChunkCoord.x);
	ImGui::Separator();
	ImGui::InputFloat3("Position", &m_TempPos.x);

	ImGui::InputInt("VoxelID", &m_VoxelSelector);

	if (m_VoxelSelector < 1) m_VoxelSelector = 1;
	else if (m_VoxelSelector > s_VoxelData->BlockCount) m_VoxelSelector = s_VoxelData->BlockCount - 1;

	ImGui::TreePop();
}