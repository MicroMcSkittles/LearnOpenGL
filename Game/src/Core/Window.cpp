#include "Window.h"
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include "Core/Event/WindowEvent.h"

Window::Window(std::function<void(Event&)> EventProc, WindowConfig config)
{
	m_Data.width = config.width;
	m_Data.height = config.height;
	m_Data.callback = EventProc;

	glfwInit();
	// Tell GLFW that I'm using OpenGL 4.6
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	// Tell GLFW that I'm using a core profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_RESIZABLE, (config.resizable) ? GLFW_TRUE : GLFW_FALSE);

	// Create window
	GLFWwindow* window = glfwCreateWindow(m_Data.width, m_Data.height, config.title.c_str(), NULL, NULL);
	m_Handle = (void*)window;
	// Varify that the window was created successfully
	if (window == NULL) {
		ErrorEvent err("Failed to create window");
		m_Data.callback(err);
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(window);

	SetVSync(config.vSync);

	glfwSetWindowUserPointer(window, &m_Data);

	glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int Width, int Height) {
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
		WindowResizeEvent e(Width, Height);
		data.callback(e);
	});
	glfwSetWindowCloseCallback(window, [](GLFWwindow* window) {
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
		WindowCloseEvent e;
		data.callback(e);
	});
}
Window::~Window()
{
}

uint32_t Window::GetWidth() const
{
	return m_Data.width;
}

uint32_t Window::GetHeight() const
{
	return m_Data.height;
}

void Window::SetVSync(bool enabled)
{
	if (enabled) glfwSwapInterval(1);
	else glfwSwapInterval(0);

	m_Data.vSync = enabled;
}

void Window::OnUpdate()
{
	glfwSwapBuffers((GLFWwindow*)m_Handle);
	glfwPollEvents();
}

