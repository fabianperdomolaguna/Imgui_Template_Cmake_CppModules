module;

#include <iostream>
#include <string>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "imgui.h"

export module Window;

export class Window
{
    std::string m_title;
    uint16_t m_width;
    uint16_t m_height;

public:
    GLFWwindow* m_window;
    bool m_running = true;
    bool m_close_popup = false;

    Window(std::string title, int32_t width, int32_t height)
    {
        m_title = title;
        m_width = width;
        m_height = height;

        if (!glfwInit())
        {
            std::cout << "Could not intialize GLFW!\n";
            m_running = false;
        }

        m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
        glfwMakeContextCurrent(m_window);

        if (!m_window)
        {
            std::cout << "Could not intialize a window!\n";
            m_running = false;
        }

        glfwSetWindowUserPointer(m_window, this);

        glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window)
        {
            Window& window_app = *(Window*)glfwGetWindowUserPointer(window);
            window_app.m_close_popup = true;
        });

        glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int width, int height)
        {
            Window& window_app = *(Window*)glfwGetWindowUserPointer(window);
            window_app.m_width = width;
            window_app.m_height = height;
        });

        gladLoadGL();
        glEnable(GL_DEPTH_TEST);
    }

    ~Window()
    {
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }

    void PreRender()
    {
        glViewport(0, 0, m_width, m_height);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void PostRender()
    {
        glfwPollEvents();
        glfwSwapBuffers(m_window);
    }

    void CloseAppPopup()
    {
        ImGui::OpenPopup("Close the application?");

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("Close the application?", NULL))
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