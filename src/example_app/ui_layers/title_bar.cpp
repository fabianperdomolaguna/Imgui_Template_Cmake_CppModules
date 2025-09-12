module;

#include <string>
#include <memory>
#include <functional>
#include <map>

#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_internal.h"

#include "icon_titlebar.embed"
#include "window_buttons.embed"

export module TitleBar;

import Layer;
import Application;
import Image;

void DrawWindowButtons(
    ImVec2& button_start, 
    ImVec2& button_end, 
    ImTextureID icon_texture,
	const float& button_size,
	const float& icon_size,
    ImU32 button_color,
    const std::function<void()>& on_click = nullptr,
    bool round_top_right = false,
    const float& round_corner_radius = 0.0f)
{
    ImRect button_rect(button_start, button_end);
    bool button_hovered = button_rect.Contains(ImGui::GetMousePos());
    if (button_hovered)
    {
        if (round_top_right)
        {
            ImGui::GetBackgroundDrawList()->AddRectFilled(
                button_start, button_end, button_color,
                round_corner_radius, ImDrawFlags_RoundCornersTopRight);
        }
        else 
        {
            ImGui::GetBackgroundDrawList()->AddRectFilled(button_start, button_end, button_color);
        }
    }
    if (button_hovered && ImGui::IsMouseClicked(0))
		on_click();

    ImVec2 center = button_rect.GetCenter();
    ImVec2 icon_start = ImVec2(floor(center.x - icon_size * 0.5f), floor(center.y - icon_size * 0.5f));
    ImVec2 icon_end = ImVec2(floor(icon_start.x + icon_size), floor(icon_start.y + icon_size));
    ImGui::GetBackgroundDrawList()->AddImage(
        icon_texture,
        icon_start,
        icon_end
    );

	button_start = ImVec2(button_start.x + button_size, button_start.y);
    button_end = ImVec2(button_end.x + button_size, button_end.y);
}

struct IconData {
    const unsigned char* data;
    unsigned int len;
};

bool IsMaximized(GLFWwindow* window)
{
    return (bool)glfwGetWindowAttrib(window, GLFW_MAXIMIZED);
}

export class TitleBar : public Layer
{
    Application* m_app;
    std::string color_style;
	ImU32 titlebar_background_color;
	float title_bar_height = 42.0f;
	float round_corner_radius = 12.0f;
    float button_size = 35.0f;
    float icon_size = 14.0f;

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
    TitleBar(Application* app) : m_app(app) 
    {
        m_app->m_imgui_context->header_height.push_back(title_bar_height);
		color_style = m_app->m_imgui_context->color_style;
        UpdateTitleBarColor();

        icon_titlebar = std::make_unique<ImageTexture>(g_icon_titlebar, g_icon_titlebar_len, GL_RGBA, true);
        LoadButtonTextures(color_style, false);
    }

