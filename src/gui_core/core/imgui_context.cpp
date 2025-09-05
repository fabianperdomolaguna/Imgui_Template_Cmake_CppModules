module;

#include <unordered_map>
#include <string>
#include <any>

#include "GLFW/glfw3.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"

export module ImguiContext;

import SettingsConfig;
import ColorStyles;

std::unordered_map <std::string, std::any> color_styles{ {"Dark", SetDarkTheme}, {"Light", SetLightTheme} };

export class ImguiContext
{
private:
    void LoadFonts(std::string executable_path, float font_size)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->Clear();

        ImFontConfig fontConfig;
        fontConfig.FontDataOwnedByAtlas = false;
        io.FontDefault = io.Fonts->AddFontFromFileTTF((executable_path + "/fonts/Roboto_Regular.ttf").c_str(), font_size, &fontConfig);

        ImFontConfig fontConfigItalic;
        fontConfigItalic.FontDataOwnedByAtlas = false;
        ImFont* italic = io.Fonts->AddFontFromFileTTF((executable_path + "/fonts/Roboto_Italic.ttf").c_str(), font_size, &fontConfigItalic);

        ImFontConfig fontConfigBold;
        fontConfigBold.FontDataOwnedByAtlas = false;
        ImFont* bold = io.Fonts->AddFontFromFileTTF((executable_path + "/fonts/Roboto_Bold.ttf").c_str(), font_size, &fontConfigBold);

        io.Fonts->Build();
    }

public:
    bool change_font = false;
    float new_font_size = 0.0f;
    std::string m_executable_path;
    std::string m_ini_file_path;

    ImguiContext(GLFWwindow* window, std::string executable_path)
    {
        m_executable_path = executable_path;
        m_ini_file_path = executable_path + "/imgui.ini";

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = m_ini_file_path.c_str();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        float font_size = GetConfigVariable<float>(m_executable_path, "FontSize");
        LoadFonts(m_executable_path, font_size);

        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.WindowBorderSize = 0.0f;
            style.WindowPadding = ImVec2(0.0f, 0.0f);
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        std::string current_style = GetConfigVariable<std::string>(m_executable_path, "GuiStyle");
        std::any_cast <void (*) ()> (color_styles[current_style]) ();

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 400 core");
    }

    ~ImguiContext()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void PreRender()
    {
        if (change_font)
        {
            LoadFonts(m_executable_path, new_font_size);
            ImGui_ImplOpenGL3_DestroyFontsTexture();
            ImGui_ImplOpenGL3_CreateFontsTexture();
            change_font = false;
        }
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
            ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_MenuBar;

        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockspaceWindow", nullptr, window_flags);
        ImGui::PopStyleVar(3);
        ImGuiID dockspace_id = ImGui::GetID("WindowDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
        ImGui::End();
    }

    void PostRender()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        ImGuiIO& io = ImGui::GetIO();

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }
};

export void UpdateTheme(std::string executable_path)
{
    std::string current_style = GetConfigVariable<std::string>(executable_path, "GuiStyle");
    std::any_cast <void (*) ()> (color_styles[current_style]) ();
}
