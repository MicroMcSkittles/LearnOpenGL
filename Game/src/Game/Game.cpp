#include "Game.h"
#include <iostream>
#include <map>
#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Core/Application.h"
#include "Renderer/Renderer.h"
#include "FontRenderer.h"
#include "Core/Input.h"
#include "Player.h"
#include "Background.h"
#include "Bullet.h"
#include "Enemy.h"
#include "Partical.h"

namespace {
	enum class GameState
	{
		None,
		MainMenu,
		Game,
		Death
	};
	GameState s_LastGameState;
	GameState s_CurrentGameState;

	struct Button {
		bool isHovered = false;
		bool isPressed = false;
		Texture* texture;
		std::string text;
		glm::vec2 position;
		glm::vec2 textPosition;
		glm::vec2 scale;

		void Update() {
			glm::vec2 cursor = {
				Input::GetMouseX(),
				Input::GetMouseY()
			};

			if (cursor.x > position.x && cursor.x < scale.x && cursor.y > position.y && cursor.y < scale.y) {
				isHovered = true;
				if (Input::IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
					isPressed = true;
				}
				else isPressed = false;
			}
			else {
				isHovered = false;
				isPressed = false;
			}
		}

		void Render(Camera* camera) {
			glm::vec4 tint = { 1, 1, 1, 1 };
			int id = 0;
			if (isHovered) { tint *= 0.75f; id = 1; }

			Renderer::DrawScreenSpaceQuadAtlas(position, scale, 0.0f, texture, { 2, 1 }, { id,0 }, false, tint);
			//Renderer::DrawScreenSpaceQuad(position, scale, 0, tint);
			FontRenderer::DrawText(
				camera->ScreenToWorld(textPosition),
				{ 0.1f,0.1f }, 0.0f,
				text
			);
		}
	};
	Button s_StartButton;
	Button s_OptionsButton;
	Button s_ExitButton;

	Texture* s_ShipAtlas;
	Texture* s_ProjectileAtlas;
	Texture* s_ButtonAtlas;
	Texture* s_ShipDebrisAtlas;
	Texture* s_TitleTexture;
	Texture* s_ArrowTexture;
	Texture* s_ExplosionAnimation;

	Player* s_Player;
	Camera* s_MainCamera;
	Camera* s_MenuCamera;

	struct EnemyArrow {
		glm::vec2 direction;

		void Render() {
			float rot = atan2(-direction.x, direction.y);
			glm::vec2 position = s_Player->GetPosition() + (direction * 0.16f);
			Renderer::DrawQuad(position, { 0.25f,0.25f }, rot, s_ArrowTexture);
		}
	};
	std::vector<EnemyArrow> s_EnemyArrows;

	std::vector<Partical*> s_Particals;

	Background* s_BackgroundDust;
	Background* s_BackgroundNebulae;
	Background* s_BackgroundPlanets;
	Background* s_BackgroundStars;

	std::vector<Bullet> s_EnemyBullets;
	std::vector<Bullet> s_PlayerBullets;
	std::vector<Enemy> s_Enemies;

	EnemySpawner* s_EnemySpawner;
	bool s_EnableEnemySpawner;
	int s_KillCount;

