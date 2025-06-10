#include "Game/Game.h"
#include "Renderer/RenderAPI.h"
#include "Core/Input/Input.h"
#include "Core/Window/Window.h"
#include "Core/Application.h"
#include "Core/ImGuiHandler.h"

#include "Game/VoxelRenderer.h"
#include "Game/Voxel.h"

#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>

void Game::StartUp() {
	if (s_Instance) {
		WARNING("Game instance alread exists");
	}
	s_Instance = this;

	json::LoadFile("userdata/config.json", &m_UserConfig);

	VoxelRenderer::Init();

	auto& settings = VoxelRenderer::GetSettings();

	// Init Camera
	m_MainCamera = new PerspectiveCamera(Frustum::ScreenFrustum(), 
		{ settings.chunkSize / 2.0f,16.0f,settings.chunkSize / 2.0f }, 
		{ 1, 0, 0 });
	m_UICamera = new OrthographicCamera(ViewBox::ScreenViewBox(), 4);

	m_Player = new Player(m_MainCamera);

	m_World = new World(m_Player);
	m_World->LoadWorld();
}
void Game::ShutDown() {
	delete(m_Player);
	delete(m_World);
	delete(m_MainCamera);
}

void Game::OnEvent(Event::Event& e) {
	Event::EventHandler handler(e);
	handler.Dispatch<Event::WindowResize>(CLASS_BIND_ARGS_1(Game::OnWindowResize));
	if (e.GetCategory() & Event::EventCategory::Input) m_Player->OnInput(e);
}
void Game::OnWindowResize(Event::WindowResize& e) {
	VoxelRenderer::Resize(e.GetWidth(), e.GetHeight());
	
}

void Game::Update(float deltaTime) {
	if (Input::KeyPressed(KEY_ESCAPE)) {
		Window::Get()->ShouldClose(true);
	}

	m_Player->Update(deltaTime);
	m_World->Update(deltaTime);
}

void Game::Render() {
	
	VoxelRenderer::StartFrame(m_MainCamera);

	m_World->Render();
	m_Player->Render();

	VoxelRenderer::StartUIFrame(m_UICamera);

	m_Player->RenderUI();

	VoxelRenderer::EndFrame();
}

void Game::ImGui() {
	ImGui::Begin("Debug Window");

	if (ImGui::TreeNode("Diagnostic Info")) {

		auto renderInfo = RenderAPI::GetDiagnostic();
		auto appInfo = Application::GetDiagnostic();
		auto imguiInfo = ImGuiHandler::GetDiagnostic();

		ImGui::Text("Draw Call Count: %i", renderInfo->DrawCallCount);
		ImGui::Text("Clear Count: %i", renderInfo->ClearCount);
		ImGui::Text("FPS: %f", appInfo->FPS);
		ImGui::Text("MS: %f", appInfo->MS);
		ImGui::Text("Update MS: %f", appInfo->UpdateMS);
		ImGui::Text("Renderer MS: %f", appInfo->RendererMS);
		ImGui::Text("ImGui MS: %f", appInfo->ImGuiMS);

		ImGui::TreePop();
	}

	m_Player->ImGui();

	json::ImGuiInputDataTree(&m_UserConfig, "User Config");

	if (ImGui::Button("Save Game")) {
		m_World->SaveWorld();
		m_Player->Save();
	}

	ImGui::Text("Press Tab to focus");
	ImGui::Text("The config.json file has most of the settings");
	ImGui::Text("Don't minamize it will crash (idk why)");

	ImGui::End();
}