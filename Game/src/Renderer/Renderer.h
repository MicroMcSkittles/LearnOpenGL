#pragma once
#include <glm/glm.hpp>
#include <map>
#include "Core/Event/Event.h"
#include "Texture.h"
#include "Camera.h"

struct RenderDiagnosticInfo
{
	int DrawCallCount;
	int RenderGroupCount;
	int QuadCount;
	float DrawMS;

	float StartBatchMS;
	float BatchMS;
};

class Renderer {
public:
	static void Init(std::function<void(Event&)> callback);

	static RenderDiagnosticInfo& GetDiagnostic() { return s_Data.DiagnosticInfo; }

	static void StartFrame(Camera* camera);
	static void ChangeCamera(Camera* camera);
	static void EndFrame();

	static void OnResize(uint32_t width, uint32_t height);

	static void ClearColor(const glm::vec3& color);

	static void DrawQuad(const glm::vec2& pos, bool transparent = false, const glm::vec4& tint = { 1, 1, 1, 1 });
	static void DrawQuad(const glm::vec2& pos, const glm::vec2& scale, bool transparent = false, const glm::vec4& tint = { 1, 1, 1, 1 });
	static void DrawQuad(const glm::vec2& pos, const glm::vec2& scale, float rot, bool transparent = false, const glm::vec4& tint = { 1, 1, 1, 1 });
	static void DrawScreenSpaceQuad(const glm::vec2& pos, const glm::vec2& scale, float rot, bool transparent = false, const glm::vec4& tint = { 1, 1, 1, 1 });
	
	// TEXTURE STUFF
	static void DrawQuad(glm::vec2 pos, Texture* texture, bool transparent = false, glm::vec4 tint = { 1, 1, 1, 1 });
	static void DrawQuad(const glm::vec2& pos, const glm::vec2& scale, Texture* texture, bool transparent = false, const glm::vec4& tint = { 1, 1, 1, 1 });
	static void DrawQuad(const glm::vec2& pos, const glm::vec2& scale, float rot, Texture* texture, bool transparent = false, const glm::vec4& tint = { 1, 1, 1, 1 });

	static void DrawQuadAtlas(glm::vec2 pos, Texture* texture, const glm::vec2& size, const glm::vec2& texid, bool transparent = false, glm::vec4 tint = { 1, 1, 1, 1 });
	static void DrawQuadAtlas(const glm::vec2& pos, const glm::vec2& scale, Texture* texture, const glm::vec2& size, const glm::vec2& texid, bool transparent = false, const glm::vec4& tint = { 1, 1, 1, 1 });
	static void DrawQuadAtlas(const glm::vec2& pos, const glm::vec2& scale, float rot, Texture* texture, const glm::vec2& size, const glm::vec2& texid, bool transparent = false, const glm::vec4& tint = { 1, 1, 1, 1 });
	
	static void DrawScreenSpaceQuad(const glm::vec2& pos, const glm::vec2& scale, float rot, Texture* texture, bool transparent = false, const glm::vec4& tint = { 1, 1, 1, 1 });
	static void DrawScreenSpaceQuadAtlas(const glm::vec2& pos, const glm::vec2& scale, float rot, Texture* texture, const glm::vec2& size, const glm::vec2& texid, bool transparent = false, const glm::vec4& tint = { 1, 1, 1, 1 });

private:
	static int FindGroup(Texture* texture, Camera* camera, bool transparent);
	static void Submit(const glm::mat4& transform, const glm::vec2& size, const glm::vec2& texid, const glm::vec4& tint, Texture* texture, bool transparent);

private:
	struct Vertex {
		glm::vec2 position;
		glm::vec2 texCoord;
	};
	struct BatchVertex {
		glm::vec2 position;
		glm::vec2 texCoord;
		glm::vec2 texID;
		glm::vec4 tint;
	};
	struct RenderGroup {
		Camera* Camera;
		Texture* Texture;
		glm::vec2 AtlasSize;
		bool Transparent;

		uint32_t LastVertexCount;
		std::vector<BatchVertex> Vertices;
		std::vector<uint32_t> Indices;

		unsigned int VAO;
		unsigned int VBO;
		unsigned int EBO;
	};
	
	struct RenderData {
		std::function<void(Event&)> callback;

		RenderDiagnosticInfo DiagnosticInfo;

		std::vector<RenderGroup*> RenderGroups;

		Camera* CurrentCamera;

		unsigned int MainFrameBuffer;
		unsigned int ViewPortFrameBuffer;

		Texture* MainColorBuffer;
		Texture* TransparencyColorBuffer;
		Texture* ViewPortColorBuffer;

		unsigned int MainShader;
		unsigned int ViewPortShader;
		unsigned int TextureAtlasShader;
		
		std::vector<Vertex> QuadVertices;
		std::vector<uint32_t> QuadIndices;
		unsigned int Quad;

		Texture* BlankTexture;
	};
	inline static RenderData s_Data;
};