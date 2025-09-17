#include <iostream>
#include <string>
#include <vector>
#include <numeric>

#include "imgui.h"

#include "custom_main_menu_bar.h"
#include "core/layer.h"
#include "core/application.h"
#include "core/imgui_context.h"
#include "core/custom_widgets.h"
#include "config/settings_config.h"

void ShowStyleEditorCustom(Application* app, std::string& style, float& size)
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

	ImGui::InputFloat("Font Size", &size, 1.0f, 0.0f, "%.0f");
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		app->m_imgui_context->new_font_size = size;
		app->m_imgui_context->change_font = true;
		ChangeConfigVariable<float>(app->m_executable_path, "FontSize", size);
	}
}

CustomMenuBar::CustomMenuBar(Application* app) : m_app(app)
{
	m_gui_style = GetConfigVariable<std::string>(app->m_executable_path, "GuiStyle");
	m_font_size = GetConfigVariable<float>(app->m_executable_path, "FontSize");
}

void CustomMenuBar::OnRender()
{
	static auto _ = (m_app->m_imgui_context->header_height.push_back(ImGui::GetFrameHeight()), 0);

	static float last_frame_height = ImGui::GetFrameHeight();
	if (ImGui::GetFrameHeight() != last_frame_height)
	{
		m_app->m_imgui_context->header_height[1] = ImGui::GetFrameHeight();
		last_frame_height = ImGui::GetFrameHeight();
	}

	if (BeginViewportFixedBar("##MainMenuBarCustom", ImGui::GetMainViewport(),
		0.0f, m_app->m_imgui_context->header_height[0], 
		ImGui::GetMainViewport()->Size.x, ImGui::GetFrameHeight(),
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_MenuBar))
	{
		ImGui::BeginMenuBar();
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

			ImGui::EndMenuBar();

		ImGui::End();
	}

	if (show_style_editor)
	{
		ImGui::Begin("Style Editor", &show_style_editor);
		ShowStyleEditorCustom(m_app, m_gui_style, m_font_size);
		ImGui::End();
	}
}

std::string CustomMenuBar::GetName() const
{
	return "Menu_Bar";
}