#include "FontRenderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Core/Application.h"
#include "Renderer/Renderer.h"

void FontRenderer::Init() {
	s_Data = new FontRendererData();

	TextureConfig config;
	config.MinFilter = TexFilterMode::Nearest;
	config.MagFilter = TexFilterMode::Nearest;
	config.FlipY = true;
	s_Data->FontAtlas = new Texture("assets/textures/CommodorePixeled.png", config);

	s_Data->PaddingX = 0.0f;
	s_Data->PaddingY = 0.1f;

	{
		s_Data->FontLookup.insert({ '0', { 0,0 } });
		s_Data->FontLookup.insert({ '1', { 1,0 } });
		s_Data->FontLookup.insert({ '2', { 2,0 } });
		s_Data->FontLookup.insert({ '3', { 3,0 } });
		s_Data->FontLookup.insert({ '4', { 4,0 } });
		s_Data->FontLookup.insert({ '5', { 5,0 } });
		s_Data->FontLookup.insert({ '6', { 6,0 } });
		s_Data->FontLookup.insert({ '7', { 7,0 } });
		s_Data->FontLookup.insert({ '8', { 8,0 } });
		s_Data->FontLookup.insert({ '9', { 9,0 } });
		s_Data->FontLookup.insert({ ':', { 10,0 } });
		s_Data->FontLookup.insert({ ';', { 11,0 } });
		s_Data->FontLookup.insert({ '<', { 12,0 } });
		s_Data->FontLookup.insert({ '=', { 13,0 } });
		s_Data->FontLookup.insert({ '>', { 14,0 } });
		s_Data->FontLookup.insert({ '?', { 15,0 } });

		s_Data->FontLookup.insert({ '@', { 16,0 } });
		s_Data->FontLookup.insert({ 'A', { 17,0 } });
		s_Data->FontLookup.insert({ 'B', { 18,0 } });
		s_Data->FontLookup.insert({ 'C', { 19,0 } });
		s_Data->FontLookup.insert({ 'D', { 20,0 } });
		s_Data->FontLookup.insert({ 'E', { 21,0 } });
		s_Data->FontLookup.insert({ 'F', { 22,0 } });
		s_Data->FontLookup.insert({ 'G', { 23,0 } });
		s_Data->FontLookup.insert({ 'H', { 24,0 } });
		s_Data->FontLookup.insert({ 'I', { 25,0 } });
		s_Data->FontLookup.insert({ 'J', { 26,0 } });
		s_Data->FontLookup.insert({ 'K', { 27,0 } });
		s_Data->FontLookup.insert({ 'L', { 28,0 } });
		s_Data->FontLookup.insert({ 'M', { 29,0 } });
		s_Data->FontLookup.insert({ 'N', { 30,0 } });
		s_Data->FontLookup.insert({ 'O', { 31,0 } });

		s_Data->FontLookup.insert({ 'P', { 0,1 } });
		s_Data->FontLookup.insert({ 'Q', { 1,1 } });
		s_Data->FontLookup.insert({ 'R', { 2,1 } });
		s_Data->FontLookup.insert({ 'S', { 3,1 } });
		s_Data->FontLookup.insert({ 'T', { 4,1 } });
		s_Data->FontLookup.insert({ 'U', { 5,1 } });
		s_Data->FontLookup.insert({ 'V', { 6,1 } });
		s_Data->FontLookup.insert({ 'W', { 7,1 } });
		s_Data->FontLookup.insert({ 'X', { 8,1 } });
		s_Data->FontLookup.insert({ 'Y', { 9,1 } });
		s_Data->FontLookup.insert({ 'Z', { 10,1 } });
		s_Data->FontLookup.insert({ '[', { 11,1 } });
		s_Data->FontLookup.insert({ '\\', { 12,1 } });
		s_Data->FontLookup.insert({ ']', { 13,1 } });
		//s_Data->FontLookup.insert({ '=', { 14,0 } });
		s_Data->FontLookup.insert({ '_', { 15,1 } });

		s_Data->FontLookup.insert({ '`', { 16,1 } });
		s_Data->FontLookup.insert({ 'a', { 17,1 } });
		s_Data->FontLookup.insert({ 'b', { 18,1 } });
		s_Data->FontLookup.insert({ 'c', { 19,1 } });
		s_Data->FontLookup.insert({ 'd', { 20,1 } });
		s_Data->FontLookup.insert({ 'e', { 21,1 } });
		s_Data->FontLookup.insert({ 'f', { 22,1 } });
		s_Data->FontLookup.insert({ 'g', { 23,1 } });
		s_Data->FontLookup.insert({ 'h', { 24,1 } });
		s_Data->FontLookup.insert({ 'i', { 25,1 } });
		s_Data->FontLookup.insert({ 'j', { 26,1 } });
		s_Data->FontLookup.insert({ 'k', { 27,1 } });
		s_Data->FontLookup.insert({ 'l', { 28,1 } });
		s_Data->FontLookup.insert({ 'm', { 29,1 } });
		s_Data->FontLookup.insert({ 'n', { 30,1 } });
		s_Data->FontLookup.insert({ 'o', { 31,1 } });

		s_Data->FontLookup.insert({ 'p', { 0,2 } });
		s_Data->FontLookup.insert({ 'q', { 1,2 } });
		s_Data->FontLookup.insert({ 'r', { 2,2 } });
		s_Data->FontLookup.insert({ 's', { 3,2 } });
		s_Data->FontLookup.insert({ 't', { 4,2 } });
		s_Data->FontLookup.insert({ 'u', { 5,2 } });
		s_Data->FontLookup.insert({ 'v', { 6,2 } });
		s_Data->FontLookup.insert({ 'w', { 7,2 } });
		s_Data->FontLookup.insert({ 'x', { 8,2 } });
		s_Data->FontLookup.insert({ 'y', { 9,2 } });
		s_Data->FontLookup.insert({ 'z', { 10,2 } });
		s_Data->FontLookup.insert({ ' ', { 11,2 } });
	}
}
void FontRenderer::SetCamera(Camera* camera) {
	s_Data->CurrentCamera = camera;
}

