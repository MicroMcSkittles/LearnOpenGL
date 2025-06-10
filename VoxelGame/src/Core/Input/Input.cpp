#include "Input.h"
#include "Core/Window/Window.h"
#include <GLFW/glfw3.h>

bool Input::KeyPressed(uint32_t keycode) {
	auto window = (GLFWwindow*)Window::Get()->GetHandle();
	auto state = glfwGetKey(window, keycode);
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Input::MouseButtonPressed(uint32_t buttoncode) {
	auto window = (GLFWwindow*)Window::Get()->GetHandle();
	return glfwGetMouseButton(window, buttoncode) == GLFW_PRESS;
}

glm::vec2 Input::GetMousePos() {
	auto window = (GLFWwindow*)Window::Get()->GetHandle();
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	return { x, y };
}
float Input::GetMouseX() {
	return GetMousePos().x;
}
float Input::GetMouseY() {
	return GetMousePos().y;
}

void Input::HideCursor(bool hide)
{
	auto window = (GLFWwindow*)Window::Get()->GetHandle();
	if (!hide) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	else glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}