	void StartGame() {
		s_EnemyBullets.clear();
		s_PlayerBullets.clear();
		s_Enemies.clear();
		s_EnemyArrows.clear();
		s_Particals.clear();

		s_KillCount = 0;
		s_EnableEnemySpawner = true;

		//s_MainCamera->SetPosition({ 0,0 });
		s_MainCamera->SetRotation(0);
		s_Player = new Player(s_ShipAtlas, s_MainCamera);
		s_EnemySpawner = new EnemySpawner(s_Player, s_ShipAtlas, &s_Enemies);
	}
	void UpdateGame(float deltaTime) {
		s_Player->OnUpdate(&s_EnemyBullets, deltaTime);
		if (s_Player->GetHealth() <= 0) s_CurrentGameState = GameState::Death;

		if(s_EnableEnemySpawner) s_EnemySpawner->OnUpdate(deltaTime);

		for (int i = 0; i < s_EnemyBullets.size(); ++i) {
			if (s_EnemyBullets[i].ShouldKill()) {
				s_EnemyBullets.erase(s_EnemyBullets.begin() + i);
				i -= 1;
				continue;
			}
			s_EnemyBullets[i].OnUpdate(deltaTime);
		}
		for (int i = 0; i < s_PlayerBullets.size(); ++i) {
			if (s_PlayerBullets[i].ShouldKill()) {
				s_PlayerBullets.erase(s_PlayerBullets.begin() + i);
				i -= 1;
				continue;
			}
			s_PlayerBullets[i].OnUpdate(deltaTime);
		}
		
		for (int i = 0; i < s_Particals.size(); ++i) {
			if (s_Particals[i]->OnUpdate(deltaTime)) {
				s_Particals.erase(s_Particals.begin() + i);
				i -= 1;
			}
		}

		if (s_EnemyArrows.size() != s_Enemies.size()) {
			s_EnemyArrows.resize(s_Enemies.size());
		}
		for (int i = 0; i < s_Enemies.size(); ++i) {
			Enemy& e = s_Enemies[i];

			s_EnemyArrows[i].direction = glm::normalize(e.GetPosition() - s_Player->GetPosition());

			for (int j = 0; j < s_PlayerBullets.size(); ++j) {
				Bullet& b = s_PlayerBullets[j];
				if (glm::distance(b.GetPosition(), e.GetPosition()) <= (0.25f / 2.0f)) {
					s_PlayerBullets.erase(s_PlayerBullets.begin() + j);
					j -= 1;
					e.Damage(1.0f);
				}
			}
			
			if (e.ShouldKill()) {
				Explosion* explosion = new Explosion(e.GetPosition(), e.GetType(), s_ExplosionAnimation, s_ShipDebrisAtlas, &s_Particals);
				s_Particals.push_back(explosion);

				s_Enemies.erase(s_Enemies.begin() + i);
				s_EnemyArrows.erase(s_EnemyArrows.begin() + i);
				i -= 1;
				s_KillCount += 1;
				s_EnemySpawner->GetKillCount() += 1;
				continue;
			}
			if (e.OnUpdate(deltaTime)) {
				glm::vec2 dir = glm::normalize(s_Player->GetPosition() - e.GetPosition());
				dir = { dir.x, -dir.y };
				Bullet b(
					e.GetPosition(),
					dir,
					{ 0,0 },
					s_ProjectileAtlas
				);
				s_EnemyBullets.push_back(b);
			}
		}
	}
	void RenderGame() {
		if (!s_EnemySpawner) return;

		s_BackgroundDust->Render(s_Player->GetPosition());
		s_BackgroundNebulae->Render(s_Player->GetPosition());
		s_BackgroundStars->Render(s_Player->GetPosition());
		s_BackgroundPlanets->Render(s_Player->GetPosition());

		for (auto& b : s_EnemyBullets) {
			b.Render();
		}
		for (auto& b : s_PlayerBullets) {
			b.Render();
		}

		for (auto& p : s_Particals) {
			p->Render();
		}

		for (auto& e : s_Enemies) {
			e.Render();
		}

		for (auto& a : s_EnemyArrows) {
			a.Render();
		}

		s_Player->Render();

		Renderer::ChangeCamera(s_MenuCamera);

		std::stringstream hud;
		hud << "Wave " << s_EnemySpawner->GetWave() << std::endl;
		hud << "Kills " << s_KillCount;

		auto window = Application::Get()->GetWindow();
		float y = FontRenderer::GetTextHeight(0.1f, hud.str());
		y = s_MenuCamera->ScreenSpacePos({ 0, -y }).y - (window->GetHeight() / 2);
		y = window->GetHeight() - y;

		FontRenderer::DrawText(s_MenuCamera->ScreenToWorld({ 25, y }), { 0.1f,0.1f }, 0, hud.str());
	}

	float s_MenuTime;

