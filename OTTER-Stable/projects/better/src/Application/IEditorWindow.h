#pragma once

#include "Utils/Macros.h"
#include "imgui.h"
#include <string>

/**
 * The editor window interface allows us to separate implementation for 
 * editor or debug windows
 */
class IEditorWindow {
public:
	MAKE_PTRS(IEditorWindow);
	NO_COPY(IEditorWindow);
	NO_MOVE(IEditorWindow);

	bool        Open = true;
	ImGuiDir_   SplitDirection = ImGuiDir_::ImGuiDir_Left;
	float       SplitDepth = 0.2f;
	std::string Name;
	std::string ParentName;
	ImGuiID     DockId;

	virtual ~IEditorWindow() =default;

	virtual void Render() = 0;

protected:
	IEditorWindow() = default;
};