float FontRenderer::GetTextWidth(float charWidth, const std::string& text)
{
	return (text.size() * charWidth) + ((text.size() - 1) * s_Data->PaddingX);
}

float FontRenderer::GetTextHeight(float charHeight, const std::string& text)
{
	int endLineCount = 0;
	for (char c : text) {
		if (c == '\n') endLineCount += 1;
	}
	return charHeight + (endLineCount * charHeight) + (endLineCount * s_Data->PaddingY);
}

void FontRenderer::DrawText(const glm::vec2& position, const glm::vec2& charScale, float rot, const std::string& text, const glm::vec3& tint) {
	
	// make position the top left corner of the first char
	// Flip chars on the y axis because i messed up the atlas and dont want to fix it.
	glm::vec2 scale = { charScale.x, -charScale.y };
	glm::vec2 worldScale = position + scale;

	glm::vec2 midPoint = {
		(position.x + worldScale.x) / 2.0f,
		(position.y + worldScale.y) / 2.0f
	};
	glm::vec2 right = {
		worldScale.x,
		midPoint.y
	};
	glm::vec2 down = {
		midPoint.x,
		worldScale.y,
	};

	glm::mat4 rotationMat = glm::rotate(glm::mat4(1.0f), rot, { 0.0f,0.0f,1.0f });
	glm::vec2 rotatedScale = position + glm::vec2(rotationMat * glm::vec4(worldScale - position, 0.0f, 1.0f));
	right				   = position + glm::vec2(rotationMat * glm::vec4(right      - position, 0.0f, 1.0f));
	down				   = position + glm::vec2(rotationMat * glm::vec4(down       - position, 0.0f, 1.0f));

	glm::vec2 currentPos = {
		(position.x + rotatedScale.x) / 2.0f,
		(position.y + rotatedScale.y) / 2.0f
	};
	right = glm::normalize(right - currentPos);
	down  = glm::normalize(down  - currentPos);

	int endLineCount = 0;

	for (int i = 0; i < text.size(); ++i) {
		char currentChar = text[i];
		if (currentChar == '\n') {
			currentPos = {
				(position.x + rotatedScale.x) / 2.0f,
				(position.y + rotatedScale.y) / 2.0f
			};
			currentPos += down * -(scale.y + (scale.y * endLineCount) + (endLineCount * scale.y * s_Data->PaddingY));
			endLineCount += 1;
			continue;
		}

		Renderer::DrawQuadAtlas(currentPos, scale, rot, s_Data->FontAtlas, { 32,32 }, s_Data->FontLookup[currentChar]);
		currentPos += right * (scale.x + (scale.x * s_Data->PaddingX));
	}
}

void FontRenderer::DrawTextScreenSpace(const glm::vec2& position, const glm::vec2& charScale, float rot, const std::string& text, const glm::vec3& tint)
{
	//auto window = Application::Get()->GetWindow();
	//glm::vec2 screenCenter;
	/*screenCenter = {
		window->GetWidth() / 2,
		window->GetHeight() / 2
	};*/
	DrawText(
		s_Data->CurrentCamera->ScreenToWorld(position),
		s_Data->CurrentCamera->ScreenToWorld(charScale),
		rot,
		text
	);
}