	void StartMainMenu() {
		s_MainCamera->SetPosition({ 0,0 });
		s_MainCamera->SetRotation(0);
		s_StartButton.texture = s_ButtonAtlas;
		s_OptionsButton.texture = s_ButtonAtlas;
		s_ExitButton.texture = s_ButtonAtlas;

		s_StartButton.textPosition = s_MenuCamera->ScreenSpacePos({ 
			-0.632f,  
			0.198f - (0.198f / 8) 
		});
		s_StartButton.text = "Start";

		s_OptionsButton.textPosition = s_MenuCamera->ScreenSpacePos({ -0.632f,  -0.100f - (-0.100f + 0.298f) / 8 });
		s_OptionsButton.text = "Options";

		s_ExitButton.textPosition = s_MenuCamera->ScreenSpacePos({ -0.632f,  -0.398f });
		s_ExitButton.text = "Exit";

		s_StartButton.position =   s_MenuCamera->ScreenSpacePos({ -0.632f,  0.198f });
		s_StartButton.scale =      s_MenuCamera->ScreenSpacePos({  0.632f,  0.000f });
		s_OptionsButton.position = s_MenuCamera->ScreenSpacePos({ -0.632f, -0.100f });
		s_OptionsButton.scale =    s_MenuCamera->ScreenSpacePos({  0.632f, -0.298f });
		s_ExitButton.position =    s_MenuCamera->ScreenSpacePos({ -0.632f, -0.398f });
		s_ExitButton.scale =       s_MenuCamera->ScreenSpacePos({  0.632f, -0.596f });

		s_MenuTime = 0;
	}
	void UpdateMainMenu(float deltaTime) {
		s_MenuTime += deltaTime;

		glm::vec2 cameraPos = s_MainCamera->GetPosition();
		glm::vec2 dir = { 0.5, 0.5 };
		dir = glm::normalize(dir);

		cameraPos += dir * (0.125f * deltaTime);
		s_MainCamera->SetPosition(cameraPos);

		s_StartButton.Update();
		s_OptionsButton.Update();
		s_ExitButton.Update();

		if (s_StartButton.isPressed) {
			s_CurrentGameState = GameState::Game;
		}

		if (s_ExitButton.isPressed) {
			WindowCloseEvent e;
			Application::Get()->ProcEvent(e);
			return;
		}
	}
	void RenderMainMenu() {
		s_BackgroundDust->Render(s_MainCamera->GetPosition());
		s_BackgroundNebulae->Render(s_MainCamera->GetPosition());
		s_BackgroundStars->Render(s_MainCamera->GetPosition());
		s_BackgroundPlanets->Render(s_MainCamera->GetPosition());

		Renderer::ChangeCamera(s_MenuCamera);
		FontRenderer::SetCamera(s_MenuCamera);

		float scale = sin(s_MenuTime / 2.0f) / 36.0f;
		Renderer::DrawScreenSpaceQuad(
			s_MenuCamera->ScreenSpacePos({ -0.632f - scale, 0.782f + scale }),
			s_MenuCamera->ScreenSpacePos({ 0.632f + scale, 0.298f - scale }),	
			glm::radians(cos(s_MenuTime)),
			s_TitleTexture
		);

		s_StartButton.Render(s_MenuCamera);
		s_OptionsButton.Render(s_MenuCamera);
		s_ExitButton.Render(s_MenuCamera);
	}

	void StartDeath() {

	}
	void UpdateDeath(float deltaTime) {
		exit(-1);
	}
	void RenderDeath() {

	}

	void Reset() {
		s_EnemyBullets.clear();
		s_PlayerBullets.clear();
		s_Enemies.clear();

		s_MainCamera->SetPosition({ 0,0 });
		s_MainCamera->SetRotation(0);
		s_Player = new Player(s_ShipAtlas, s_MainCamera);
	}
};

#define BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

void Game::OnEvent(Event& e) {
	EventHandler eh(e);
	eh.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Game::OnResize));
	eh.Dispatch<MouseButtonEvent>(BIND_EVENT_FN(Game::OnMouseButton));
}

void Game::OnMouseButton(MouseButtonEvent& e)
{
	if (s_CurrentGameState != GameState::Game) return;
	if (e.GetButton() != MOUSE_BUTTON_LEFT || e.GetAction() != PRESS) return;

	Bullet bullet(
		s_Player->GetPosition(),
		s_Player->GetDirection(),
		{ 0,1 },
		s_ProjectileAtlas
	);
	s_PlayerBullets.push_back(bullet);
}

