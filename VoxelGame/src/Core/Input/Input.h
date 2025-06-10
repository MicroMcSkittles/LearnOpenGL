#pragma once
#include <stdint.h>
#include <glm/glm.hpp>
#include "KeyCodes.h"

class Input {
public:
	static bool KeyPressed(uint32_t keycode);

	static bool MouseButtonPressed(uint32_t buttoncode);

	static glm::vec2 GetMousePos();
	static float GetMouseX();
	static float GetMouseY();

	static void HideCursor(bool hide);
};