#include "Application.h"
#include <iostream>
#include "Renderer/Renderer.h"
#include "System.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#define BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

Application::Application()
{
	if (s_App) {
		std::cout << "Application already exists" << std::endl;
		m_IsRunning = false;
		return;
	}
	s_App = this;

	WindowConfig config;
	config.resizable = true;
	config.vSync = false;
	config.title = "Test Window";
	m_Window = new Window(BIND_EVENT_FN(Application::ProcEvent), config);

	Renderer::Init(BIND_EVENT_FN(Application::ProcEvent));

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	auto window = (GLFWwindow*)m_Window->GetHandle();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");

	m_Game = new Game();

	m_IsRunning = true;
}
Application::~Application()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	delete(m_Window);
	delete(m_Game);
}

void Application::ProcEvent(Event& e)
{
	EventHandler eh(e);
	eh.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnResize));
	eh.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnClose));
	eh.Dispatch<ErrorEvent>(BIND_EVENT_FN(Application::OnError));

	m_Game->OnEvent(e);
}
void Application::OnClose(WindowCloseEvent& e)
{
	m_IsRunning = false;
}
void Application::OnError(ErrorEvent& e)
{
	std::cout << e.ToString() << std::endl;
	exit(-1);
}
void Application::OnResize(WindowResizeEvent& e)
{
	Renderer::OnResize(e.GetWidth(), e.GetHeight());
}

void Application::Run()
{
	m_LastFrameTime = System::GetTime();

	m_Game->StartUp();

	while (m_IsRunning) {

		float time = System::GetTime();
		float deltaTime = time - m_LastFrameTime;
		m_LastFrameTime = time;
		m_DiagnosticInfo.frameTimeMS = deltaTime * 1000.0f;
		m_DiagnosticInfo.fps = 1 / deltaTime;

		time = System::GetTime();
		m_Game->Update(deltaTime);
		m_DiagnosticInfo.updateMS = (System::GetTime() - time) * 1000.0f;

		time = System::GetTime();
		m_Game->Render();
		m_DiagnosticInfo.renderMS = (System::GetTime() - time) * 1000.0f;

		time = System::GetTime();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		m_Game->OnImGui();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		m_DiagnosticInfo.imguiMS = (System::GetTime() - time) * 1000.0f;

		m_Window->OnUpdate();
	}

	m_Game->Shutdown();
}

