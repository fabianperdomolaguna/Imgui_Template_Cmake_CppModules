module;

#include <string>

#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_internal.h"

export module TitleBar;

import Layer;
import Application;

export class TitleBar : public Layer
{
    Application* m_app;
	float title_bar_height = 42.0f;

public:
    TitleBar(Application* app) : m_app(app) 
    {
        m_app->m_imgui_context->header_height.push_back(title_bar_height);
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
    }

    std::string GetName() const override
    {
        return "Title_Bar";
    }
};
