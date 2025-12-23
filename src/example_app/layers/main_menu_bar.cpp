#include <iostream>
#include <string>
#include <vector>

#include "imgui.h"

#include "main_menu_bar.h"
#include "core/layer.h"
#include "core/application.h"
#include "core/imgui_context.h"
#include "config/settings_config.h"

void ShowStyleEditor(Application* app, std::string& style, float& size)
{
	ImGuiIO& io = ImGui::GetIO();

	static std::vector<std::string> themes = { "Dark", "Light" };
	if (ImGui::BeginCombo("Theme", style.c_str()))
	{
		for (auto theme : themes)
		{
			if (ImGui::Selectable(theme.c_str(), style == theme))
			{
				style = theme;
				ChangeConfigVariable<std::string>(app->m_executable_path, "GuiStyle", style);
				UpdateTheme(app->m_executable_path, app->m_imgui_context->color_style);
			}
		}
		ImGui::EndCombo();
	}

	if (ImGui::InputFloat("Font Size", &size, 1.0f, 0.0f, "%.0f")) {
		app->m_imgui_context->change_font = true;
		app->m_imgui_context->new_font_size = size;
		ChangeConfigVariable<float>(app->m_executable_path, "FontSize", size);
	}
}

MainMenuBar::MainMenuBar(Application* app) : m_app(app) 
{
	m_gui_style = m_app->m_imgui_context->color_style;
	m_font_size = m_app->m_imgui_context->font_size;
}

void MainMenuBar::OnRender()
{
	ImGuiIO& io = ImGui::GetIO();

	ImGui::BeginMainMenuBar();
	
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Exit"))
			m_app->m_window->m_close_popup = true;

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Settings"))
	{
		ImGui::MenuItem("Style Editor", NULL, &show_style_editor);

		ImGui::EndMenu();
	}

	ImGui::EndMainMenuBar();

	if (show_style_editor)
	{
		ImGui::Begin("Style Editor", &show_style_editor);
		ShowStyleEditor(m_app, m_gui_style, m_font_size);
		ImGui::End();
	}
}

std::string MainMenuBar::GetName() const
{
	return "Menu_Bar";
}