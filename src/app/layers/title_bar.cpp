module;

#include <cmath>
#include <functional>
#include <map>
#include <memory>
#include <string>

#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_internal.h"

export module app.layer.title_bar;

import beryl.core;
import beryl.renderer;
import beryl.config.gui;
import beryl.utils.imgui;
import app.widget.viewportbar;
import app.icons;

struct IconData 
{
    const unsigned char* data;
    unsigned int len;
};

struct TitleBarIcons 
{
    IconData minimize;
    IconData maximize;
    IconData close;
    IconData restore;
};

static const TitleBarIcons light_theme = 
{
    .minimize = { app::icons::minimize_button_light, app::icons::minimize_button_light_len },
    .maximize = { app::icons::maximize_button_light, app::icons::maximize_button_light_len },
    .close    = { app::icons::close_button_light,    app::icons::close_button_light_len    },
    .restore  = { app::icons::restore_button_light,  app::icons::restore_button_light_len  }
};

static const TitleBarIcons dark_theme = 
{
    .minimize = { app::icons::minimize_button_dark,  app::icons::minimize_button_dark_len  },
    .maximize = { app::icons::maximize_button_dark,  app::icons::maximize_button_dark_len  },
    .close    = { app::icons::close_button_dark,     app::icons::close_button_dark_len     },
    .restore  = { app::icons::restore_button_dark,   app::icons::restore_button_dark_len   }
};

export namespace app::layer
{
    class TitleBar : public beryl::core::Layer
    {
        beryl::core::Application* m_app = nullptr;
        ImU32 m_titlebar_background_color = IM_COL32(220, 220, 215, 255);
        bool m_is_light_theme = true;
        bool m_is_resizing = false;

        const float k_title_bar_height = 42.0f;
        const float k_round_corner_radius = 12.0f;
        const float k_button_width = 35.0f;
        const float k_icon_size = 14.0f;
        const int k_border_size = 8;

        std::unique_ptr<beryl::renderer::Texture2D> m_icon_titlebar;
        std::unique_ptr<beryl::renderer::Texture2D> m_minimize_button;
        std::unique_ptr<beryl::renderer::Texture2D> m_maximize_button;
        std::unique_ptr<beryl::renderer::Texture2D> m_close_button;
        std::unique_ptr<beryl::renderer::Texture2D> m_restore_button;

        enum class ResizeDirection {
            None,
            Left,
            Right,
            Bottom,
            BottomLeft,
            BottomRight
        };
        ResizeDirection m_resize_dir = ResizeDirection::None;

        void DrawWindowButton(
        ImVec2& cursor_pos, 
        ImTextureID icon_texture,
        float button_width,
        float button_height,
        float icon_size,
        ImU32 hover_color,
        const std::function<void()>& on_click,
        ImDrawFlags rounding_flags = ImDrawFlags_None,
        float radius = 0.0f)
        {
            ImVec2 button_end = { cursor_pos.x + button_width, cursor_pos.y + button_height };
            ImRect button_rect(cursor_pos, button_end);
            bool hovered = ImGui::IsMouseHoveringRect(button_rect.Min, button_rect.Max);

            auto* draw_list = ImGui::GetBackgroundDrawList();

            if (hovered)
            {
                draw_list->AddRectFilled(
                    button_rect.Min, button_rect.Max, hover_color, radius, rounding_flags);
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && on_click)
                    on_click();
            }

            ImVec2 center = button_rect.GetCenter();
            ImVec2 offset = { icon_size * 0.5f, icon_size * 0.5f };
            ImVec2 icon_pos_min = { std::floor(center.x - offset.x), std::floor(center.y - offset.y) };
            ImVec2 icon_pos_max = { icon_pos_min.x + icon_size, icon_pos_min.y + icon_size };

            draw_list->AddImage(icon_texture, icon_pos_min, icon_pos_max);

            cursor_pos.x += button_width;
        }

        bool IsMaximized(GLFWwindow* window)
        {
            return glfwGetWindowAttrib(window, GLFW_MAXIMIZED) != 0;
        }

    public:
        TitleBar(beryl::core::Application* app) 
            : Layer("TitleBar"), m_app(app) 
        {
            m_app->m_gui_context->AddHeaderHeight(k_title_bar_height);
            m_is_light_theme = (m_app->m_gui_context->m_theme == "Light");
            UpdateTitleBarColor();

            m_icon_titlebar = std::make_unique<beryl::renderer::Texture2D>(
                app::icons::titlebar_logo, app::icons::titlebar_logo_len, GL_RGBA, true);
            
            LoadButtonTextures(true);
        }

        void RefreshTheme()
        {
            m_is_light_theme = (m_app->m_gui_context->m_theme == "Light");
            UpdateTitleBarColor();
            LoadButtonTextures(true);
        }

