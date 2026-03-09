module;

#include <string>

#include "imgui.h"

export module app.layer.menu_bar;

import beryl.core;
import beryl.gui;
import beryl.config.gui;
import app.component.logging_console;

void ShowStyleEditorMain(beryl::core::Application* app)
{
	ImGuiIO& io = ImGui::GetIO();

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

export namespace app::layer
{
	class MenuBar : public beryl::core::Layer
	{
		bool m_show_style_editor = false;
		bool m_show_logging_console = false;

		beryl::core::Application* m_app;

	public:
		MenuBar(beryl::core::Application* app) 
			: Layer("MenuBar"), 
			m_app(app) {}

		void OnRender() override
		{
			ImGuiIO& io = ImGui::GetIO();

			ImGui::BeginMainMenuBar();
			
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Exit"))
					m_app->m_window->m_close_popup = true;

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Tools"))
			{
				ImGui::MenuItem("Logging Console", NULL, &m_show_logging_console);

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Settings"))
			{
				ImGui::MenuItem("Style Editor", NULL, &m_show_style_editor);

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
			
			if (m_show_style_editor)
			{
				ImGui::Begin("Style Editor", &m_show_style_editor);
				ShowStyleEditorMain(m_app);
				ImGui::End();
			}

			if (m_show_logging_console)
				app::component::LoggingConsole(&m_show_logging_console);
		}
	};
}