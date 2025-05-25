#pragma once
#include <glm/glm.hpp>
#include "Core/Event/Event.h"
#include "Texture.h"
#include "Camera.h"

class Renderer {
public:
	static void Init(std::function<void(Event&)> callback);

	static void StartFrame(Camera* camera);
	static void EndFrame();

	static void OnResize(uint32_t width, uint32_t height);

	static void ClearColor(const glm::vec3& color);

	static void DrawQuad(const glm::vec2& pos, const glm::vec3& tint = { 1, 1, 1 });
	static void DrawQuad(const glm::vec2& pos, const glm::vec2& scale, const glm::vec3& tint = { 1, 1, 1 });
	static void DrawQuad(const glm::vec2& pos, const glm::vec2& scale, float rot, const glm::vec3& tint = { 1, 1, 1 });
	
	// TEXTURE STUFF
	static void DrawQuad(glm::vec2 pos, Texture* texture, glm::vec3 tint = { 1, 1, 1 });
	static void DrawQuad(const glm::vec2& pos, const glm::vec2& scale, Texture* texture, const glm::vec3& tint = { 1, 1, 1 });
	static void DrawQuad(const glm::vec2& pos, const glm::vec2& scale, float rot, Texture* texture, const glm::vec3& tint = { 1, 1, 1 });


private:
	struct RenderData {
		std::function<void(Event&)> callback;

		Camera* CurrentCamera;

		unsigned int MainShader;
		unsigned int Quad;

		Texture* BlankTexture;
	};
	inline static RenderData s_Data;
};