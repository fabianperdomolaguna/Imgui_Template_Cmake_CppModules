#pragma once

#include <array>
#include <cstdint>
#include <string>

#include "glad/gl.h"
#include "GLFW/glfw3.h"

namespace beryl::core
{
    struct WindowSpecification
    {
        std::string title;
        int32_t width;
        int32_t height;
        bool custom_title_bar;
    };

    class Window
    {
        std::array<float, 4> clear_color = { 0.2f, 0.2f, 0.2f, 1.0f };
        float depth_clear = 1.0f;

    public:
        WindowSpecification m_window_specification;
        GLFWwindow* m_glfw_window = nullptr;
        bool m_running = true;
        bool m_close_popup = false;

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;
        Window(Window&&) = delete;
        Window& operator=(Window&&) = delete;

        static void glfw_error_callback(int error, const char* description);

        Window(const WindowSpecification& spec);
        ~Window();

        void PreRender() const;
        void PostRender() const;

        void CloseAppPopup();
    };
}