#include "Enemy.h"
#include "Renderer/Renderer.h"
#include <random>

Enemy::Enemy(const glm::vec2& pos, const glm::vec2& dir, Player* player, Texture* atlas)
{
	m_Player = player;

	m_Position = pos;
	m_Direction = dir;
	m_Atlas = atlas;

	m_Rotation = atan2(dir.x, dir.y);

	glm::vec2 TextureIDS[] = {
		{0,0},
		{0,1},
		{2,0},
		{3,1}
	};

	m_Type = rand() % 4;
	m_TexID = TextureIDS[m_Type];
	m_Speed = 1.0f + (rand() % 50) / 25;
	m_TurnSpeed = 2.0f + (rand() % 50) / 50;
	m_FireTime = 0.5f + (rand() % 6) / 2;
	m_Health = 1 + (rand() % 2);

	m_FireRange = glm::radians(20.0f);
	m_Time = 6.0f;

	m_TintTime = 0.0f;
}

bool Enemy::OnUpdate(float deltaTime)
{
	m_TintTime -= 5.0f * deltaTime;
	if (m_TintTime < 0.0f) m_TintTime = 0.0f;

	glm::vec2 directionToPlayer = m_Player->GetPosition() - m_Position;
	if (glm::length(directionToPlayer) == 0) directionToPlayer = { 1, 0 };
	else directionToPlayer = glm::normalize(directionToPlayer);

	bool shoot = (glm::length(directionToPlayer + m_Direction) >= m_FireRange);
	if (shoot) {
		if (m_Time <= 0.0f) {
			m_Time = m_FireTime;
		}
		else shoot = false;
	}
	m_Time -= deltaTime;
	if (m_Time < 0) m_Time = 0;

	glm::vec2 newDirection = {};
	if (glm::length(directionToPlayer + m_Direction) <= 0.2) {
		if (rand() % 2) newDirection = glm::vec2(directionToPlayer.y, -directionToPlayer.x);
		else  newDirection = glm::vec2(directionToPlayer.y, directionToPlayer.x);
	}
	else {
		newDirection = deltaTime * m_TurnSpeed * directionToPlayer + m_Direction;
	}

	float length = glm::length(newDirection);
	m_Direction = glm::normalize(newDirection);
	m_Rotation = atan2(m_Direction.x, -m_Direction.y);

	length = glm::clamp(length, 0.1f, 3.0f);

	m_Position += m_Direction * deltaTime * m_Speed * length;
	return shoot;
}

void Enemy::Render()
{
	glm::vec4 tint = { 1.0f, 1.0f - (m_TintTime / 2.0f), 1.0f - (m_TintTime / 2.0f), 1.0f };
	Renderer::DrawQuadAtlas(m_Position, { 0.25f, 0.25f }, m_Rotation, m_Atlas, { 5,2 }, m_TexID, false, tint);
}

void Enemy::Damage(int damage)
{
	m_TintTime = 2.0f;
	m_Health -= damage;
}

bool Enemy::ShouldKill()
{
	//if (glm::distance(m_Position, m_Player->GetPosition()) >= 12) return true;
	if (m_Health <= 0.0f) return true;

	return false;
}

EnemySpawner::EnemySpawner(Player* player, Texture* atlas, std::vector<Enemy>* enemies)
{
	m_Player = player;
	m_Enemies = enemies;
	m_Atlas = atlas;

	m_SpawnRadius = 3.0f;
	m_SpawnRate = 3.0f;
	m_Time = m_SpawnRate;

	m_Wave = 0;
	m_WaveCount = 25;
	m_KillCount = 0;
}

void EnemySpawner::OnUpdate(float deltaTime)
{
	if (m_Time <= 0.0f && m_KillCount < m_WaveCount) {
		glm::vec2 dir = {
			(rand() % 1000) / 1000.0f,
			(rand() % 1000) / 1000.0f,
		};
		dir = glm::normalize(dir);

		Enemy e(
			m_Player->GetPosition() + (dir * m_SpawnRadius),
			-dir,
			m_Player,
			m_Atlas
		);

		m_Enemies->push_back(e);

		m_Time = m_SpawnRate;
	}
	else if (m_KillCount >= m_WaveCount) {
		m_Wave += 1;
		m_KillCount = 0;
		m_WaveCount = m_WaveCount * 2;
		m_SpawnRate -= ((m_Wave * m_Wave) / 2.0f);
		if (m_SpawnRate < 0.5f) m_SpawnRate = 0.5f;
	}

	m_Time -= deltaTime;
	if (m_Time < 0.0f) m_Time = 0.0f;
}