        void OnRender() override
        {
            auto* viewport = ImGui::GetMainViewport();
            auto* background_draw_list = ImGui::GetBackgroundDrawList();

            HandleBorderResize();

            ImGui::SetNextWindowPos(viewport->Pos);
            ImGui::SetNextWindowSize(viewport->Size);
            ImGui::Begin("##Overlay", nullptr,
                ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs |
                ImGuiWindowFlags_NoBackground);

            MenuBar();

            ImVec2 top_left = viewport->Pos;
            ImVec2 bottom_right = ImVec2(top_left.x + viewport->Size.x, top_left.y + k_title_bar_height);
            ImRect titlebar_rect(top_left, bottom_right);

            bool maximized = IsMaximized(m_app->m_window->m_glfw_window);
            float current_radius = maximized ? 0.0f : k_round_corner_radius;

            background_draw_list->AddRectFilled(
                top_left, bottom_right, m_titlebar_background_color, 
                current_radius, ImDrawFlags_RoundCornersTop);

            DrawCenteredText(m_app->m_window->m_window_specification.title, titlebar_rect);

            ImGui::InvisibleButton("##DragZone", ImVec2(viewport->Size.x, k_title_bar_height));
            if (ImGui::IsMouseHoveringRect(top_left, bottom_right) && ImGui::IsMouseDragging(0))
            {
                int wx, wy;
                glfwGetWindowPos(m_app->m_window->m_glfw_window, &wx, &wy);
                ImVec2 delta = ImGui::GetIO().MouseDelta;
                glfwSetWindowPos(m_app->m_window->m_glfw_window, wx + static_cast<int>(delta.x),
                    wy + static_cast<int>(delta.y));
            }

            float logo_size = k_title_bar_height * 0.8f;
            const ImVec2 logo_pos(top_left.x + k_title_bar_height * 0.3, top_left.y + k_title_bar_height * 0.1);
            background_draw_list->AddImage(
                beryl::utils::imgui::GetImID(m_icon_titlebar->GetTextureId()),
                logo_pos,
                ImVec2(logo_pos.x + logo_size, logo_pos.y + logo_size)
            );

            ImVec2 button_cursor = { top_left.x + viewport->Size.x - (k_button_width * 3),
                top_left.y };

            DrawWindowButton(
                button_cursor,
                beryl::utils::imgui::GetImID(m_minimize_button->GetTextureId()),
                k_button_width,
                k_title_bar_height,
                k_icon_size,
                IM_COL32(154, 140, 152, 128),
                [&]() { glfwIconifyWindow(m_app->m_window->m_glfw_window); }
            );

            uint32_t maximize_icon_texture = maximized
                                            ? m_restore_button->GetTextureId()
                                            : m_maximize_button->GetTextureId();

            DrawWindowButton(
                button_cursor,
                beryl::utils::imgui::GetImID(maximize_icon_texture),
                k_button_width,
                k_title_bar_height,
                k_icon_size,
                IM_COL32(154, 140, 152, 128),
                [&]() {
                    if (maximized)
                        glfwRestoreWindow(m_app->m_window->m_glfw_window);
                    else
                        glfwMaximizeWindow(m_app->m_window->m_glfw_window);
                }
            );

            DrawWindowButton(
                button_cursor,
                beryl::utils::imgui::GetImID(m_close_button->GetTextureId()),
                k_button_width,
                k_title_bar_height,
                k_icon_size,
                IM_COL32(255, 0, 0, 128),
                [&]() { m_app->m_window->m_close_popup = true; },
                maximized ? ImDrawFlags_None : ImDrawFlags_RoundCornersTopRight, current_radius
            );

            ImGui::End();
        }

    private:
        void MenuBar()
        {
            ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_MenuBarBg, IM_COL32(0, 0, 0, 0));

            float menu_offset_x = k_title_bar_height + 15.0f;

