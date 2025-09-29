#pragma once 

#include "imgui.h"

bool BeginViewportBar(const char* name, ImGuiViewport* viewport_p, float pos_x_offset, float pos_y_offset,
	float size_x, float size_y, ImGuiWindowFlags window_flags);