    void OnRender() override
    {
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos);
        ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size);
        ImGui::Begin("OverlayWindow", nullptr,
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoInputs |
            ImGuiWindowFlags_NoBackground);

        ImGui::GetWindowDrawList()->PushClipRectFullScreen();

        if (color_style != m_app->m_imgui_context->color_style)
        {
            color_style = m_app->m_imgui_context->color_style;
            UpdateTitleBarColor();
            LoadButtonTextures(color_style, true);
		}

        ImVec2 top_left = ImGui::GetMainViewport()->Pos;
        ImVec2 bottom_right = ImVec2(top_left.x + ImGui::GetMainViewport()->Size.x, top_left.y + 42.0f);
        ImRect titlebar_rect(top_left, bottom_right);
        DrawCenteredText(m_app->m_app_specification.title, titlebar_rect);

        ImGui::GetBackgroundDrawList()->AddRectFilled(
            top_left,
            bottom_right,
            titlebar_background_color,
            IsMaximized(m_app->m_window->m_window) ? 0.0f : round_corner_radius,
            ImDrawFlags_RoundCornersTop
        );

        ImGui::InvisibleButton("##TitleBarDragZone", ImVec2(ImGui::GetMainViewport()->Size.x, 42.0f));
        if (ImGui::IsMouseHoveringRect(top_left, bottom_right) && ImGui::IsMouseDragging(0))
        {
            int wx, wy;
            glfwGetWindowPos(m_app->m_window->m_window, &wx, &wy);
            ImVec2 delta = ImGui::GetIO().MouseDelta;
            glfwSetWindowPos(m_app->m_window->m_window, wx + (int)delta.x, wy + (int)delta.y);
        }
        ImGui::End();

		float logo_size = title_bar_height * 0.8;
        const ImVec2 logo_offset(title_bar_height * 0.3, title_bar_height * 0.1);
        ImGui::GetBackgroundDrawList()->AddImage(
            (ImTextureID)(intptr_t)icon_titlebar->get_texture(),
            ImVec2(top_left.x + logo_offset.x, top_left.y + logo_offset.y),
            ImVec2(top_left.x + logo_offset.x + logo_size, top_left.y + logo_offset.y + logo_size)
		);

        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 button_start = ImVec2(viewport->Pos.x + viewport->Size.x - button_size * 3, viewport->Pos.y);
		ImVec2 button_end = ImVec2(button_start.x + button_size, button_start.y + title_bar_height);

        DrawWindowButtons(
            button_start,
            button_end,
            (ImTextureID)(intptr_t)minimize_button->get_texture(),
            button_size,
            icon_size,
            IM_COL32(154, 140, 152, 128),
            [&]() { glfwIconifyWindow(m_app->m_window->m_window); }
        );

        DrawWindowButtons(
            button_start,
            button_end,
            (ImTextureID)(intptr_t)(
                IsMaximized(m_app->m_window->m_window) ? restore_button->get_texture()
                : maximize_button->get_texture()),
            button_size,
            icon_size,
            IM_COL32(154, 140, 152, 128),
            [&]() {
                if (IsMaximized(m_app->m_window->m_window))
                    glfwRestoreWindow(m_app->m_window->m_window);
                else
                    glfwMaximizeWindow(m_app->m_window->m_window);
            }
        );

        DrawWindowButtons(
            button_start,
            button_end,
            (ImTextureID)(intptr_t)close_button->get_texture(),
            button_size,
            icon_size,
            IM_COL32(255, 0, 0, 128),
            [&]() { m_app->m_window->m_close_popup = true; },
            !IsMaximized(m_app->m_window->m_window),
            round_corner_radius
        );
    }

    void UpdateTitleBarColor()
    {
        titlebar_background_color =
            (m_app->m_imgui_context->color_style == "Light")
            ? IM_COL32(222, 220, 215, 255)
            : IM_COL32(40, 40, 40, 255);
    }

    void LoadButtonTextures(const std::string& style, bool reload = false)
    {
        auto suffix = "_" + style;

        auto load_or_reload = [&](std::unique_ptr<ImageTexture>& texture, const std::string& key) {
            const auto& icon = button_map[key + suffix];
            if (reload)
                texture->reload(icon.data, icon.len);
            else
                texture = std::make_unique<ImageTexture>(icon.data, icon.len, GL_RGBA, true);
        };

        load_or_reload(minimize_button, "minimize");
        load_or_reload(maximize_button, "maximize");
        load_or_reload(close_button, "close");
        load_or_reload(restore_button, "restore");
    }

    void DrawCenteredText(std::string text, const ImRect& rect)
    {
        const char* text_cstr = text.c_str();
        ImVec2 text_size = ImGui::CalcTextSize(text_cstr);

        ImVec2 center = rect.GetCenter();
        ImVec2 text_pos(
            floor(center.x - text_size.x * 0.5f),
            floor(center.y - text_size.y * 0.5f)
        );
        ImU32 text_color = ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Text));

        ImGuiIO& io = ImGui::GetIO();
        auto boldFont = io.Fonts->Fonts[2];
        ImGui::PushFont(boldFont);
		ImGui::GetWindowDrawList()->AddText(text_pos, text_color, text_cstr);
        ImGui::PopFont();
    }

    std::string GetName() const override
    {
        return "Title_Bar";
    }
};
