#include "Application.h"
#include <iostream>
#include "Renderer/Renderer.h"
#include "System.h"

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
	config.resizable = false;
	config.title = "Test Window";
	m_Window = new Window(BIND_EVENT_FN(Application::ProcEvent), config);

	Renderer::Init(BIND_EVENT_FN(Application::ProcEvent));

	m_Game = new Game();

	m_IsRunning = true;
}
Application::~Application()
{
	delete(m_Window);
	delete(m_Game);
}

void Application::ProcEvent(Event& e)
{
	EventHandler eh(e);
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

		// TODO: add delta time system
		m_Game->Update(deltaTime);

		m_Game->Render();

		m_Window->OnUpdate();
	}

	m_Game->Shutdown();
}