void Game::OnResize(WindowResizeEvent& e)
{
	ViewFrustum frustum = ViewFrustum::CalculateScreenFrustum();
	s_MainCamera->SetViewFrustum(frustum);
	s_MenuCamera->SetViewFrustum(frustum);

	s_StartButton.textPosition   = s_MenuCamera->ScreenSpacePos({ -0.632f,  0.198f / 2.0f });
	s_OptionsButton.textPosition = s_MenuCamera->ScreenSpacePos({ -0.632f,  -0.100f });
	s_ExitButton.textPosition    = s_MenuCamera->ScreenSpacePos({ -0.632f,  -0.398f });

	s_StartButton.position       = s_MenuCamera->ScreenSpacePos({ -0.632f,  0.198f });
	s_StartButton.scale          = s_MenuCamera->ScreenSpacePos({  0.632f,  0.000f });
	s_OptionsButton.position     = s_MenuCamera->ScreenSpacePos({ -0.632f, -0.100f });
	s_OptionsButton.scale        = s_MenuCamera->ScreenSpacePos({  0.632f, -0.298f });
	s_ExitButton.position        = s_MenuCamera->ScreenSpacePos({ -0.632f, -0.398f });
	s_ExitButton.scale           = s_MenuCamera->ScreenSpacePos({  0.632f, -0.596f });
}

void Game::StartUp() {
	ViewFrustum frustum = ViewFrustum::CalculateScreenFrustum();
	s_MainCamera = new Camera(frustum, {0,0}, 1.5f);
	s_MenuCamera = new Camera(frustum);

	FontRenderer::Init();

	TextureConfig config;
	config.MinFilter = TexFilterMode::Nearest;
	config.MagFilter = TexFilterMode::Nearest;
	config.FlipY = true;
	s_ShipAtlas = new Texture("assets/textures/spaceships.png", config);
	s_ProjectileAtlas = new Texture("assets/textures/projectiles.png", config);
	s_ShipDebrisAtlas = new Texture("assets/textures/ShipDebris.png", config);
	s_ExplosionAnimation = new Texture("assets/textures/ExplosionSpritesheet.png", config);

	config.FlipY = false;
	s_ButtonAtlas = new Texture("assets/textures/menu_button.png", config);
	s_TitleTexture = new Texture("assets/textures/Title.png", config);
	s_ArrowTexture = new Texture("assets/textures/Arrow.png", config);

	s_BackgroundDust =    new Background("assets/textures/SpaceBackgroundDust.png", 0, s_MainCamera);
	s_BackgroundNebulae = new Background("assets/textures/SpaceBackgroundNebulae.png", 0.03, s_MainCamera);
	s_BackgroundStars =   new Background("assets/textures/SpaceBackgroundStars.png", 0.05, s_MainCamera);
	s_BackgroundPlanets = new Background("assets/textures/SpaceBackgroundPlanets.png", 0.07, s_MainCamera);

	s_Player = new Player(s_ShipAtlas, s_MainCamera);

	s_LastGameState = GameState::None;
	s_CurrentGameState = GameState::MainMenu;
}
void Game::Shutdown() {

}

void Game::Update(float deltaTime) {
	if (s_LastGameState != s_CurrentGameState) {
		if (s_CurrentGameState == GameState::Game) StartGame();
		else if (s_CurrentGameState == GameState::Death) StartDeath();
		else if (s_CurrentGameState == GameState::MainMenu) StartMainMenu();
	}
	s_LastGameState = s_CurrentGameState;

	if (s_CurrentGameState == GameState::Game) UpdateGame(deltaTime);
	else if (s_CurrentGameState == GameState::Death) UpdateDeath(deltaTime);
	else if (s_CurrentGameState == GameState::MainMenu) UpdateMainMenu(deltaTime);
}

void Game::Render() {
	Renderer::StartFrame(s_MainCamera);
	Renderer::ClearColor({ 0.0f, 0.0f, 0.0f });

	if (s_CurrentGameState == GameState::Game) RenderGame();
	else if (s_CurrentGameState == GameState::Death) RenderDeath();
	else if (s_CurrentGameState == GameState::MainMenu) RenderMainMenu();

	Renderer::EndFrame();
}

