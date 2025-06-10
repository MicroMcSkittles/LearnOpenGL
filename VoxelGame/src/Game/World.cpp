#include "World.h"
#include "Game/VoxelRenderer.h"
#include "Game/Game.h"
#include <algorithm>

World::World(Player* player)
{
	m_Player = player;

	auto& settings = VoxelRenderer::GetSettings();
	m_ViewBox.size = { 
		settings.renderDistance * 2 + 1,
		settings.renderDistance * 2 + 1
	};
	m_ViewBox.lastPos = { 0,0 };

	DataTree& worldSettings = Game::GetConfig()["Game"]["WorldSettings"];
	m_SaveLocation = worldSettings["SaveDirectory"].GetValue();
	m_InfiniteWorld = worldSettings["InfiniteWorld"].GetValue();
	m_SaveLocation.append("worldData.json");
}

void World::Update(float deltaTime)
{
	if(!m_InfiniteWorld) return;
	auto& settings = VoxelRenderer::GetSettings();

	glm::vec3 pos = m_Player->GetPosition();
	glm::vec2 chunkCoord = {
		floor(pos.x / settings.chunkSize),
		floor(pos.z / settings.chunkSize)
	};

	if (chunkCoord != m_ViewBox.lastPos) {
		glm::vec2 delta = chunkCoord - m_ViewBox.lastPos;
		m_ViewBox.lastPos = chunkCoord;
		std::vector<glm::vec2> updatedChunks;

		if (delta.x < 0) {
			for (int i = 0; i < m_ViewBox.size.y; ++ i) {

				glm::vec2 nextPos = m_Chunks[i][m_Chunks[i].size() - 2]->GetPosition();
				nextPos.x -= m_Chunks[i].size() - abs(delta.x);

				Chunk* newChunk = new Chunk(nextPos);
				newChunk->BuildVoxels();
				updatedChunks.push_back({ 0, i });
				updatedChunks.push_back({ 1, i });

				delete(m_Chunks[i][m_Chunks[i].size() - 1]);
				m_Chunks[i].pop_back();
				m_Chunks[i].insert(m_Chunks[i].begin(), newChunk);
			}
		}
		else if (delta.x > 0) {
			for (int i = 0; i < m_ViewBox.size.y; ++i) {

				glm::vec2 nextPos = m_Chunks[i][0]->GetPosition();
				nextPos.x += m_Chunks[i].size() - abs(delta.x) + 1;

				Chunk* newChunk = new Chunk(nextPos);
				newChunk->BuildVoxels();
				updatedChunks.push_back({ m_Chunks[i].size() - 1, i });
				updatedChunks.push_back({ m_Chunks[i].size() - 2, i });

				delete(m_Chunks[i][0]);
				m_Chunks[i].erase(m_Chunks[i].begin());
				m_Chunks[i].push_back(newChunk);
			}
		}
		if (delta.y < 0) {

			float nextZ = m_Chunks[m_Chunks.size() - 2][0]->GetPosition().y;
			nextZ -= m_Chunks.size() - abs(delta.y);
			float startX = m_Chunks[m_Chunks.size() - 2][0]->GetPosition().x;
			for (auto chunk : m_Chunks[m_Chunks.size() - 1]) delete chunk;
			m_Chunks.pop_back();
			m_Chunks.insert(m_Chunks.begin(), std::vector<Chunk*>());

			for (int i = 0; i < m_ViewBox.size.x; ++i) {

				glm::vec2 nextPos = { startX + i, nextZ };
				Chunk* newChunk = new Chunk(nextPos);
				newChunk->BuildVoxels();
				updatedChunks.push_back({ i, 0 });
				updatedChunks.push_back({ i, 1 });

				m_Chunks[0].push_back(newChunk);
			}
		}
		else if (delta.y > 0) {

			float nextZ = m_Chunks[0][0]->GetPosition().y;
			nextZ += m_Chunks.size() - abs(delta.y) + 1;
			float startX = m_Chunks[0][0]->GetPosition().x;
			for (auto chunk : m_Chunks[0]) delete chunk;
			m_Chunks.erase(m_Chunks.begin());
			m_Chunks.push_back(std::vector<Chunk*>());

			for (int i = 0; i < m_ViewBox.size.x; ++i) {

				glm::vec2 nextPos = { startX + i, nextZ };
				Chunk* newChunk = new Chunk(nextPos);
				newChunk->BuildVoxels();
				updatedChunks.push_back({ i, m_Chunks.size() - 1 });
				updatedChunks.push_back({ i, m_Chunks.size() - 2 });

				m_Chunks[m_Chunks.size() - 1].push_back(newChunk);
			}
		}
	
		for (auto& uChunk : updatedChunks) {
			Chunk* c = m_Chunks[(int)uChunk.y][(int)uChunk.x];
			c->BuildMesh(CLASS_BIND_ARGS_1(World::IsTransparent));
		}
	}
}
void World::Render()
{
	for (int y = 0; y < m_Chunks.size(); ++y) {
		for (int x = 0; x < m_Chunks[y].size(); ++x) {
			Chunk* chunk = m_Chunks[y][x];
			VoxelRenderer::DrawChunk(chunk);
		}
	}
}
void World::ImGui()
{

}

