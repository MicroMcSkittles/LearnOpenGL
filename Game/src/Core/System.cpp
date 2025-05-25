#include "System.h"
#include <GLFW/glfw3.h>

float System::GetTime() {
	return glfwGetTime();
}