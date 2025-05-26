#pragma once
#include "Window.h"
#include "Core/Event/Event.h"
#include "Core/Event/WindowEvent.h"
#include "Game/Game.h"

struct DiagnosticInfo
{
	float fps;
	float frameTimeMS;
	float updateMS;
	float renderMS;
	float imguiMS;
};

class Application
{
public:
	Application();
	~Application();

	static Application* Get() { return s_App; }
	Window* GetWindow() { return m_Window; }
	const DiagnosticInfo& GetDiagnosticInfo() { return m_DiagnosticInfo; }

	void ProcEvent(Event& e);
	void OnClose(WindowCloseEvent& e);
	void OnError(ErrorEvent& e);
	void OnResize(WindowResizeEvent& e);

	void Run();

private:
	DiagnosticInfo m_DiagnosticInfo;

	Window* m_Window;
	bool m_IsRunning;
	float m_LastFrameTime;

	Game* m_Game;

private:
	inline static Application* s_App;
};

