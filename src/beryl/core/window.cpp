module;

#include <array>
#include <cstdint>
#include <string>

#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "imgui.h"

export module beryl.core:window;

import beryl.utils.gl;
import beryl.logger;

export namespace beryl::core
{

    struct WindowSpecification
    {
        std::string title;
        std::int32_t width = 0;
        std::int32_t height = 0;
        bool custom_title_bar = false;
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

        static void glfw_error_callback(int error, const char* description)
        {
            beryl::logger::Error("GLFW Error", "code", error, "description", description);
        }

        Window(const WindowSpecification& spec) : m_window_specification(spec)
        {
            glfwSetErrorCallback(glfw_error_callback);

            if (glfwInit() == 0)
            {
                beryl::logger::Critical("Failed to initialize GLFW");
                m_running = false;
                return;
            }

            if (m_window_specification.custom_title_bar)
            {
                glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
                glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
                clear_color = { 0.0f, 0.0f, 0.0f, 0.0f };
            }

            m_glfw_window = glfwCreateWindow(m_window_specification.width,
                m_window_specification.height, m_window_specification.title.c_str(), nullptr, nullptr);

            if (m_glfw_window == nullptr)
            {
                beryl::logger::Critical("Could not initialize a GLFW window");
                m_running = false;
                return;
            }

            glfwMakeContextCurrent(m_glfw_window);
            glfwSwapInterval(0);
            glfwSetWindowUserPointer(m_glfw_window, this);

            glfwSetWindowCloseCallback(m_glfw_window, [](GLFWwindow* window)
            {
                glfwSetWindowShouldClose(window, GLFW_FALSE);
                auto* window_app = static_cast<Window*>(glfwGetWindowUserPointer(window));
                window_app->m_close_popup = true;
            });

            glfwSetFramebufferSizeCallback(m_glfw_window, [](GLFWwindow* window, int width, int height)
            {
                auto* window_app = static_cast<Window*>(glfwGetWindowUserPointer(window));
                window_app->m_window_specification.width = width;
                window_app->m_window_specification.height = height;
            });

            if (gladLoadGL((GLADloadfunc)glfwGetProcAddress) == 0)
            {
                beryl::logger::Critical("Failed to initialize GLAD");
                m_running = false;
                return;
            }

            if (m_running) 
            {
                beryl::utils::gl::EnableOpenGLDebug();
                glEnable(GL_DEPTH_TEST);
            }
        }

        ~Window()
        {
            if (m_glfw_window != nullptr)
                glfwDestroyWindow(m_glfw_window);
            glfwTerminate();
        }

        void PreRender() const
        {
            glViewport(0, 0, m_window_specification.width, m_window_specification.height);
            glClearBufferfv(GL_COLOR, 0, clear_color.data());
            glClearBufferfv(GL_DEPTH, 0, &depth_clear);
        }

        void PostRender() const
        {
            glfwPollEvents();
            glfwSwapBuffers(m_glfw_window);
        }

        void CloseAppPopup()
        {
            if (m_close_popup)
            {
                ImGui::OpenPopup("Close the application?");
                m_close_popup = false;
            }

            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

            if (ImGui::BeginPopupModal("Close the application?", nullptr))
            {
                ImGui::Text("Are you sure to close the application?\nActive edits will not be saved!\n\n");
                ImGui::Separator();

                if (ImGui::Button("Ok", ImVec2(120, 0)))
                    m_running = false;

                ImGui::SameLine();

                if (ImGui::Button("Cancel", ImVec2(120, 0)))
                {
                    ImGui::CloseCurrentPopup();
                    m_close_popup = false;
                }

                ImGui::EndPopup();
            }
        }
    };
}