            if (app::widget::BeginViewportBar("##TitleBarMenuBar", ImGui::GetMainViewport(),
                ImVec2{ menu_offset_x, k_title_bar_height * 0.5f - ImGui::GetFrameHeight() * 0.5f }, 
                ImVec2{ 0.0f, ImGui::GetFrameHeight() },
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | 
                ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_MenuBar))
            {
                ImGui::BeginMenuBar();
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("Exit", "Alt+F4"))
                        m_app->m_window->m_close_popup = true;

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Edit"))
                {
                    ImGui::MenuItem("Copy", "Ctrl + C");

                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();

                ImGui::End();
            }
            ImGui::PopStyleColor(2);
        }

        void HandleBorderResize()
        {
            if (IsMaximized(m_app->m_window->m_glfw_window)) 
                return;

            GLFWwindow* window = m_app->m_window->m_glfw_window;
            double mouse_x_raw, mouse_y_raw;
            glfwGetCursorPos(window, &mouse_x_raw, &mouse_y_raw);

            const int mouse_x = static_cast<int>(mouse_x_raw);
            const int mouse_y = static_cast<int>(mouse_y_raw);

            int width, height, window_x, window_y;
            glfwGetWindowSize(window, &width, &height);
            glfwGetWindowPos(window, &window_x, &window_y);

            if (m_is_resizing == false) 
            {
                const bool left =
                    (mouse_x < k_border_size) && (mouse_y > static_cast<int>(k_title_bar_height));
                const bool right = (mouse_x > width - k_border_size) &&
                    (mouse_y > static_cast<int>(k_title_bar_height));
                const bool bottom = (mouse_y > height - k_border_size);

                if (right && bottom) 
                    ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNWSE);
                else if (left && bottom) 
                    ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNESW);
                else if (left || right) 
                    ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
                else if (bottom) 
                    ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);

                if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) 
                {
                    if (left && bottom)       
                        m_resize_dir = ResizeDirection::BottomLeft;
                    else if (right && bottom) 
                        m_resize_dir = ResizeDirection::BottomRight;
                    else if (left)            
                        m_resize_dir = ResizeDirection::Left;
                    else if (right)           
                        m_resize_dir = ResizeDirection::Right;
                    else if (bottom)          
                        m_resize_dir = ResizeDirection::Bottom;
                    
                    if (m_resize_dir != ResizeDirection::None)
                        m_is_resizing = true;
                }
            }

            if (m_is_resizing) 
            {
                if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) 
                {
                    m_is_resizing = false;
                    m_resize_dir = ResizeDirection::None;
                    return;
                }

                if (m_resize_dir == ResizeDirection::Left || m_resize_dir == ResizeDirection::Right)
                    ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
                else if (m_resize_dir == ResizeDirection::Bottom)
                    ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);

                int new_width = width;
                int new_height = height;
                int new_x = window_x;
                const int new_y = window_y;

                switch (m_resize_dir)
                {
                    case ResizeDirection::Right:
                        new_width = mouse_x;
                        break;
                    case ResizeDirection::Bottom:
                        new_height = mouse_y;
                        break;
                    case ResizeDirection::Left:
                        new_width = width - mouse_x;
                        new_x = window_x + mouse_x;
                        break;
                    case ResizeDirection::BottomRight:
                        new_width = mouse_x;
                        new_height = mouse_y;
                        break;
                    case ResizeDirection::BottomLeft:
                        new_width = width - mouse_x;
                        new_x = window_x + mouse_x;
                        new_height = mouse_y;
                        break;
                    default: break;
                }

                if (new_width < 400) 
                {
                    if (m_resize_dir == ResizeDirection::Left || m_resize_dir == ResizeDirection::BottomLeft)
                        new_x = window_x + (width - 400);
                    new_width = 400;
                }
                
                new_height = std::max(new_height, 200);

                if (new_width != width || new_height != height) 
                {
                    if (new_x != window_x) 
                        glfwSetWindowPos(window, new_x, new_y);
                    glfwSetWindowSize(window, new_width, new_height);
                }
            }
        }

        void UpdateTitleBarColor()
        {
            m_titlebar_background_color =
                m_titlebar_background_color = m_is_light_theme
                ? IM_COL32(222, 220, 215, 255)
                : IM_COL32(40, 40, 40, 255);
        }

        void LoadButtonTextures(bool reload)
        {
            const TitleBarIcons& icons = m_is_light_theme ? light_theme : dark_theme;

            auto load_helper = [&](std::unique_ptr<beryl::renderer::Texture2D>& texture,
                const IconData& icon) -> void
            {
                if (reload && texture)
                    texture->Reload(icon.data, icon.len);
                else
                    texture = std::make_unique<beryl::renderer::Texture2D>(
                        icon.data, icon.len, GL_RGBA, true);
            };

            load_helper(m_minimize_button, icons.minimize);
            load_helper(m_maximize_button, icons.maximize);
            load_helper(m_close_button, icons.close);
            load_helper(m_restore_button, icons.restore);
        }

        void DrawCenteredText(std::string& text, const ImRect& rect)
        {
            auto* boldFont = ImGui::GetIO().Fonts->Fonts[2];
            ImGui::PushFont(boldFont);

            const char* text_cstr = text.c_str();
            ImVec2 text_size = ImGui::CalcTextSize(text_cstr, nullptr);

            ImVec2 center = rect.GetCenter();
            ImVec2 text_pos(
                std::floor(center.x - (text_size.x * 0.5f)),
                std::floor(center.y - (text_size.y * 0.5f))
            );
            ImU32 text_color = ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Text));
            ImGui::GetWindowDrawList()->AddText(text_pos, text_color, text_cstr);
            
            ImGui::PopFont();
        }
    };
}