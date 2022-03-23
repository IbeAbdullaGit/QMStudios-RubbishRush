#include "ImGuiHelper.h"

// Needed for OpenGl
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// ImGui includes
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "imgui_internal.h"

#include <Logging.h>

#include <GLM/glm.hpp>
#include "StringUtils.h"

GLFWwindow* ImGuiHelper::_window = nullptr;

bool CheckboxEx(ImGuiID id, ImVec2 pos, bool* value) {
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	ImGuiContext& g = *GImGui;
	ImGuiStyle& style = g.Style;

	const float square_sz = ImGui::GetFrameHeight();
	const ImRect total_bb(pos, ImVec2(pos.x + square_sz, pos.y + square_sz));

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(total_bb, id, &hovered, &held);
	if (pressed)
	{
		*value = !(*value);
		ImGui::MarkItemEdited(id);
	}

	ImGui::RenderNavHighlight(total_bb, id);
	ImGui::RenderFrame(total_bb.Min, total_bb.Max, ImGui::GetColorU32((held && hovered) ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg), true, style.FrameRounding);
	ImU32 check_col = ImGui::GetColorU32(ImGuiCol_CheckMark);
	if (*value)
	{
		const float pad = ImMax(1.0f, (float)(int)(square_sz / 6.0f));
		ImGui::RenderCheckMark(ImVec2(pos.x + pad, pos.y + pad), check_col, square_sz - pad * 2.0f);
	}

	return pressed;
}

void ImGuiHelper::Init(GLFWwindow* window) {
	LOG_ASSERT(_window == nullptr, "Init has already been called! Should only be called once per application");
	// Store the window
	_window = window;

	// Creates a new ImGUI context
	ImGui::CreateContext();
	// Gets our ImGUI input/output 
	ImGuiIO& io = ImGui::GetIO();
	// Enable keyboard navigation
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	// Allow docking to our window
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	// Allow multiple viewports (so we can drag ImGui off our window)
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	// Set up the ImGui implementation for OpenGL
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 410");

	// Dark mode FTW
	ImGui::StyleColorsDark();

	// Get our imgui style
	ImGuiStyle& style = ImGui::GetStyle();
	//style.Alpha = 1.0f;
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 0.8f;
	}
}

void ImGuiHelper::Cleanup() {
	if (_window != nullptr) {
		// Cleanup the ImGui implementation
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		// Destroy our ImGui context
		ImGui::DestroyContext();

		_window = nullptr;
	} else {
		LOG_WARN("Called ImGuiHelper::Cleanup without initializing, seems sus");
	}
}

bool ImGuiHelper::WarningButton(const char* text, const ImVec2& size) {
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.6f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f, 0.7f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.0f, 0.8f, 0.8f));

	bool result = ImGui::Button(text, size);
	ImGui::PopStyleColor(3);
	return result;
}

void ImGuiHelper::ResourceDragSource(const IResource* resource, const std::string& name)
{
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
		std::string typeName = StringTools::SanitizeClassName(typeid(*resource).name());
		ImGui::SetDragDropPayload(typeName.c_str(), &resource->GetGUID(), sizeof(Guid));
		ImGui::Text(name.c_str());
		ImGui::EndDragDropSource();
	}
}

void ImGuiHelper::HeaderCheckbox(ImGuiID headerId, bool* value)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	ImGuiContext& g = *GImGui;
	ImGuiItemHoveredDataBackup last_item_backup;
	float button_size = g.FontSize;
	float button_x = ImMax(window->DC.LastItemRect.Min.x, window->DC.LastItemRect.Max.x - g.Style.FramePadding.x * 2.0f - button_size);
	float button_y = window->DC.LastItemRect.Min.y;
	CheckboxEx(window->GetID((void*)((intptr_t)headerId + 1)), ImVec2(button_x, button_y), value);
	last_item_backup.Restore();
}

void ImGuiHelper::StartFrame() {
	LOG_ASSERT(_window != nullptr, "You must initialize ImGuiHelper before use!");

	// Implementation new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	// ImGui context new frame
	ImGui::NewFrame();
}

void ImGuiHelper::EndFrame() {
	LOG_ASSERT(_window != nullptr, "You must initialize ImGuiHelper before use!");

	// Make sure ImGui knows how big our window is
	ImGuiIO& io = ImGui::GetIO();
	int width{ 0 }, height{ 0 };
	glfwGetWindowSize(_window, &width, &height);
	io.DisplaySize = ImVec2((float)width, (float)height);

	// Render all of our ImGui elements
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// If we have multiple viewports enabled (can drag into a new window)
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		// Update the windows that ImGui is using
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		// Restore our gl context
		glfwMakeContextCurrent(_window);
	}
}

