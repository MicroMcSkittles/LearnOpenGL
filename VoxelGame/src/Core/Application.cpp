#include "Application.h"
#include "Core/Core.h"
#include "Core/System.h"
#include "Core/ImGuiHandler.h"
#include "Renderer/RenderAPI.h"

Application::Application() {
	if (s_Instance) {
		WARNING("Application instance already exists");
	}
	s_Instance = this;

#ifdef DEBUG
	m_Diagnostic = new ApplicationDiagnostic();
#endif 

	WindowConfig windowConfig;
	windowConfig.title = "Voxel Game";
	windowConfig.resizable = true;
	windowConfig.eventCallback = CLASS_BIND_ARGS_1(Application::EventCallback);
	m_Window = new Window(windowConfig);

	RenderAPI::Init();

	ImGuiConfig imGuiConfig;
	imGuiConfig.style = Style::Dark;
	ImGuiHandler::Init(imGuiConfig);

	m_GameInstance = new Game();

	m_Running = true;
}
Application::~Application() {
	delete(m_Window);
}

ApplicationDiagnostic* Application::GetDiagnostic()
{
#ifdef DEBUG
	return s_Instance->m_Diagnostic;
#elif RELEASE
	return new ApplicationDiagnostic();
#endif
}

void Application::EventCallback(Event::Event& e) {
	m_GameInstance->OnEvent(e);
}

void Application::Run() {

	float lastFrameTime = System::GetTime();

	m_GameInstance->StartUp();

	while (m_Running) {
		if (m_Window->ShouldClose()) break;

		float currentFrameTime = System::GetTime();
		float deltaTime = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;

#ifdef DEBUG
		m_Diagnostic->FPS = 1.0f / deltaTime;
		m_Diagnostic->MS  = deltaTime * 1000.0f;

		float lastTime = System::GetTime();
#endif

		m_GameInstance->Update(deltaTime);

#ifdef DEBUG
		m_Diagnostic->UpdateMS = System::GetTime() - lastTime;
		lastTime = System::GetTime();
#endif

		m_GameInstance->Render();

#ifdef DEBUG
		m_Diagnostic->RendererMS = System::GetTime() - lastTime;
		lastTime = System::GetTime();
#endif

		ImGuiHandler::StartFrame();

		m_GameInstance->ImGui();

		ImGuiHandler::EndFrame();

#ifdef DEBUG
		m_Diagnostic->ImGuiMS = System::GetTime() - lastTime;
#endif

		m_Window->Update();
	}

	m_GameInstance->ShutDown();
}