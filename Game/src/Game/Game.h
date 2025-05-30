#pragma once
#include <glm/glm.hpp>
#include "Renderer/Texture.h"
#include "Renderer/Camera.h"
#include "Core/Event/Event.h"
#include "Core/Event/WindowEvent.h"
#include "Core/Event/InputEvent.h"

class Game {
public:
	Game() { }

	void OnEvent(Event& e);
	void OnMouseButton(MouseButtonEvent& e);
	void OnResize(WindowResizeEvent& e);

	void StartUp();
	void Shutdown();

	void Update(float deltaTime);
	void Render();
	void OnImGui();

private:
	
};