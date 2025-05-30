#pragma once
#include <functional>
#include <string>
#include "Core/Event/Event.h"

struct WindowConfig {
	uint32_t width = 800;
	uint32_t height = 600;
	std::string title = "Window";
	bool resizable = true;
	bool vSync = true;
};

struct WindowData {
	uint32_t width;
	uint32_t height;
	bool vSync;
	std::function<void(Event&)> callback;
};

class Window {
public:
	Window(std::function<void(Event&)> EventProc, WindowConfig config);
	~Window();

	void* GetHandle() { return m_Handle; }

	uint32_t GetWidth() const;
	uint32_t GetHeight() const;

	bool GetVSync() const { return m_Data.vSync; }

	void SetVSync(bool enabled);

	void OnUpdate();

private:
	void* m_Handle;
	WindowData m_Data;
};