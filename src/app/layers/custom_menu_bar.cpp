module;

#include <string>

#include "imgui.h"

export module app.layer.custom_menu_bar;

import beryl.core;
import beryl.gui;
import beryl.config.gui;
import app.layer.title_bar;
import app.widget.viewportbar;
import app.component.logging_console;

void ShowStyleEditor(beryl::core::Application* app)
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

				auto* title_bar = app->GetLayer<app::layer::TitleBar>("TitleBar");
				if (title_bar)
					title_bar->RefreshTheme();
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
	class CustomMenuBar : public beryl::core::Layer
	{
		bool m_show_style_editor = false;
		bool m_show_logging_console = false;

		beryl::core::Application* m_app = nullptr;

	public:
		CustomMenuBar(beryl::core::Application* app) 
			: Layer("CustomMainMenuBar"), m_app(app) {}

		void OnRender() override
		{
			static auto _ = (m_app->m_gui_context->AddHeaderHeight(ImGui::GetFrameHeight()), 0);

			static float last_frame_height = ImGui::GetFrameHeight();
			if (ImGui::GetFrameHeight() != last_frame_height)
			{
				m_app->m_gui_context->m_header_height[1] = ImGui::GetFrameHeight();
				last_frame_height = ImGui::GetFrameHeight();
			}

			if (app::widget::BeginViewportBar("##CustomMainMenuBar", ImGui::GetMainViewport(),
				ImVec2{ 0.0f, m_app->m_gui_context->m_header_height[0] }, 
				ImVec2{ ImGui::GetMainViewport()->Size.x, ImGui::GetFrameHeight() },
				ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | 
				ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_MenuBar))
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
					ImGui::MenuItem("Logging Console", nullptr, &m_show_logging_console);

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Settings"))
				{
					ImGui::MenuItem("Style Editor", nullptr, &m_show_style_editor);

					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();

				ImGui::End();
			}

			if (m_show_style_editor)
			{
				ImGui::Begin("Style Editor", &m_show_style_editor);
				ShowStyleEditor(m_app);
				ImGui::End();
			}

			if (m_show_logging_console)
				app::component::LoggingConsole(&m_show_logging_console);
		}
	};
}