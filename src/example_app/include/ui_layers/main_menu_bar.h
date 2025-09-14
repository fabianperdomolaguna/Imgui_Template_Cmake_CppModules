#pragma once

#include <string>

#include "core/layer.h"
#include "core/application.h"

void ShowStyleEditor(Application* app, std::string& style, float& size);

class MainMenuBar : public Layer
{
    bool show_style_editor = false;
	std::string m_gui_style;
	float m_font_size;

	Application* m_app;

public:
	MainMenuBar(Application* app);

    void OnRender() override;
    std::string GetName() const override;
};