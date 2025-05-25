#include "Input.h"
#include <GLFW/glfw3.h>
#include "Application.h"

bool Input::IsKeyPressed(uint32_t keycode) {
	GLFWwindow* window = (GLFWwindow*)Application::Get()->GetWindow()->GetHandle();
	return (glfwGetKey(window, keycode) == GLFW_PRESS);
}
bool Input::IsMouseButtonPressed(uint32_t keycode) {
	GLFWwindow* window = (GLFWwindow*)Application::Get()->GetWindow()->GetHandle();
	return (glfwGetMouseButton(window, keycode) == GLFW_PRESS);
}

std::pair<float, float> Input::GetMousePos() {
	GLFWwindow* window = (GLFWwindow*)Application::Get()->GetWindow()->GetHandle();
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	return { x, y };
}
float Input::GetMouseX() {
	auto [x, y] = GetMousePos();
	return x;
}
float Input::GetMouseY() {
	auto [x, y] = GetMousePos();
	return y;
}