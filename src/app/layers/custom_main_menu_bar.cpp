module;

#include <iostream>
#include <string>
#include <vector>
#include <numeric>

#include "imgui.h"

export module CustomMainMenuBar;

import beryl.core;
import beryl.gui;
import beryl.config.gui;
import app.widget.viewportbar;
import LoggingConsole;

void ShowStyleEditor(beryl::core::Application* app)
{
	ImGuiIO& io = ImGui::GetIO();

	static std::vector<std::string> themes = { "Dark", "Light" };
	if (ImGui::BeginCombo("Theme", app->m_gui_context->m_theme.c_str()))
	{
		for (auto theme : app->m_gui_context->m_themes_map)
		{
			if (ImGui::Selectable(theme.first.c_str(), app->m_gui_context->m_theme == theme.first))
			{
				app->m_gui_context->m_theme = theme.first;
				app->m_gui_context->UpdateTheme();
				beryl::config::gui::Set<std::string>(app->m_executable_path, "GuiStyle", theme.first);
			}
		}
		ImGui::EndCombo();
	}

	ImGui::InputFloat("Font Size", &app->m_gui_context->m_font_size, 1.0f, 0.0f, "%.0f");
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		app->m_gui_context->ChangeFontSize();
		beryl::config::gui::Set<float>(app->m_executable_path, "FontSize", app->m_gui_context->m_font_size);
	}
}

export class CustomMenuBar : public beryl::core::Layer
{
	bool show_style_editor = false;
	bool show_logging_console = false;
	std::string m_gui_style;
	float m_font_size;

	beryl::core::Application* m_app;
	bool first_render = true;

public:
    CustomMenuBar(beryl::core::Application* app) 
		: Layer("CustomMainMenuBar"), 
		m_app(app) {}

	void OnRender() override
	{
		static auto _ = (m_app->m_gui_context->AddHeaderHeight(ImGui::GetFrameHeight()), 0);

		static float last_frame_height = ImGui::GetFrameHeight();
		if (ImGui::GetFrameHeight() != last_frame_height)
		{
			m_app->m_gui_context->m_header_height[1] = ImGui::GetFrameHeight();
			last_frame_height = ImGui::GetFrameHeight();
		}

		if (app::widget::BeginViewportBar("##MainMenuBarCustom", ImGui::GetMainViewport(),
			ImVec2{ 0.0f, m_app->m_gui_context->m_header_height[0] }, 
			ImVec2{ ImGui::GetMainViewport()->Size.x, ImGui::GetFrameHeight() },
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_MenuBar))
		{
			ImGui::BeginMenuBar();

			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Exit"))
					m_app->m_window->m_close_popup = true;

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Tools"))
			{
				ImGui::MenuItem("Logging Console", NULL, &show_logging_console);

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Settings"))
			{
				ImGui::MenuItem("Style Editor", NULL, &show_style_editor);

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();

			ImGui::End();
		}

		if (show_style_editor)
		{
			ImGui::Begin("Style Editor", &show_style_editor);
			ShowStyleEditor(m_app);
			ImGui::End();
		}

		if (show_logging_console)
			LoggingConsole(&show_logging_console);
	}
};