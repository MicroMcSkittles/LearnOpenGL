#include "System.h"
#include "Core/Core.h"
#include <fstream>
#include <sstream>
#include <GLFW/glfw3.h>

float System::GetTime() {
	return (float)glfwGetTime();
}

void System::LoadStringFromFile(std::string& outStr, const std::string& filename)
{
	std::ifstream file;
	file.open(filename);
	if (!file.is_open()) {
		WARNING("Failed to open file ( " + filename + " )");
		outStr = "";
		return;
	}
	std::stringstream ss;
	ss << file.rdbuf();
	outStr = ss.str();
}
