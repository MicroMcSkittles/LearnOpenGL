#pragma once
#include <glm/glm.hpp>
#include "Renderer/Texture.h"
#include "Renderer/Camera.h"
#include "Core/Event/Event.h"

class Game {
public:
	Game() { }

	void OnEvent(Event& e);

	void StartUp();
	void Shutdown();

	void Update(float deltaTime);
	void Render();

private:
	Camera* m_MainCamera;
	Texture* m_Texture;

	float m_Time;
};