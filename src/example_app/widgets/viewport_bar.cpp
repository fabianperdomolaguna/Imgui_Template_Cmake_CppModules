#include "imgui.h"
#include "imgui_internal.h"

#include "title_bar.h"

bool BeginViewportBar(const char* name, ImGuiViewport* viewport_p, float pos_x_offset, float pos_y_offset,
	float size_x, float size_y, ImGuiWindowFlags window_flags)
{
	ImGuiWindow* bar_window = ImGui::FindWindowByName(name);
	ImGuiViewportP* viewport = (ImGuiViewportP*)(void*)(viewport_p ? viewport_p : ImGui::GetMainViewport());
	if (bar_window == NULL || bar_window->BeginCount == 0)
	{
		ImRect avail_rect = viewport->GetBuildWorkRect();
		ImVec2 pos = avail_rect.Min;
		pos.x += pos_x_offset;
		pos.y += pos_y_offset;

		ImGui::SetNextWindowPos(pos);
		ImGui::SetNextWindowSize(ImVec2(size_x, size_y));
	}

	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0, 0));
	bool is_open = ImGui::Begin(name, NULL, window_flags);
	ImGui::PopStyleVar(2);

	return is_open;
}