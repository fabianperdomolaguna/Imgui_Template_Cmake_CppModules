module;

#include <iostream>
#include <string>
#include <vector>

#include "imgui.h"

export module MainMenuBar;

import Application;
import ImguiContext;
import Layer;
import SettingsConfig;

void ShowStyleEditor(Application* app)
{
	ImGuiIO& io = ImGui::GetIO();

	std::vector<std::string> themes = { "Dark", "Light" };
	std::string style = GetConfigVariable<std::string>("GuiStyle");
	if (ImGui::BeginCombo("Theme", style.c_str()))
	{
		for (auto theme : themes)
		{
			if (ImGui::Selectable(theme.c_str(), style == theme))
			{
				style = theme;
				ChangeConfigVariable<std::string>("GuiStyle", style);
				UpdateTheme();
			}
		}
		ImGui::EndCombo();
	}

	static float size = io.FontDefault->FontSize;
	if (ImGui::InputFloat("Font Size", &size, 1.0f, 0.0f, "%.0f")) {
		app->m_imgui_context->change_font = true;
		app->m_imgui_context->new_font_size = size;
		ChangeConfigVariable<float>("FontSize", size);
	}
}

export class MainMenuBar : public Layer
{
    bool show_style_editor = false;

	Application* m_app;

public:
	MainMenuBar(Application* app) : m_app(app) {}

	void OnRender() override
	{
		ImGuiIO& io = ImGui::GetIO();

		if (ImGui::BeginMainMenuBar())
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

			ImGui::EndMainMenuBar();
		}

		if (show_style_editor)
		{
			ImGui::Begin("Style Editor", &show_style_editor);
			ShowStyleEditor(m_app);
			ImGui::End();
		}
	}

	std::string GetName() const override
	{
		return "Menu_Bar";
	}
};