#include "Game.h"
#include <iostream>
#include "Renderer/Renderer.h"
#include "Core/Input.h"

void Game::OnEvent(Event& e) {
	std::cout << e.ToString() << std::endl;
}

void Game::StartUp() {
	ViewFrustum frustum = ViewFrustum::CalculateScreenFrustum();
	m_MainCamera = new Camera(frustum);

	m_Texture = new Texture("assets/textures/test.jpg");
	m_Time = 0;

	Renderer::ClearColor({ 0.2f, 0.3f, 0.3f });
}
void Game::Shutdown() {

}

void Game::Update(float deltaTime) {
	m_Time += 50 * deltaTime;

	glm::vec2 pos = m_MainCamera->GetPosition();
	if (Input::IsKeyPressed(KEY_W)) pos.y += 3 * deltaTime;
	if (Input::IsKeyPressed(KEY_S)) pos.y -= 3 * deltaTime;
	if (Input::IsKeyPressed(KEY_A)) pos.x -= 3 * deltaTime;
	if (Input::IsKeyPressed(KEY_D)) pos.x += 3 * deltaTime;

	m_MainCamera->SetPosition(pos);
}

void Game::Render() {
	Renderer::StartFrame(m_MainCamera);

	float i = sin(m_Time / 50);
	Renderer::DrawQuad({ i, 0 }, { i,0.5 }, glm::radians(m_Time), { 1.0f, 0.5f, 0.2f });
	if (i < 0.1) i = 0.1;
	Renderer::DrawQuad({ 1, 0 }, {0.5 * i, 0.5 * i }, glm::radians(-m_Time), m_Texture, { 1.0f, 1.0f, 1.0f });
	
	Renderer::EndFrame();
}