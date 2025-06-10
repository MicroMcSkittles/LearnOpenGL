#include "Chunk.h"
#include "Game/VoxelRenderer.h"
#include "Game/Game.h"

Chunk::Chunk(const glm::vec2& position)
{
	auto settings = VoxelRenderer::GetSettings();
	m_Voxels = (uint32_t*)malloc(sizeof(uint32_t) * settings.chunkVolume);
	m_Position = position;
	m_Mesh = nullptr;
}

Chunk::~Chunk()
{
	free(m_Voxels);
	m_Voxels = nullptr;
	delete(m_Mesh);
}

void Chunk::BuildVoxels()
{
	auto settings = VoxelRenderer::GetSettings();
	memset(m_Voxels, 0, sizeof(uint32_t) * settings.chunkVolume);
	for (int y = 0; y < settings.chunkHeight; ++y) {
		for (int x = 0; x < settings.chunkSize; ++x) {
			for (int z = 0; z < settings.chunkSize; ++z) {
				uint32_t id = x + settings.chunkSize * z + settings.chunkArea * y;
				m_Voxels[id] = 0;
				//if (x == y && y == z) m_Voxels[id] = { 1, 0 };
				if (y < 8) {
					m_Voxels[id] = s_VoxelData->VoxelMap["Stone"];
				}
				else if (y >= 8 && y < 12) {
					m_Voxels[id] = s_VoxelData->VoxelMap["Dirt"];
				}
				else if (y == 12) {
					m_Voxels[id] = s_VoxelData->VoxelMap["Grass"];
				}
			}
		}
		if (y > 12) break;
	}
	Game::GetWorld()->ApplyAlteredVoxels(this);
}

bool Chunk::IsTransparent(const glm::vec3& position)
{
	auto settings = VoxelRenderer::GetSettings();
	uint32_t id = position.x + settings.chunkSize * position.z + settings.chunkArea * position.y;
	for (auto& i : s_VoxelData->TransparentVoxels) {
		if (m_Voxels[id] == i) return true;
	}
	if (m_Voxels[id] != 0) return false;
	return true;
}

bool Chunk::IsVoid(const glm::vec3& position)
{
	auto settings = VoxelRenderer::GetSettings();
	uint32_t id = position.x + settings.chunkSize * position.z + settings.chunkArea * position.y;
	if (m_Voxels[id] != 0) return false;
	return true;
}

void Chunk::SetVoxel(const glm::vec3& position, uint32_t voxelID)
{
	auto settings = VoxelRenderer::GetSettings();
	uint32_t id = position.x + settings.chunkSize * position.z + settings.chunkArea * position.y;
	m_Voxels[id] = voxelID;
}
uint32_t Chunk::GetVoxel(const glm::vec3& position)
{
	auto settings = VoxelRenderer::GetSettings();
	uint32_t id = position.x + settings.chunkSize * position.z + settings.chunkArea * position.y;
	return m_Voxels[id];
}