void Game::OnImGui() {
	ImGui::Begin("Debug Window");
	const auto& app_info = Application::Get()->GetDiagnosticInfo();
	const auto& render_info = Renderer::GetDiagnostic();

	ImGui::SeparatorText("Application Diagnostic");
	ImGui::Text("FPS: %f", app_info.fps);
	ImGui::Text("MS: %f", app_info.frameTimeMS);
	ImGui::Text("Game Update: %f ~ (%i%%)", app_info.updateMS, (int)((app_info.updateMS / app_info.frameTimeMS) * 100));
	ImGui::Text("Render Update: %f ~ (%i%%)", app_info.renderMS, (int)((app_info.renderMS / app_info.frameTimeMS) * 100));
	ImGui::Text("ImGui Update: %f ~ (%i%%)", app_info.imguiMS, (int)((app_info.imguiMS / app_info.frameTimeMS) * 100));
	
	ImGui::SeparatorText("Renderer Diagnostic");
	ImGui::Text("Render MS: %f", app_info.renderMS);
	ImGui::Text("Draw Call MS: %f", render_info.DrawMS);
	ImGui::Text("Batching MS: %f", render_info.BatchMS);
	ImGui::Text("Draw Call Count: %i", render_info.DrawCallCount);
	ImGui::Text("Render Group Count: %i", render_info.RenderGroupCount);
	ImGui::Text("Quad Count: %i", render_info.QuadCount);

	ImGui::SeparatorText("Game Info");
	std::map<GameState, std::string> StateToStr = {
		{ GameState::MainMenu, "Main Menu" },
		{ GameState::Game, "Game" },
		{ GameState::Death, "Death" },
	};

	ImGui::Text("Bullet Count: %i", s_PlayerBullets.size() + s_EnemyBullets.size());
	ImGui::Text("Enemy Count: %i", s_Enemies.size());
	ImGui::Text("Partical Count: %i", s_Particals.size());
	ImGui::Text(std::string("Current Game State ( " + StateToStr[s_CurrentGameState] + " )").c_str());
	
	const char* items[] = { "Main Menu", "Game", "Death" };
	const char* current_item = StateToStr[s_CurrentGameState].c_str();

	if (ImGui::BeginCombo("##combo", current_item)) // The second parameter is the label previewed before opening the combo.
	{
		for (int n = 0; n < IM_ARRAYSIZE(items); n++)
		{
			bool is_selected = (current_item == items[n]); // You can store your selection however you want, outside or inside your objects
			if (ImGui::Selectable(items[n], is_selected))
				s_CurrentGameState = (GameState)(n + 1);
				if (is_selected)
					ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
		}
		ImGui::EndCombo();
	}

	if (s_Player) {
		ImGui::SeparatorText("Player");
		s_Player->OnImGui();
	}

	ImGui::SeparatorText("Settings");
	if (ImGui::Button("Reset")) {
		Reset();
	}

	static int count;
	if (ImGui::Button("Spawn Enemy")) {
		for (int i = 0; i < count; ++i) {
			Enemy e(
				s_Player->GetPosition(),
				s_Player->GetDirection(),
				s_Player,
				s_ShipAtlas
			);
			s_Enemies.push_back(e);
		}
	}
	ImGui::SameLine();
	ImGui::InputInt("Count", &count);
	ImGui::Checkbox("Enable Enemy Spawner", &s_EnableEnemySpawner);

	auto window = Application::Get()->GetWindow();
	bool temp = window->GetVSync();
	ImGui::Checkbox("VSync", &temp);
	if (temp != window->GetVSync()) window->SetVSync(temp);

	ImGui::SeparatorText("Misc");
	glm::vec2 cursor = {
			Input::GetMouseX(),
			Input::GetMouseY()
	};
	glm::vec2 WS_MC_MousePos = s_MenuCamera->ScreenToWorld(cursor);
	glm::vec2 WS_C_MousePos = s_MainCamera->ScreenToWorld(cursor);
	ImGui::InputFloat2("Mouse Position", &cursor.x);
	ImGui::InputFloat2("World Space Mouse", &WS_C_MousePos.x);
	ImGui::InputFloat2("UI Space Mouse", &WS_MC_MousePos.x);

	ImGui::End();
}