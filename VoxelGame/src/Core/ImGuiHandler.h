#pragma once

enum class Style {
	Dark,
	Light,
	Classic
};

// I thought this would do more at first
struct ImGuiConfig {
	Style style = Style::Dark;
	const char* glslVersion = "#version 460";
};

struct ImGuiDiagnostic {
	float lastMS = 0;
	float MS     = 0;
};

class ImGuiHandler {
public:
	// Must be called after window gets created
	static void Init(const ImGuiConfig& config = { });
	// Returns a blank ImGuiDiagnostic in release mode
	static ImGuiDiagnostic* GetDiagnostic();

	static void StartFrame();

	static void EndFrame();

#ifdef DEBUG
private:
	inline static ImGuiDiagnostic* s_DiagnosticInstance;
#endif
};