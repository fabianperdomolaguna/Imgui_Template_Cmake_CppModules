#pragma once

#include <string>
#include <vector>

#include "GLFW/glfw3.h"

class ImguiContext
{
private:
    void LoadFonts(std::string executable_path, float font_size);

public:
    std::string color_style = "";
    float font_size = 0.0f;
    bool change_font = false;
    float new_font_size = 0.0f;
    std::string m_executable_path;
    std::string m_ini_file_path;

    std::vector<float> header_height;

    ImguiContext(GLFWwindow* window, std::string executable_path);
    ~ImguiContext();

    void PreRender(bool custom_title_bar);
    void PostRender();
};

void UpdateTheme(std::string executable_path, std::string& color_style);