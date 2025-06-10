#include "ImGuiHandler.h"
#include "Core/Core.h"
#include "Core/System.h"
#include "Core/Window/Window.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

void ImGuiHandler::Init(const ImGuiConfig& config)
{
#ifdef DEBUG
	if (s_DiagnosticInstance) WARNING("ImGui diagnostic instance already exists");
	s_DiagnosticInstance = new ImGuiDiagnostic();
#endif 

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	switch (config.style)
	{
	case Style::Dark: ImGui::StyleColorsDark(); break;
	case Style::Light: ImGui::StyleColorsLight(); break;
	case Style::Classic: ImGui::StyleColorsClassic(); break;
	}
	
	GLFWwindow* window = (GLFWwindow*)Window::Get()->GetHandle();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(config.glslVersion);

	NORM_MESSAGE("Initialized ImGui successfully");
}

ImGuiDiagnostic* ImGuiHandler::GetDiagnostic()
{
#ifdef DEBUG
	return s_DiagnosticInstance;
#elif RELEASE
	return new ImGuiDiagnostic();
#endif
}

void ImGuiHandler::StartFrame()
{
#ifdef DEBUG
	s_DiagnosticInstance->lastMS = System::GetTime();
#endif

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void ImGuiHandler::EndFrame()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

#ifdef DEBUG
	s_DiagnosticInstance->MS = System::GetTime() - s_DiagnosticInstance->lastMS;
#endif
}
