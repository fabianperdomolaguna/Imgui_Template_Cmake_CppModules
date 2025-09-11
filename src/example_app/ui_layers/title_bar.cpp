module;

#include <string>
#include <memory>
#include <functional>

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
	float button_size,
	float icon_size,
    ImU32 button_color,
	float horizontal_offset,
    const std::function<void()>& on_click = nullptr)
{
    ImRect button_rect(button_start, button_end);
    bool button_hovered = button_rect.Contains(ImGui::GetMousePos());
    if (button_hovered)
        ImGui::GetBackgroundDrawList()->AddRectFilled(button_start, button_end, button_color);
    if (button_hovered && ImGui::IsMouseClicked(0))
		on_click();

    ImVec2 center = button_rect.GetCenter();
    ImVec2 icon_start = ImVec2(center.x - icon_size * 0.5f, center.y - icon_size * 0.5f);
    ImVec2 icon_end = ImVec2(icon_start.x + icon_size, icon_start.y + icon_size);
    ImGui::GetBackgroundDrawList()->AddImage(
        icon_texture,
        icon_start,
        icon_end
    );

	button_start = ImVec2(button_start.x + button_size + horizontal_offset, button_start.y);
    button_end = ImVec2(button_end.x + button_size + horizontal_offset, button_end.y);
}

bool IsMaximized(GLFWwindow* window)
{
    return (bool)glfwGetWindowAttrib(window, GLFW_MAXIMIZED);
}

export class TitleBar : public Layer
{
    Application* m_app;
	float title_bar_height = 42.0f;

    std::unique_ptr<ImageTexture> icon_titlebar;
    std::unique_ptr<ImageTexture> minimize_button;
    std::unique_ptr<ImageTexture> maximize_button;
    std::unique_ptr<ImageTexture> close_button;

public:
    TitleBar(Application* app) : m_app(app) 
    {
        m_app->m_imgui_context->header_height.push_back(title_bar_height);

        icon_titlebar = std::make_unique<ImageTexture>(g_icon_titlebar, g_icon_titlebar_len, GL_RGBA, true);
        minimize_button = std::make_unique<ImageTexture>(g_minimize_button, g_minimize_button_len, GL_RGBA, true);
        maximize_button = std::make_unique<ImageTexture>(g_maximize_button, g_maximize_button_len, GL_RGBA, true);
        close_button = std::make_unique<ImageTexture>(g_close_button, g_close_button_len, GL_RGBA, true);
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

        ImGui::GetBackgroundDrawList()->AddRectFilled(
            ImVec2(ImGui::GetMainViewport()->Pos.x, ImGui::GetMainViewport()->Pos.y + 42.0f),
            ImVec2(ImGui::GetMainViewport()->Pos.x + ImGui::GetMainViewport()->Size.x,
                ImGui::GetMainViewport()->Pos.y + ImGui::GetMainViewport()->Size.y),
            IM_COL32(51, 51, 51, 255)
        );

        ImVec2 top_left = ImGui::GetMainViewport()->Pos;
        ImVec2 bottom_right = ImVec2(top_left.x + ImGui::GetMainViewport()->Size.x, top_left.y + 42.0f);
        ImGui::GetBackgroundDrawList()->AddRectFilled(
            top_left,
            bottom_right,
            IM_COL32(222, 220, 215, 255),
            12.0f,
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

        float horizontal_offset = 0.5f;
        float button_size = 35.0f;
        float icon_size = 14.0f;

        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 button_start = ImVec2(viewport->Pos.x + viewport->Size.x - button_size * 3 - horizontal_offset * 2, viewport->Pos.y);
		ImVec2 button_end = ImVec2(button_start.x + button_size, button_start.y + title_bar_height);

        DrawWindowButtons(
            button_start,
            button_end,
            (ImTextureID)(intptr_t)minimize_button->get_texture(),
            button_size,
            icon_size,
            IM_COL32(154, 140, 152, 128),
            horizontal_offset,
            [&]() { glfwIconifyWindow(m_app->m_window->m_window); }
        );

        DrawWindowButtons(
            button_start,
            button_end,
            (ImTextureID)(intptr_t)maximize_button->get_texture(),
            button_size,
            icon_size,
            IM_COL32(154, 140, 152, 128),
            horizontal_offset,
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
            horizontal_offset,
            [&]() { m_app->m_window->m_close_popup = true; }
        );
    }

    std::string GetName() const override
    {
        return "Title_Bar";
    }
};
