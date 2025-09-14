#pragma once

#include <string>
#include <array>

#include "GLFW/glfw3.h"

class Window
{
    std::string m_title;
    uint16_t m_width;
    uint16_t m_height;
    std::array<float, 4> clear_color;

public:
    GLFWwindow* m_window;
    bool m_running = true;
    bool m_close_popup = false;

    Window(std::string title, int32_t width, int32_t height, bool custom_title_bar);
    ~Window();

    void PreRender();
    void PostRender();

    void CloseAppPopup();
};