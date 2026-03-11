#pragma once

#include <array>
#include <filesystem>
#include <vector>

#include "GLFW/glfw3.h"

#include "beryl/gui/themes.h"

namespace beryl::gui
{
    class Context
    {
    private:
        std::string m_ini_file_path;
        ImGuiWindowFlags m_dockspace_flags = 0;

        void LoadFonts(std::filesystem::path& executable_path, float font_size);

    public:
        static constexpr std::array<std::pair<std::string, void(*)()>, 2> m_themes_map
        {
            std::pair{ "Dark",  beryl::themes::SetDarkTheme },
            std::pair{ "Light", beryl::themes::SetLightTheme }
        };

        std::string m_theme;
        float m_font_size = 0.0f;

        std::vector<float> m_header_height;
        float m_header_total = 0.0f;

        Context(const Context&) = delete;
        Context& operator=(const Context&) = delete;
        Context(Context&&) = delete;
        Context& operator=(Context&&) = delete;

        void UpdateDockspaceFlags(const bool custom_title_bar);

        Context(GLFWwindow* window);
        ~Context();

        void PreRender() const;
        void PostRender() const;
        void ChangeFontSize() const;
        void UpdateTheme() const;
        void AddHeaderHeight(float height);
    };
}