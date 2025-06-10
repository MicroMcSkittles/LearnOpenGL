#include "Window.h"
#include "Core/Core.h"
#include "Core/Window/WindowEvent.h"
#include "Core/Input/InputEvent.h"
#include <GLFW/glfw3.h>

Window::Window(const WindowConfig& config) {
	
	if (s_Instance) {
		SOFT_ERROR("A window instance already exists");
		return;
	}
	s_Instance = this;

	m_Config = config;
	m_Data.eventCallback = config.eventCallback;
	m_Data.width		 = config.width;
	m_Data.height		 = config.height;

	if (!glfwInit()) FATAL_ERROR("Failed to initialize GLFW");

	// Configure GLFW for OpenGL
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Configure window
	glfwWindowHint(GLFW_RESIZABLE, m_Config.resizable);

	// Create window handle
	GLFWwindow* window = glfwCreateWindow(m_Data.width, m_Data.height, m_Config.title.c_str(), NULL, NULL);
	if (window == NULL) {
		glfwTerminate();
		FATAL_ERROR("Failed to initalize window ( " + m_Config.title + " )");
	}
	glfwMakeContextCurrent(window);
	m_Handle = (void*)window;

	SetVSync(m_Config.vSync);

	// Set callback functions
	glfwSetWindowUserPointer(window, (void*)&m_Data);
	
	glfwSetWindowCloseCallback(window, [](GLFWwindow* window) {
		WindowData* data = (WindowData*)glfwGetWindowUserPointer(window);
		Event::WindowClose e;
		data->eventCallback(e);
	});
	glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height) {
		WindowData* data = (WindowData*)glfwGetWindowUserPointer(window);
		data->width = width;
		data->height = height;
		Event::WindowResize e(width, height);
		data->eventCallback(e);
	});

	glfwSetErrorCallback([](int errorCode, const char* desc) {
		std::stringstream ss;
		ss << "Window Error: Error Code( " << errorCode << " )" << std::endl;
		ss << (char)9 << desc << std::endl;
		FATAL_ERROR(ss.str());
	});

	// Set mouse callback functions
	glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
		WindowData* data = (WindowData*)glfwGetWindowUserPointer(window);
		Event::MouseButton e(button, action, mods);
		data->eventCallback(e);
	});
	glfwSetScrollCallback(window, [](GLFWwindow* window, double xOffset, double yOffset) {
		WindowData* data = (WindowData*)glfwGetWindowUserPointer(window);
		Event::MouseScroll e(xOffset, yOffset);
		data->eventCallback(e);
	});
	glfwSetCursorPosCallback(window, [](GLFWwindow* window, double x, double y) {
		WindowData* data = (WindowData*)glfwGetWindowUserPointer(window);
		Event::MouseMoved e(x, y);
		data->eventCallback(e);
	});

	// Set keyboard callback functions
	glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		WindowData* data = (WindowData*)glfwGetWindowUserPointer(window);
		Event::Keyboard e(key, scancode, action, mods);
		data->eventCallback(e);
	});
}

void Window::Update()
{
	glfwPollEvents();
	glfwSwapBuffers((GLFWwindow*)m_Handle);
}

void Window::SetVSync(bool vSync) {
	m_Config.vSync = vSync;
	if (vSync) glfwSwapInterval(1);
	else	   glfwSwapInterval(0);
}

void Window::ShouldClose(bool shouldClose)
{
	glfwSetWindowShouldClose((GLFWwindow*)m_Handle, shouldClose);
}

bool Window::ShouldClose() const
{
	return glfwWindowShouldClose((GLFWwindow*)m_Handle);
}