void World::LoadWorld()
{
	DataTree save;
	json::LoadFile(m_SaveLocation, &save);

	for (auto& chunk : save["AlteredChunks"].elements) {
		glm::vec2 position = {
			chunk["Position"].At(0).GetValue(),
			chunk["Position"].At(1).GetValue()
		};
		uint32_t id = (int)chunk["VoxelListID"].GetValue();
		m_AlteredChunks.push_back({ position, id });
	}
	for (auto& voxels : save["AlteredVoxels"].elements) {
		std::vector<std::pair<glm::vec3, uint32_t>> voxelData;
		for (auto& voxel : voxels.elements) {
			glm::vec3 position = {
				voxel["Position"].At(0).GetValue(),
				voxel["Position"].At(1).GetValue(),
				voxel["Position"].At(2).GetValue()
			};
			uint32_t id = (int)voxel["Id"].GetValue();
			voxelData.push_back({ position, id });
		}
		m_AlteredVoxels.push_back(voxelData);
	}

	auto& settings = VoxelRenderer::GetSettings();

	m_Chunks.resize(m_ViewBox.size.y);

	for (int x = -settings.renderDistance; x <= settings.renderDistance; ++x) {
		for (int z = -settings.renderDistance; z <= settings.renderDistance; ++z) {
			Chunk* c = new Chunk({ x, z });
			c->BuildVoxels();

			uint32_t viewZ = z + settings.renderDistance;
			m_Chunks[viewZ].push_back(c);
		}
	}

	for (int y = 0; y < m_Chunks.size(); ++y) {
		for (int x = 0; x < m_Chunks[y].size(); ++x) {
			m_Chunks[y][x]->BuildMesh(CLASS_BIND_ARGS_1(World::IsTransparent));
		}
	}
}
void World::ApplyAlteredVoxels(Chunk* chunk)
{
	if (m_AlteredChunks.empty()) return;

	int chunkID = 0;
	for (; chunkID < m_AlteredChunks.size(); chunkID++) {
		if (m_AlteredChunks[chunkID].first != chunk->GetPosition()) {
			continue;
		}
		break;
	}
	if (chunkID == m_AlteredChunks.size()) return;

	auto& voxels = m_AlteredVoxels[m_AlteredChunks[chunkID].second];

	for (int i = 0; i < voxels.size(); i++) {
		glm::vec3 position = voxels[i].first;
		chunk->SetVoxel(position, voxels[i].second);
	}
}
void World::PushAlteredVoxel(const glm::vec2& chunkPos, const glm::vec3& position, uint32_t voxelID)
{
	for (int i = 0; i < m_AlteredChunks.size(); i++) {
		if (m_AlteredChunks[i].first == chunkPos) {
			
			uint32_t index = m_AlteredChunks[i].second;
			auto& voxels = m_AlteredVoxels[index];

			for (int j = 0; j < voxels.size(); j++) {
				if (voxels[j].first == position) {
					voxels[j].second = voxelID;
					return;
				}
			}
			voxels.push_back({ position, voxelID });

			return;
		}
	}
	
	std::vector<std::pair<glm::vec3, uint32_t>> voxels;
	voxels.push_back({ position, voxelID });
	m_AlteredChunks.push_back({ chunkPos, m_AlteredVoxels.size() });
	m_AlteredVoxels.push_back(voxels);
}

