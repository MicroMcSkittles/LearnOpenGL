#pragma once
#include "Renderer/Camera.h"
#include "Renderer/Shader.h"
#include "Renderer/Texture.h"
#include "Renderer/VertexArray.h"

#include "Game/Chunk.h"

enum Alignment {
	AlignMiddle = BIT(0),
	AlignLeft = BIT(1),
	AlignRight = BIT(2),
	AlignTop = BIT(3),
	AlignBottom = BIT(4),
};

struct VoxelRendererSettings {
public:
	int chunkSize;
	int chunkHeight;
	int chunkArea;
	int chunkVolume;

	int renderDistance;
};

class VoxelRenderer {
public:

	static void Init();

	static VoxelRendererSettings GetSettings() { return s_Data->settings; }

	static void Resize(uint32_t width, uint32_t height);

	static void StartFrame(Camera* camera);
	static void StartUIFrame(Camera* camera);
	static void EndFrame();

	static void DrawChunk(Chunk* chunk);

	static void DrawSelector(const glm::vec3& position);
	static void DrawSelector(const glm::vec3& position, const glm::vec3& size);

	static void DrawUIVoxelPreview(const glm::vec2& offset, uint32_t alignment, const glm::vec2& scale, uint32_t voxel);

	static void DrawUIElement(const glm::vec2& offset, uint32_t alignment, const glm::vec2& scale, const glm::vec2& textureID = { 0,0 });
	static void DrawUIElement(const glm::vec2& offset, uint32_t alignment, const glm::vec2& scale, const glm::vec2& size, Texture* texture, const glm::vec2& textureID = { 0,0 });

	// TODO:Fix this
	static void GenerateVoxelPreview();

private:
	struct RendererData {
		VoxelRendererSettings settings;

		FrameBuffer* postProcBuffer;

		VertexArray* quad;
		VertexArray* selector;

		Camera* currentCamera;
		Camera* currentUICamera;

		Shader* mainShader;
		Shader* UIShader;
		Shader* selectorShader;
		Shader* postProcShader;

		Texture* voxelAtlas;
		Texture* otherAtlas;

		std::vector<Texture*> voxelPreviews;
	};
	inline static RendererData* s_Data;
};