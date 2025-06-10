#pragma once
#include "Core/Window/Window.h"
#include "Core/Window/WindowEvent.h"
#include "Core/Event.h"
#include "Game/Game.h"

struct ApplicationDiagnostic {
	float FPS = 0;
	float MS = 0;

	float UpdateMS = 0;
	float RendererMS = 0;
	float ImGuiMS = 0;
};

class Application {
public:
	Application();
	~Application();

	static Application* Get() { return s_Instance; }
	static ApplicationDiagnostic* GetDiagnostic();

	void EventCallback(Event::Event& e);

	void Run();

private:
	Window* m_Window;
	Game* m_GameInstance;

	bool m_Running;

#ifdef DEBUG
private:
	ApplicationDiagnostic* m_Diagnostic;
#endif

private:
	inline static Application* s_Instance;
};