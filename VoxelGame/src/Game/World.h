#pragma once
#include "Game/Chunk.h"
#include "Game/Player.h"
#include "Renderer/Texture.h"
#include "Core/JSON.h"

class World {
public:

	World(Player* player);

	void Update(float deltaTime);
	void Render();
	void ImGui();

public:
	void LoadWorld();
	void ApplyAlteredVoxels(Chunk* chunk);
	void PushAlteredVoxel(const glm::vec2& chunkPos, const glm::vec3& position, uint32_t voxelID);
	
	void SaveWorld();

	void SetVoxel(const glm::vec3& position, uint32_t voxelID);
	uint32_t GetVoxel(const glm::vec3& position);

	glm::vec3 GetSubChunkPosition(const glm::vec3& position);
	bool IsTransparent(const glm::vec3& position);
	bool IsVoid(const glm::vec3& position);

	bool CastRay(glm::vec3 position, glm::vec3 direction, float length, glm::vec3& point, glm::vec3& normal);
	// returns true if the given bounding box collides with any voxels and pushes all collided voxels to the vector&
	bool Collide(const glm::vec3& position, const glm::vec3& size, std::vector<glm::vec3>& voxels);

private:
	std::vector<std::vector<Chunk*>> m_Chunks;

	std::vector<std::vector<std::pair<glm::vec3, uint32_t>>> m_AlteredVoxels;
	std::vector<std::pair<glm::vec2, uint32_t>> m_AlteredChunks;

	Player* m_Player;

	std::string m_SaveLocation;
	bool m_InfiniteWorld;

	struct {
		glm::vec2 size;
		glm::vec2 lastPos;
	} m_ViewBox;
};