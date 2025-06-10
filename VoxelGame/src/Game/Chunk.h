#pragma once
#include "Game/Voxel.h"
#include "Renderer/VertexArray.h"
#include <vector>
#include <functional>

class Chunk {
public:
	Chunk(const glm::vec2& position);
	~Chunk();

	void BuildVoxels();
	void BuildMesh(std::function<bool(const glm::vec3&)> testVoid);

	VertexArray* GetMesh();

	glm::vec2 GetPosition() { return m_Position; }

	bool IsTransparent(const glm::vec3& position);
	bool IsVoid(const glm::vec3& position);
	void SetVoxel(const glm::vec3& position, uint32_t voxelID);
	uint32_t GetVoxel(const glm::vec3& position);

private:
	uint32_t* m_Voxels;
	VertexArray* m_Mesh;
	glm::vec2 m_Position;
};