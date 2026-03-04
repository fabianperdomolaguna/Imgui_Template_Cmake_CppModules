module;

#include <array>
#include <filesystem>
#include <format>
#include <numeric>
#include <optional>
#include <ranges>
#include <string>
#include <vector>

#include "GLFW/glfw3.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

export module beryl.gui:context;

import :themes;
import :publicsansfont;
import beryl.config.gui;
import beryl.utils.path;
import beryl.logger;

export namespace beryl::gui
{
    class Context
    {
    private:
        std::string m_ini_file_path;
        ImGuiWindowFlags m_dockspace_flags = 0;

        void LoadFonts(std::filesystem::path& executable_path, float font_size)
        {
            ImGuiIO& io = ImGui::GetIO();
            io.Fonts->Clear();

            const std::filesystem::path fonts_dir = executable_path / "fonts";

            auto load_from_file = [&](const std::string& filename, bool is_default = false) 
            {
                std::filesystem::path font_path = fonts_dir / filename;
                ImFontConfig config;

                ImFont* font = nullptr;
                if (std::filesystem::exists(font_path))
                {
                    font = io.Fonts->AddFontFromFileTTF(font_path.string().c_str(), font_size, &config);
                }
                else
                {
                    beryl::logger::Error(std::format(
                        "Font {} not found at {}. Using ImGui default.", filename, fonts_dir.string()));
                    font = io.Fonts->AddFontFromMemoryCompressedTTF(
                        static_cast<const void*>(std::data(PublicSans_compressed_data)),
                        static_cast<int>(std::size(PublicSans_compressed_data)),
                        font_size,
                        &config);
                }

                if (is_default) io.FontDefault = font;

                return font;
            };

            load_from_file("Roboto_Regular.ttf", true);
            load_from_file("Roboto_Italic.ttf");
            load_from_file("Roboto_Bold.ttf"); 
        }

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

        void UpdateDockspaceFlags(const bool custom_title_bar) {
            m_dockspace_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
                ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

            if (!custom_title_bar)
                m_dockspace_flags |= ImGuiWindowFlags_MenuBar;
        }

        Context(GLFWwindow* window)
        {
            std::filesystem::path executable_path = beryl::utils::GetExecutablePath();
            m_ini_file_path = (executable_path / "imgui.ini").string();

            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO();
            io.IniFilename = m_ini_file_path.c_str();
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
            io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

            m_font_size = beryl::config::gui::Get<float>(executable_path, "FontSize").value_or(18.0f);
            LoadFonts(executable_path, m_font_size);

            ImGuiStyle& style = ImGui::GetStyle();
            style.WindowRounding = 0.0f;
            style.WindowBorderSize = 0.0f;
            style.WindowPadding = ImVec2(0.0f, 0.0f);
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;

            m_theme = beryl::config::gui::Get<std::string>(executable_path, "GuiStyle").value_or("Light");
            UpdateTheme();

            ImGui_ImplGlfw_InitForOpenGL(window, true);
            ImGui_ImplOpenGL3_Init("#version 430 core");
        }

        ~Context()
        {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
        }

        void PreRender() const
        {        
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + m_header_total));
            ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, viewport->Size.y - m_header_total));
            ImGui::SetNextWindowViewport(viewport->ID);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGui::Begin("DockspaceWindow", nullptr, m_dockspace_flags);
            ImGuiID dockspace_id = ImGui::GetID("WindowDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
            ImGui::End();
            ImGui::PopStyleVar(3);
        }

        void PostRender() const
        {
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            ImGuiIO& io = ImGui::GetIO();

            if ((io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) != 0)
            {
                GLFWwindow* backup_current_context = glfwGetCurrentContext();
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                glfwMakeContextCurrent(backup_current_context);
            }
        }

        void ChangeFontSize() const
        {
            ImGui::GetStyle()._NextFrameFontSizeBase = m_font_size;
        }

        void UpdateTheme() const
        {
            const auto it = std::ranges::find_if(m_themes_map,
                [this](const auto& p) { return p.first == m_theme; });

            if (it != m_themes_map.end())
                it->second();
        }

        void AddHeaderHeight(float height)
        {
            m_header_height.push_back(height);
            m_header_total = std::accumulate(m_header_height.begin(), m_header_height.end(), 0.0f);
        }
    };
}