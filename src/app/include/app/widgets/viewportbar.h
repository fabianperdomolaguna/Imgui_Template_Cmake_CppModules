#pragma once 

#include "imgui.h"

namespace app::widget
{
	bool BeginViewportBar(const char* name, ImGuiViewport* viewport_p = nullptr, 
		ImVec2 offset = {0.0f, 0.0f}, ImVec2 size = {0.0f, 0.0f}, ImGuiWindowFlags window_flags = 0);
}