module;

#include <iostream>
#include <string>
#include <vector>
#include <numeric>

#include "imgui.h"

export module CustomMainMenuBar;

import Application;
import ImguiContext;
import Layer;
import ViewportBar;
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

export class CustomMenuBar : public Layer
{
	bool show_style_editor = false;
	std::string m_gui_style;
	float m_font_size;

	Application* m_app;
	bool first_render = true;

public:
	CustomMenuBar(Application* app) : m_app(app)
	{
		m_gui_style = GetConfigVariable<std::string>(app->m_executable_path, "GuiStyle");
		m_font_size = GetConfigVariable<float>(app->m_executable_path, "FontSize");
	}

	void OnRender() override
	{
		static auto _ = (m_app->m_imgui_context->header_height.push_back(ImGui::GetFrameHeight()), 0);

		static float last_frame_height = ImGui::GetFrameHeight();
		if (ImGui::GetFrameHeight() != last_frame_height)
		{
			m_app->m_imgui_context->header_height[1] = ImGui::GetFrameHeight();
			last_frame_height = ImGui::GetFrameHeight();
		}

		if (BeginViewportBar("##MainMenuBarCustom", ImGui::GetMainViewport(),
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
			ShowStyleEditor(m_app, m_gui_style, m_font_size);
			ImGui::End();
		}
	}

	std::string GetName() const override
	{
		return "Menu_Bar";
	}
};