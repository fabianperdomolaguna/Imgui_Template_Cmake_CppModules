#pragma once 

#include <string>
#include <memory>
#include <functional>
#include <map>

#include "imgui.h"
#include "imgui_internal.h"

#include "core/layer.h"
#include "core/application.h"
#include "core/custom_widgets.h"
#include "image/image_reader.h"
#include "window_buttons.embed"

struct IconData {
    const unsigned char* data;
    unsigned int len;
};

void DrawWindowButtons(
    ImVec2& button_start, 
    ImVec2& button_end, 
    ImTextureID icon_texture,
	const float& button_size,
	const float& icon_size,
    ImU32 button_color,
    const std::function<void()>& on_click = nullptr,
    bool round_top_right = false,
    const float& round_corner_radius = 0.0f);

bool IsMaximized(GLFWwindow* window);

class TitleBar : public Layer
{
    Application* m_app;
    std::string color_style;
	ImU32 titlebar_background_color;
	float title_bar_height = 42.0f;
	float round_corner_radius = 12.0f;
    float button_size = 35.0f;
    float icon_size = 14.0f;
    int border_size = 8;

    std::unique_ptr<ImageTexture> icon_titlebar;
    std::unique_ptr<ImageTexture> minimize_button;
    std::unique_ptr<ImageTexture> maximize_button;
    std::unique_ptr<ImageTexture> close_button;
	std::unique_ptr<ImageTexture> restore_button;

    std::map<std::string, IconData> button_map = {
        {"minimize_Light",{ g_minimize_button_light,  g_minimize_button_light_len }},
        {"minimize_Dark", { g_minimize_button_dark,   g_minimize_button_dark_len }},
        {"maximize_Light",{ g_maximize_button_light,  g_maximize_button_light_len }},
        {"maximize_Dark", { g_maximize_button_dark,   g_maximize_button_dark_len }},
        {"close_Light",   { g_close_button_light,     g_close_button_light_len }},
        {"close_Dark",    { g_close_button_dark,      g_close_button_dark_len }},
        {"restore_Light", { g_restore_button_light,   g_restore_button_light_len }},
        {"restore_Dark",  { g_restore_button_dark,    g_restore_button_dark_len }},
    };

public:
    TitleBar(Application* app);

    void MenuBar();
    void HandleBorderResize();
    void OnRender() override;
    void UpdateTitleBarColor();
    void LoadButtonTextures(const std::string& style, bool reload = false);
    void DrawCenteredText(std::string text, const ImRect& rect);
    std::string GetName() const override;
};