void World::SaveWorld()
{
	DataTree save(DataTreeType::Object);

	DataTree alteredVoxels(DataTreeType::Array);
	for (auto& voxels : m_AlteredVoxels) {
		DataTree voxelsData(DataTreeType::Array);
		for (auto& [position, id] : voxels) {
			DataTree positionData(DataTreeType::Array);
			positionData.elements.push_back(position.x);
			positionData.elements.push_back(position.y);
			positionData.elements.push_back(position.z);

			DataTree voxelData(DataTreeType::Object);
			voxelData.children.insert({ "Position", positionData });
			voxelData.children.insert({ "Id", (int)id });

			voxelsData.elements.push_back(voxelData);
		}
		alteredVoxels.elements.push_back(voxelsData);
	}
	save.children.insert({ "AlteredVoxels", alteredVoxels });

	DataTree alteredChunks(DataTreeType::Array);
	for (auto& [position, id] : m_AlteredChunks) {
		DataTree positionData(DataTreeType::Array);
		positionData.elements.push_back(position.x);
		positionData.elements.push_back(position.y);

		DataTree chunkData(DataTreeType::Object);
		chunkData.children.insert({ "Position", positionData });
		chunkData.children.insert({ "VoxelListID", (int)id });

		alteredChunks.elements.push_back(chunkData);
	}
	save.children.insert({ "AlteredChunks", alteredChunks });

	json::Serialize(m_SaveLocation, &save);
}

void World::SetVoxel(const glm::vec3& position, uint32_t voxelID)
{
	auto& settings = VoxelRenderer::GetSettings();
	glm::vec2 chunkCoord = {
		floor(position.x / settings.chunkSize),
		floor(position.z / settings.chunkSize)
	};
	glm::vec3 subCoord = GetSubChunkPosition(position);

	PushAlteredVoxel(chunkCoord, subCoord, voxelID);

	if (chunkCoord.x > m_ViewBox.lastPos.x + settings.renderDistance ||
		chunkCoord.x < m_ViewBox.lastPos.x - settings.renderDistance ||
		chunkCoord.y > m_ViewBox.lastPos.y + settings.renderDistance ||
		chunkCoord.y < m_ViewBox.lastPos.y - settings.renderDistance) {
		return;
	}

	glm::vec2 viewChunkCoord = {
		settings.renderDistance + (chunkCoord.x - m_ViewBox.lastPos.x),
		settings.renderDistance + (chunkCoord.y - m_ViewBox.lastPos.y)
	};

	m_Chunks[viewChunkCoord.y][viewChunkCoord.x]->SetVoxel(subCoord, voxelID);

	m_Chunks[viewChunkCoord.y][viewChunkCoord.x]->BuildMesh(CLASS_BIND_ARGS_1(World::IsTransparent));
	if (subCoord.x == 0)
		m_Chunks[viewChunkCoord.y][viewChunkCoord.x - 1]->BuildMesh(CLASS_BIND_ARGS_1(World::IsTransparent));
	else if (subCoord.x == settings.chunkSize -  1)
		m_Chunks[viewChunkCoord.y][viewChunkCoord.x + 1]->BuildMesh(CLASS_BIND_ARGS_1(World::IsTransparent));
	if (subCoord.z == 0)
		m_Chunks[viewChunkCoord.y - 1][viewChunkCoord.x]->BuildMesh(CLASS_BIND_ARGS_1(World::IsTransparent));
	else if (subCoord.z == settings.chunkSize - 1)
		m_Chunks[viewChunkCoord.y + 1][viewChunkCoord.x]->BuildMesh(CLASS_BIND_ARGS_1(World::IsTransparent));
}
uint32_t World::GetVoxel(const glm::vec3& position)
{
	auto& settings = VoxelRenderer::GetSettings();
	glm::vec2 chunkCoord = {
		floor(position.x / settings.chunkSize),
		floor(position.z / settings.chunkSize)
	};

	if (chunkCoord.x > m_ViewBox.lastPos.x + settings.renderDistance ||
		chunkCoord.x < m_ViewBox.lastPos.x - settings.renderDistance ||
		chunkCoord.y > m_ViewBox.lastPos.y + settings.renderDistance ||
		chunkCoord.y < m_ViewBox.lastPos.y - settings.renderDistance) {
		return 0;
	}

	glm::vec2 viewChunkCoord = {
		settings.renderDistance + (chunkCoord.x - m_ViewBox.lastPos.x),
		settings.renderDistance + (chunkCoord.y - m_ViewBox.lastPos.y)
	};

	glm::vec3 subCoord = GetSubChunkPosition(position);

	return m_Chunks[viewChunkCoord.y][viewChunkCoord.x]->GetVoxel(subCoord);
}

