#pragma once
#include "Core/Event.h"
#include "Core/Input/InputEvent.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Camera.h"
#include "Core/JSON.h"

class Player {
public:
	Player(Camera* camera);

	void OnInput(Event::Event& e);
	void OnClick(Event::MouseButton& e);
	void OnScroll(Event::MouseScroll& e);
	void OnKey(Event::Keyboard& e);

	glm::vec3 FindNonCollision(const glm::vec3& position);
	void UpdateController(float deltaTime);
	void Update(float deltaTime);
	void ImGui();

	void Render();
	void RenderUI();

	void Save();

	glm::vec3 GetPosition() { return m_Position; }

private:
	Camera* m_Camera;

	std::string m_SavePath;
	DataTree m_Save;

	glm::vec3 m_Position;
	glm::vec3 m_Direction;
	glm::vec2 m_LastMousePosition;

	// The distance from the players feet to their eyes
	glm::vec3 m_EyeOffset;
	glm::vec3 m_ColliderSize;

	glm::vec3 m_TempPos;

	glm::vec3 m_Selector;
	int m_VoxelSelector;

	float m_InteractLength;
	float m_Speed;
	float m_MouseSensitivity;

	bool m_FocusLastFrame;
	bool m_HasFocus;
};