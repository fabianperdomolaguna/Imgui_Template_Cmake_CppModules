#include "app/layers/menu_bar.h"

#include <string>

#include "imgui.h"

#include "beryl/gui/context.h"
#include "beryl/config/config_gui.h"
#include "app/components/logging_console.h"

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

app::layer::MenuBar::MenuBar(beryl::core::Application* app) 
    : Layer("MenuBar"), 
    m_app(app) {}

void app::layer::MenuBar::OnRender()
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