glm::vec3 World::GetSubChunkPosition(const glm::vec3& position)
{
	auto& settings = VoxelRenderer::GetSettings();

	glm::vec3 subCoord = {
		(int)position.x % settings.chunkSize,
		position.y,
		(int)position.z % settings.chunkSize
	};
	if (subCoord.x < 0) subCoord.x = settings.chunkSize + subCoord.x;
	if (subCoord.z < 0) subCoord.z = settings.chunkSize + subCoord.z;

	return subCoord;
}
bool World::IsTransparent(const glm::vec3& position)
{
	auto& settings = VoxelRenderer::GetSettings();
	glm::vec2 chunkCoord = {
		floor(position.x / settings.chunkSize),
		floor(position.z / settings.chunkSize)
	};

	if (chunkCoord.x > m_ViewBox.lastPos.x + settings.renderDistance ||
		chunkCoord.x < m_ViewBox.lastPos.x - settings.renderDistance ||
		chunkCoord.y > m_ViewBox.lastPos.y + settings.renderDistance ||
		chunkCoord.y < m_ViewBox.lastPos.y - settings.renderDistance) {
		return true;
	}

	if (position.y < 0 || position.y >= settings.chunkHeight) {
		return true;
	}

	glm::vec2 viewChunkCoord = {
		settings.renderDistance + (chunkCoord.x - m_ViewBox.lastPos.x),
		settings.renderDistance + (chunkCoord.y - m_ViewBox.lastPos.y)
	};

	glm::vec3 subCoord = GetSubChunkPosition(position);

	return m_Chunks[viewChunkCoord.y][viewChunkCoord.x]->IsTransparent(subCoord);
}
bool World::IsVoid(const glm::vec3& position)
{
	auto& settings = VoxelRenderer::GetSettings();
	glm::vec2 chunkCoord = {
		floor(position.x / settings.chunkSize),
		floor(position.z / settings.chunkSize)
	};

	if (chunkCoord.x > m_ViewBox.lastPos.x + settings.renderDistance ||
		chunkCoord.x < m_ViewBox.lastPos.x - settings.renderDistance ||
		chunkCoord.y > m_ViewBox.lastPos.y + settings.renderDistance ||
		chunkCoord.y < m_ViewBox.lastPos.y - settings.renderDistance) {
		return true;
	}

	if (position.y < 0 || position.y >= settings.chunkHeight) {
		return true;
	}

	glm::vec2 viewChunkCoord = {
		settings.renderDistance + (chunkCoord.x - m_ViewBox.lastPos.x),
		settings.renderDistance + (chunkCoord.y - m_ViewBox.lastPos.y)
	};

	glm::vec3 subCoord = GetSubChunkPosition(position);

	return m_Chunks[viewChunkCoord.y][viewChunkCoord.x]->IsVoid(subCoord);
}

