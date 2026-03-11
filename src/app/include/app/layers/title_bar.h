#pragma once

#include <memory>
#include <functional>

#include "beryl/core/application.h"
#include "beryl/core/layer.h"
#include "beryl/renderer/texture.h"
#include "app/icons/window_buttons.h"

#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_internal.h"

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

namespace app::layer
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
            float radius = 0.0f);
        
        bool IsMaximized(GLFWwindow* window);

    public:
        TitleBar(beryl::core::Application* app); 

        void RefreshTheme();
        void OnRender() override;

    private:
        void MenuBar();
        void HandleBorderResize();
        void UpdateTitleBarColor();
        void LoadButtonTextures(bool reload);
        void DrawCenteredText(std::string& text, const ImRect& rect);
    };
}