void Chunk::BuildMesh(std::function<bool(const glm::vec3&)> testVoid)
{
	auto settings = VoxelRenderer::GetSettings();

	if(m_Mesh) delete(m_Mesh);

	std::vector<VoxelVertex> vertexData;
	std::vector<uint32_t>    indexData;

	uint32_t index = 0;

	glm::vec3 worldOffset = {
		m_Position.x * (float)settings.chunkSize,
		0,
		m_Position.y * (float)settings.chunkSize
	};

	for (int x = 0; x < settings.chunkSize; ++x) {
		for (int y = 0; y < settings.chunkHeight; ++y) {
			for (int z = 0; z < settings.chunkSize; ++z) {
				uint32_t id = x + settings.chunkSize * z + settings.chunkArea * y;
				if (m_Voxels[id] == 0 || m_Voxels[id] >= s_VoxelData->VoxelInfo.size()) continue;
				Voxel& v = s_VoxelData->VoxelInfo[m_Voxels[id]];

				if (v.IsFoliage) {
					for (int i = 0; i < s_VoxelData->Foliage.VerticesCount; ++i) {
						VoxelVertex vertex = s_VoxelData->Foliage.VoxelVertices[i];
						vertex.position += glm::vec3(x, y, z);
						vertex.textureID = v.FrontTextureID;
						vertexData.push_back(vertex);
					}
					for (int i = 0; i < s_VoxelData->Foliage.IndicesCount; ++i) {
						indexData.push_back(s_VoxelData->Foliage.VoxelIndices[i] + index);
					}
					index += s_VoxelData->Foliage.VerticesCount;
					continue;
				}

				// Front Face 
				if (testVoid(glm::vec3(x,y,z + 1) + worldOffset))
				{
					for (int i = 0; i < s_VoxelData->Front.VerticesCount; ++i) {
						VoxelVertex vertex = s_VoxelData->Front.VoxelVertices[i];
						vertex.position += glm::vec3(x, y, z);
						vertex.textureID = v.FrontTextureID;
						vertexData.push_back(vertex);
					}
					for (int i = 0; i < s_VoxelData->Front.IndicesCount; ++i) {
						indexData.push_back(s_VoxelData->Front.VoxelIndices[i] + index);
					}
					index += s_VoxelData->Front.VerticesCount;
				}

				// Back Face 
				if (testVoid(glm::vec3(x,y,z - 1) + worldOffset))
				{
					for (int i = 0; i < s_VoxelData->Back.VerticesCount; ++i) {
						VoxelVertex vertex = s_VoxelData->Back.VoxelVertices[i];
						vertex.position += glm::vec3(x, y, z);
						vertex.textureID = v.BackTextureID;
						vertexData.push_back(vertex);
					}
					for (int i = 0; i < s_VoxelData->Back.IndicesCount; ++i) {
						indexData.push_back(s_VoxelData->Back.VoxelIndices[i] + index);
					}
					index += s_VoxelData->Back.VerticesCount;
				}

				// Left Face 
				if (testVoid(glm::vec3(x - 1,y,z) + worldOffset))
				{
					for (int i = 0; i < s_VoxelData->Left.VerticesCount; ++i) {
						VoxelVertex vertex = s_VoxelData->Left.VoxelVertices[i];
						vertex.position += glm::vec3(x, y, z);
						vertex.textureID = v.LeftTextureID;
						vertexData.push_back(vertex);
					}
					for (int i = 0; i < s_VoxelData->Left.IndicesCount; ++i) {
						indexData.push_back(s_VoxelData->Left.VoxelIndices[i] + index);
					}
					index += s_VoxelData->Left.VerticesCount;
				}

				// Right Face 
				if (testVoid(glm::vec3(x + 1,y,z) + worldOffset))
				{
					for (int i = 0; i < s_VoxelData->Right.VerticesCount; ++i) {
						VoxelVertex vertex = s_VoxelData->Right.VoxelVertices[i];
						vertex.position += glm::vec3(x, y, z);
						vertex.textureID = v.RightTextureID;
						vertexData.push_back(vertex);
					}
					for (int i = 0; i < s_VoxelData->Right.IndicesCount; ++i) {
						indexData.push_back(s_VoxelData->Right.VoxelIndices[i] + index);
					}
					index += s_VoxelData->Right.VerticesCount;
				}

				// Top Face 
				if (testVoid(glm::vec3(x,y + 1,z) + worldOffset))
				{
					for (int i = 0; i < s_VoxelData->Top.VerticesCount; ++i) {
						VoxelVertex vertex = s_VoxelData->Top.VoxelVertices[i];
						vertex.position += glm::vec3(x, y, z);
						vertex.textureID = v.TopTextureID;
						vertexData.push_back(vertex);
					}
					for (int i = 0; i < s_VoxelData->Top.IndicesCount; ++i) {
						indexData.push_back(s_VoxelData->Top.VoxelIndices[i] + index);
					}
					index += s_VoxelData->Top.VerticesCount;
				}

				// Bottom Face 
				if (testVoid(glm::vec3(x,y - 1,z) + worldOffset))
				{
					for (int i = 0; i < s_VoxelData->Bottom.VerticesCount; ++i) {
						VoxelVertex vertex = s_VoxelData->Bottom.VoxelVertices[i];
						vertex.position += glm::vec3(x, y, z);
						vertex.textureID = v.BottomTextureID;
						vertexData.push_back(vertex);
					}
					for (int i = 0; i < s_VoxelData->Bottom.IndicesCount; ++i) {
						indexData.push_back(s_VoxelData->Bottom.VoxelIndices[i] + index);
					}
					index += s_VoxelData->Bottom.VerticesCount;
				}
			}
		}
	}

	if (vertexData.size() == 0) return;

	m_Mesh = new VertexArray();
	m_Mesh->Bind();

	VertexBuffer* vbo = new VertexBuffer(VertexLayout({ DataType::Float3, DataType::Float2, DataType::Float2, DataType::Float }));
	vbo->SetData<VoxelVertex>(&vertexData[0], vertexData.size() * sizeof(VoxelVertex));
	vbo->Initialize();
	m_Mesh->AttachBuffer(vbo);

	ElementBuffer* ebo = new ElementBuffer();
	ebo->SetData(&indexData[0], indexData.size() * sizeof(uint32_t));
	ebo->Initialize();
	m_Mesh->AttachBuffer(ebo);

	m_Mesh->Unbind();

	vertexData.clear();
	vertexData = std::vector<VoxelVertex>();

	indexData.clear();
	indexData =  std::vector<uint32_t>();
}

VertexArray* Chunk::GetMesh()
{
	return m_Mesh;
}