bool World::CastRay(glm::vec3 position, glm::vec3 direction, float length, glm::vec3& point, glm::vec3& normal) {
	// Almost everything here is from this page
	// I didn't feel like reading 4 papers on ray casts at midnight
	// https://stackoverflow.com/questions/12367071/how-do-i-initialize-the-t-variables-in-a-fast-voxel-traversal-algorithm-for-ray

#define SIGN(x) (x > 0 ? 1 : (x < 0 ? -1 : 0));
#define FRAC0(x) (x - floorf(x))
#define FRAC1(x) (1 - x + floorf(x))

	float tMaxX, tMaxY, tMaxZ, tDeltaX, tDeltaY, tDeltaZ;
	glm::vec3 voxel;
	glm::vec3 v1 = position;
	glm::vec3 v2 = v1 + (direction * length);

	int dx = SIGN(v2.x - v1.x);
	if (dx != 0) tDeltaX = fmin(dx / (v2.x - v1.x), 10000000.0f); else tDeltaX = 10000000.0f;
	if (dx > 0) tMaxX = tDeltaX * FRAC1(v1.x); else tMaxX = tDeltaX * FRAC0(v1.x);
	voxel.x = floor(v1.x);

	int dy = SIGN(v2.y - v1.y);
	if (dy != 0) tDeltaY = fmin(dy / (v2.y - v1.y), 10000000.0f); else tDeltaY = 10000000.0f;
	if (dy > 0) tMaxY = tDeltaY * FRAC1(v1.y); else tMaxY = tDeltaY * FRAC0(v1.y);
	voxel.y = floor(v1.y);

	int dz = SIGN(v2.z - v1.z);
	if (dz != 0) tDeltaZ = fmin(dz / (v2.z - v1.z), 10000000.0f); else tDeltaZ = 10000000.0f;
	if (dz > 0) tMaxZ = tDeltaZ * FRAC1(v1.z); else tMaxZ = tDeltaZ * FRAC0(v1.z);
	voxel.z = floor(v1.z);

	while (!(tMaxX > 1 && tMaxY > 1 && tMaxZ > 1)) {
		if (!IsVoid(voxel)) {

			point = voxel;
			return true;
		}
		if (tMaxX < tMaxY) {
			if (tMaxX < tMaxZ) {
				voxel.x += dx;
				tMaxX += tDeltaX;
				normal = { -floor(dx), 0, 0 };
			}
			else {
				voxel.z += dz;
				tMaxZ += tDeltaZ;
				normal = { 0, 0, -floor(dz) };
			}
		}
		else {
			if (tMaxY < tMaxZ) {
				voxel.y += dy;
				tMaxY += tDeltaY;
				normal = { 0, -floor(dy), 0 };
			}
			else {
				voxel.z += dz;
				tMaxZ += tDeltaZ;
				normal = { 0, 0, -floor(dz) };
			}
		}
	}
	return false;
}
bool World::Collide(const glm::vec3& position, const glm::vec3& size, std::vector<glm::vec3>& voxels)
{
	glm::vec3 unitPos = {
		(position.x >= 0) ? floor(position.x) : ceil(position.x),
		(position.y >= 0) ? floor(position.y) : ceil(position.y),
		(position.z >= 0) ? floor(position.z) : ceil(position.z)
	};
	glm::vec3 unitSize = {
		ceil(position.x + size.x),
		ceil(position.y + size.y),
		ceil(position.z + size.z)
	};

	for (int x = unitPos.x; x < unitSize.x; x++) {
		for (int y = unitPos.y; y < unitSize.y; y++) {
			for (int z = unitPos.z; z < unitSize.z; z++) {
				if (!IsVoid({ x,y,z })) {
					voxels.push_back({ x,y,z });
				}
			}
		}
	}

	return (!voxels.empty());
}


