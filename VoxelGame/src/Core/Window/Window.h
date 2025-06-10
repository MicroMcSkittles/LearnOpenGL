#pragma once
#include <string>
#include <functional>
#include "Core/Event.h"

// Settings needed to create a window
struct WindowConfig {
	std::string title;
	std::function<void(Event::Event&)> eventCallback;

	uint32_t width  = 800;
	uint32_t height = 600;

	bool resizable  = true;
	bool vSync      = true;
};

// You can only have one window at a time
class Window {
public:
	Window(const WindowConfig& config);

	static Window* Get() { return s_Instance; }

	void     Update();
	void     SetVSync(bool vSync);
	void     ShouldClose(bool shouldClose);

	uint32_t GetWidth()    const { return m_Config.width; }
	uint32_t GetHeight()   const { return m_Config.height; }
	bool	 GetVSync()    const { return m_Config.vSync; }
	void*    GetHandle()   const { return m_Handle; }
	bool     ShouldClose() const; // Returns true if the window should close

private:
	struct WindowData
	{
		std::function<void(Event::Event&)> eventCallback;

		uint32_t width;
		uint32_t height;
	};

	void*		 m_Handle;
	WindowConfig m_Config;
	WindowData   m_Data;

private:
	inline static Window* s_Instance;
};