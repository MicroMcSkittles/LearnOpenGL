#pragma once
#include <string>
#include <map>
#include "Renderer/Texture.h"
#include "Renderer/Camera.h"

class FontRenderer {
public:
	static void Init();
	static void SetCamera(Camera* camera);

	static float GetTextWidth(float charWidth, const std::string& text);
	static float GetTextHeight(float charHeight, const std::string& text);

	static void DrawText(const glm::vec2& position, const glm::vec2& charScale, float rot, const std::string& text, const glm::vec3& tint = { 1.0f,1.0f,1.0f });
	static void DrawTextScreenSpace(const glm::vec2& position, const glm::vec2& charScale, float rot, const std::string& text, const glm::vec3& tint = { 1.0f,1.0f,1.0f });

private:
	struct FontRendererData {
		Camera* CurrentCamera;
		Texture* FontAtlas;
		std::map<char, glm::vec2> FontLookup;

		float PaddingX;
		float PaddingY;
	};
	inline static FontRendererData* s_Data;
};