#include "Game.h"
#include <iostream>
#include <imgui.h>
#include "Core/Application.h"
#include "Renderer/Renderer.h"
#include "Core/Input.h"
#include "Player.h"
#include "Background.h"
#include "Bullet.h"

namespace {
	Texture* s_ShipAtlas;
	Texture* s_ProjectileAtlas;

	Player* s_Player;
	Background* s_BackgroundDust;
	Background* s_BackgroundNebulae;
	Background* s_BackgroundPlanets;
	Background* s_BackgroundStars;

	std::vector<Bullet> s_Bullets;
};

#define BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

void Game::OnEvent(Event& e) {
	EventHandler eh(e);
	eh.Dispatch<MouseButtonEvent>(BIND_EVENT_FN(Game::OnMouseButton));
}

void Game::OnMouseButton(MouseButtonEvent& e)
{
	if (e.GetButton() != MOUSE_BUTTON_LEFT || e.GetAction() != PRESS) return;

	Bullet bullet(
		s_Player->GetPosition(),
		s_Player->GetDirection(),
		s_ProjectileAtlas
	);
	s_Bullets.push_back(bullet);
}

void Game::StartUp() {
	ViewFrustum frustum = ViewFrustum::CalculateScreenFrustum();
	m_MainCamera = new Camera(frustum, {0,0}, 1.5f);

	TextureConfig config;
	config.MinFilter = TexFilterMode::Nearest;
	config.MagFilter = TexFilterMode::Nearest;
	config.FlipY = true;
	s_ShipAtlas = new Texture("assets/textures/spaceships.png", config);
	s_ProjectileAtlas = new Texture("assets/textures/projectiles.png", config);

	s_Player = new Player(s_ShipAtlas, m_MainCamera);

	s_BackgroundDust =    new Background("assets/textures/SpaceBackgroundDust.png", 0, m_MainCamera, s_Player);
	s_BackgroundNebulae = new Background("assets/textures/SpaceBackgroundNebulae.png", 0.03, m_MainCamera, s_Player);
	s_BackgroundStars =   new Background("assets/textures/SpaceBackgroundStars.png", 0.05, m_MainCamera, s_Player);
	s_BackgroundPlanets = new Background("assets/textures/SpaceBackgroundPlanets.png", 0.07, m_MainCamera, s_Player);

	Renderer::ClearColor({ 0.0f, 0.0f, 0.0f });
}
void Game::Shutdown() {

}

void Game::Update(float deltaTime) {
	s_Player->OnUpdate(deltaTime);

	for (int i = 0; i < s_Bullets.size(); ++i) {
		if (s_Bullets[i].ShouldKill()) {
			s_Bullets.erase(s_Bullets.begin() + i);
			i -= 1;
			continue;
		}
		s_Bullets[i].OnUpdate(deltaTime);
	}
}

void Game::Render() {
	Renderer::StartFrame(m_MainCamera);
	
	s_BackgroundDust->Render();
	s_BackgroundNebulae->Render();
	s_BackgroundStars->Render();
	s_BackgroundPlanets->Render();

	for (auto& b : s_Bullets) {
		b.Render();
	}

	s_Player->Render();

	Renderer::EndFrame();
}

void Game::OnImGui() {
	ImGui::Begin("Debug Window");
	auto info = Application::Get()->GetDiagnosticInfo();

	ImGui::SeparatorText("Application Diagnostic");
	ImGui::Text("FPS: %f", info.fps);
	ImGui::Text("MS: %f", info.frameTimeMS);
	ImGui::Text("Game Update: %f ~ (%i%%)", info.updateMS, (int)((info.updateMS / info.frameTimeMS) * 100));
	ImGui::Text("Render Update: %f ~ (%i%%)", info.renderMS, (int)((info.renderMS / info.frameTimeMS) * 100));
	ImGui::Text("ImGui Update: %f ~ (%i%%)", info.imguiMS, (int)((info.imguiMS / info.frameTimeMS) * 100));
	
	ImGui::SeparatorText("Game Info");
	ImGui::Text("Bullet Count: %i", s_Bullets.size());
	ImGui::End();
}