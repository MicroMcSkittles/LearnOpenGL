#pragma once
#include <glm/glm.hpp>
#include "Player.h"
#include "Renderer/Texture.h"
#include "Renderer/Camera.h"

class Background {
public:
	Background(const std::string& texture, float paralaxStrength, Camera* camera, Player* player);

	void Render();

private:
	int m_TileSize;

	int m_PlayerTX;
	int m_PlayerTY;

	float m_ParalaxStrength;

	Texture* m_Texture;
	Player* m_Player;
	Camera* m_Camera;
};