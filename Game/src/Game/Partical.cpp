#include "Partical.h"
#include "Renderer/Renderer.h"

Partical::Partical(const glm::vec2& position, const glm::vec2& scale,
	const glm::vec2& direction, float rotation,
	Texture* atlas, const glm::vec2& atlasSize, const glm::vec2& texID,
	std::vector<Partical*>* particals)
{
	m_Position = position;
	m_Scale = scale;
	m_Direction = direction;
	m_Rotation = rotation;

	m_Atlas = atlas;
	m_TextureID = texID;
	m_AtlasSize = atlasSize;

	m_Particals = particals;
}

Debris::Debris(const glm::vec2& position, const glm::vec2& direction, 
			   Texture* atlas, const glm::vec2& atlasSize, const glm::vec2& texID,
			   std::vector<Partical*>* particals)
	:Partical(position, {0.25f, 0.25f}, direction, 0.0f, atlas, atlasSize, texID, particals)
{
	m_Rotation = atan2(-m_Direction.x, m_Direction.y);

	m_Speed = 1.0f;
	m_LeadOut = 0.0f;
	m_LifeSpan = 25.0f;
}
bool Debris::OnUpdate(float deltaTime)
{
	if (m_LifeSpan <= 0.0f) return true;
	m_LifeSpan -= deltaTime;

	m_Position += m_Direction * (m_Speed * deltaTime);
	m_Speed -= m_LeadOut * deltaTime;
	if (m_Speed < 0.0f) m_Speed = 0.0f;

	return false;
}
void Debris::Render()
{
	Renderer::DrawQuadAtlas(m_Position, m_Scale, m_Rotation, m_Atlas, m_AtlasSize, m_TextureID);
}

void Debris::SpawnShipDebris(int type, int count, glm::vec2 position, Texture* atlas, std::vector<Partical*>* particals)
{
	for (int i = 0; i < count; ++i) {
		float r = (rand() % (int)(1000 / (i + 1)));
		glm::vec2 dir = { cos(r), sin(r) };
		dir = glm::normalize(dir);

		glm::vec2 texID = { 0,0 };
		if (type < 4) {
			texID = {
				(rand() % 3),
				type
			};
		}
		else if (type == 5) {
			texID = {
				5,
				(rand() % 4)
			};
		}

		Debris* debris = new Debris(position, dir, atlas, { 4,4 }, texID, particals);
		particals->push_back(debris);
	}
}

AnimatedPartical::AnimatedPartical(const glm::vec2& position, const glm::vec2& scale,
				 const glm::vec2& direction, float rotation,
				 int frameCount, float frameDuration,
				 Texture* atlas, const glm::vec2& atlasSize,
				 std::vector<Partical*>* particals)
	: Partical(position, scale, direction, rotation, atlas, atlasSize, { 0, 0 }, particals)
{
	m_CurrentFrame = 0;
	m_FrameCount = frameCount;
	m_FrameDuration = frameDuration;
	m_Time = m_FrameDuration;
}
bool AnimatedPartical::OnUpdate(float deltaTime) {
	UpdateAnimation(deltaTime);
	return false;
}
void AnimatedPartical::Render() {
	RenderAnimation();
}
void AnimatedPartical::UpdateAnimation(float deltaTime)
{
	if (m_CurrentFrame == m_FrameCount) m_CurrentFrame = 0;
	if (m_Time <= 0.0f) {
		m_CurrentFrame += 1;
		m_Time = m_FrameDuration;
	}
	m_Time -= deltaTime;
}
void AnimatedPartical::RenderAnimation()
{
	Renderer::DrawQuadAtlas(m_Position, m_Scale, m_Rotation, m_Atlas, m_AtlasSize, FindFramePosition(m_CurrentFrame));
}
glm::vec2 AnimatedPartical::FindFramePosition(int frame)
{
	glm::vec2 texID = { 0,0 };
	texID.x = m_CurrentFrame % (int)m_AtlasSize.x;
	texID.y = floor(m_CurrentFrame / m_AtlasSize.x);
	return texID;
}

Explosion::Explosion(const glm::vec2& position, 
					 int shipType, Texture* atlas, Texture* debrisAtlas,
					 std::vector<Partical*>* particals)
	: AnimatedPartical(position, { 0.5f, 0.5f }, { 0.0f, 1.0f, }, 0.0f, 10, 0.05f, atlas, { 3,4 }, particals)
{
	m_DebrisAtlas = debrisAtlas;
	m_ShipType = shipType;
	m_SpawnedDebris = false;
}
bool Explosion::OnUpdate(float deltaTime) {
	UpdateAnimation(deltaTime);
	if (m_CurrentFrame == (m_FrameCount - 5) && m_SpawnedDebris == false) {
		Debris::SpawnShipDebris(m_ShipType, 3, m_Position, m_DebrisAtlas, m_Particals);
		m_SpawnedDebris = true;
	}
	if (m_CurrentFrame == m_FrameCount) return true;
	return false;
}