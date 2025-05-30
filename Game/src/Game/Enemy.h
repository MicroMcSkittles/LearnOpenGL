#pragma once
#include "Renderer/Texture.h"
#include "Player.h"

class Enemy
{
public:
	Enemy(const glm::vec2& pos, const glm::vec2& dir, Player* player, Texture* atlas);

	bool OnUpdate(float deltaTime);
	void Render();

	void Damage(int damage);
	bool ShouldKill();

	const glm::vec2& GetPosition() { return m_Position; }
	const glm::vec2& GetDirection() { return m_Direction; }

	int GetType() { return m_Type; }

private:
	Texture* m_Atlas;
	glm::vec2 m_TexID;

	Player* m_Player;

	glm::vec2 m_Position;
	glm::vec2 m_Direction;
	float m_Rotation;

	float m_Health;
	float m_Speed;
	float m_TurnSpeed;
	float m_FireRange;
	float m_FireTime;
	float m_Time;
	float m_TintTime;
	int m_Type;
};

class EnemySpawner {
public:
	EnemySpawner(Player* player, Texture* atlas, std::vector<Enemy>* enemies);

	void OnUpdate(float deltaTime);

	int GetWave() { return m_Wave; }

	int& GetKillCount() { return m_KillCount; }

private:
	Player* m_Player;
	std::vector<Enemy>* m_Enemies;

	Texture* m_Atlas;

	int m_Wave;
	int m_WaveCount;
	int m_KillCount;

	float m_SpawnRadius;
	float m_SpawnRate;
	float m_Time;
	float m_BreakTime;
};