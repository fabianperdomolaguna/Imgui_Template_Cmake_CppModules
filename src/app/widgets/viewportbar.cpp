module;

#include "imgui.h"
#include "imgui_internal.h"

export module app.widget.viewportbar;

export namespace app::widget
{
	bool BeginViewportBar(const char* name, ImGuiViewport* viewport_p = nullptr, 
		ImVec2 offset = {0.0f, 0.0f}, ImVec2 size = {0.0f, 0.0f}, ImGuiWindowFlags window_flags = 0)
	{
		ImGuiWindow* bar_window = ImGui::FindWindowByName(name);
		ImGuiViewport* target_viewport = (viewport_p != nullptr) ? viewport_p : ImGui::GetMainViewport();
		auto* viewport = static_cast<ImGuiViewportP*>(target_viewport);
		if (bar_window == nullptr || bar_window->BeginCount == 0)
		{
			ImRect avail_rect = viewport->GetBuildWorkRect();
			ImVec2 pos = avail_rect.Min;
			pos.x += offset.x;
			pos.y += offset.y;

			ImGui::SetNextWindowPos(pos);
            ImGui::SetNextWindowSize(size);
		}

		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0, 0));
		bool is_open = ImGui::Begin(name, nullptr, window_flags);
		ImGui::PopStyleVar(2);

		return is_open;
	}
}