#pragma once
#include "Core/Core.h"
#include "Renderer/RendererEnum.h"
#include <glm/glm.hpp>

struct RenderAPIDiagnostic {
	int DrawCallCount = 0;
	int ClearCount    = 0;
};

class RenderAPI {
public:

	// Must be called after the window is created
	static void Init();
	// Returns a blank RenderAPIDiagnostic in release mode
	static RenderAPIDiagnostic* GetDiagnostic();

	static void SetViewPortSize(uint32_t width, uint32_t height);

	static void ClearColor(float r, float g, float b, float a);
	static void ClearColor(const glm::vec4& color);

	static void SetClearMask(uint32_t mask);
	static void Clear();

	static void SetDrawMode(DrawMode mode);
	static void DrawElements(uint32_t count, DataType type, const void* indices);

private:
	struct RenderAPIData {
		uint32_t clearMask;
		uint32_t drawMode;
	};
	inline static RenderAPIData* s_Data;

#ifdef DEBUG
private:
	inline static RenderAPIDiagnostic* s_DiagnosticInstance;
#endif
};