#pragma once
#include "Core/Event.h"
#include "Core/Window/WindowEvent.h"
#include "Core/JSON.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Shader.h"
#include "Renderer/Camera.h"
#include "Renderer/Texture.h"

#include "Game/Player.h"
#include "Game/World.h"

class Game {
public:
	Game() { }

	static World* GetWorld() { return s_Instance->m_World; }
	static DataTree& GetConfig() { return s_Instance->m_UserConfig; }

	void StartUp();
	void ShutDown();
	
	void OnEvent(Event::Event& e);
	void OnWindowResize(Event::WindowResize& e);

	void Update(float deltaTime);
	void Render();
	void ImGui();

private:
	World* m_World;

	Camera* m_MainCamera;
	Camera* m_UICamera;
	Player* m_Player;

	DataTree m_UserConfig;

private:
	inline static Game* s_Instance;
};