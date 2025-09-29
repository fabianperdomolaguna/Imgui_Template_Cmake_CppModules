#pragma once

#include <string>
#include <array>

#include "GLFW/glfw3.h"

struct WindowSpecification
{
    std::string title;
    int32_t width;
    int32_t height;
    bool custom_title_bar;
};

class Window
{
    std::array<float, 4> clear_color;

public:
    WindowSpecification m_window_specification;
    GLFWwindow* m_glfw_window;
    bool m_running = true;
    bool m_close_popup = false;

    Window(const WindowSpecification& spec);
    ~Window();

    void PreRender();
    void PostRender();

    void CloseAppPopup();
};