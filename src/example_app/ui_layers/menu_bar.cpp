module;

#include <iostream>
#include <string>
#include <vector>

#include "imgui.h"

export module MenuBar;

import Application;
import ImguiContext;
import Layer;
import SettingsConfig;

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
				UpdateTheme(app->m_executable_path);
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

export class MenuBar : public Layer
{
	bool show_style_editor = false;
	std::string m_gui_style;
	float m_font_size;

	Application* m_app;
	float previous_area_height;
	bool first_render = true;

public:
	MenuBar(Application* app) : m_app(app)
	{
		m_gui_style = GetConfigVariable<std::string>(app->m_executable_path, "GuiStyle");
		m_font_size = GetConfigVariable<float>(app->m_executable_path, "FontSize");
		previous_area_height = m_app->m_imgui_context->header_area_height;
	}

	void OnAttach() override
	{
		m_app->m_imgui_context->header_area_height += ImGui::GetFrameHeight();
	}

	void OnRender() override
	{
		if (first_render) 
		{
			m_app->m_imgui_context->header_area_height += ImGui::GetFrameHeight();
			first_render = false;
		}

		ImGuiIO& io = ImGui::GetIO();

		ImGui::SetNextWindowPos(ImVec2(ImGui::GetMainViewport()->Pos.x, ImGui::GetMainViewport()->Pos.y + previous_area_height));
		ImGui::SetNextWindowSize(ImVec2(ImGui::GetMainViewport()->Size.x, ImGui::GetFrameHeight()));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(ImGui::GetStyle().FramePadding.x, ImGui::GetStyle().FramePadding.y * 2));
		ImGui::Begin("CustomMainMenuBar", nullptr,
			ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_MenuBar);

		if (ImGui::BeginMenuBar())
		{
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
		}

		ImGui::End();
		ImGui::PopStyleVar();

		if (show_style_editor)
		{
			ImGui::Begin("Style Editor", &show_style_editor);
			ShowStyleEditor(m_app, m_gui_style, m_font_size);
			ImGui::End();
		}
	}

	std::string GetName() const override
	{
		return "Menu_Bar";
	}
};