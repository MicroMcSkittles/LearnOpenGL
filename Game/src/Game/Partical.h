#pragma once
#include <glm/glm.hpp>
#include "Renderer/Texture.h"

class Partical {
public:
	Partical(const glm::vec2& position, const glm::vec2& scale, 
			 const glm::vec2& direction, float rotation,
			 Texture* atlas, const glm::vec2& atlasSize, const glm::vec2& texID,
			 std::vector<Partical*>* particals);

	// Return true to destroy partical
	virtual bool OnUpdate(float deltaTime) = 0;
	virtual void Render() = 0;

protected:
	glm::vec2 m_Position;
	glm::vec2 m_Scale;
	glm::vec2 m_Direction;
	float m_Rotation;

	Texture* m_Atlas;
	glm::vec2 m_AtlasSize;
	glm::vec2 m_TextureID;

	std::vector<Partical*>* m_Particals;
};

class Debris : public Partical {
public:
	Debris(const glm::vec2& position, const glm::vec2& direction, 
		   Texture* atlas, const glm::vec2& atlasSize, const glm::vec2& texID,
		   std::vector<Partical*>* particals);

	virtual bool OnUpdate(float deltaTime) override;
	virtual void Render() override;

	static void SpawnShipDebris(int type, int count, glm::vec2 position, Texture* atlas, std::vector<Partical*>* particals);

private:
	float m_Speed;
	float m_LeadOut;
	float m_LifeSpan;
};

class AnimatedPartical : public Partical {
public:
	AnimatedPartical(const glm::vec2& position, const glm::vec2& scale,
					 const glm::vec2& direction, float rotation,
					 int frameCount, float frameDuration,
					 Texture* atlas, const glm::vec2& atlasSize,
					 std::vector<Partical*>* particals);

	virtual bool OnUpdate(float deltaTime) override;
	virtual void Render() override;

protected:
	void UpdateAnimation(float deltaTime);
	void RenderAnimation();

	glm::vec2 FindFramePosition(int frame);

protected:
	int m_CurrentFrame;
	int m_FrameCount;

	float m_FrameDuration;
	float m_Time;
};

class Explosion : public AnimatedPartical {
public:
	Explosion(const glm::vec2& position, 
		      int shipType, Texture* atlas, Texture* debrisAtlas,
		      std::vector<Partical*>* particals);

	virtual bool OnUpdate(float deltaTime) override;

private:
	Texture* m_DebrisAtlas;

	int m_ShipType;
	bool m_SpawnedDebris;
};