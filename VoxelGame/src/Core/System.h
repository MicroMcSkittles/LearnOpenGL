#pragma once
#include <string>

class System {
public:
	static float GetTime();

	static void LoadStringFromFile(std::string& outStr, const std::string& filename);
};