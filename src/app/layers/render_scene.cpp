module;

#include <format>
#include <string>
#include <memory>

#include "glad/gl.h"
#include "imgui.h"
#include "pybind11/embed.h"

export module RenderScene;

import beryl.core;
import beryl.renderer;
import beryl.logger;
import PythonManager;

namespace py = pybind11;

const std::string vertex_shader_src = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
)";

const std::string fragment_shader_src = R"(
#version 330 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}
)";

const float vertices[] = 
{
	-0.5f, -0.5f, 0.0f, 
	0.5f, -0.5f, 0.0f, 
	0.0f,  0.5f, 0.0f  
};

export class SimpleRender : public beryl::core::Layer
{
    std::unique_ptr<beryl::renderer::Texture2D> image_texture;
    std::unique_ptr<beryl::renderer::Texture2D> mpl_texture;
    std::string m_executable_path;
    std::string m_python_bind_error = "";

    std::unique_ptr<beryl::renderer::Vertex> m_vertex;
    std::unique_ptr<beryl::renderer::Framebuffer> m_framebuffer;

    bool m_show_mpl_window = false;
    
public:
    SimpleRender(std::string executable_path) : Layer("SimpleRender")
    {
        m_executable_path = executable_path;
    }

    void OnAttach() override
    {
        image_texture = std::make_unique<beryl::renderer::Texture2D>(m_executable_path + "/cpp_python_logos.jpg", GL_RGBA, true);

        m_vertex = std::make_unique<beryl::renderer::Vertex>(vertex_shader_src, fragment_shader_src, vertices);
        m_framebuffer = std::make_unique<beryl::renderer::Framebuffer>(1600, 800);
    }

    void GenerateMplTexture()
    {
        try {
            if (!PyMgr.BeginSession(m_executable_path + "/scripts")) 
            {
                beryl::logger::Error("Python session could not be started");
                return;
            }
            
            {
                py::module::import("matplotlib").attr("use")("Agg");
                auto np = PyMgr.ImportModule("numpy");
                auto plt = PyMgr.ImportModule("matplotlib.pyplot");
                auto agg = PyMgr.ImportModule("matplotlib.backends.backend_agg");

                auto add_module = PyMgr.ImportModule("add");
                auto add = add_module.attr("add");

                pybind11::object fig = plt.attr("figure")();
                plt.attr("plot")(np.attr("random").attr("randn")(100));
                plt.attr("title")("Random numbers");

                pybind11::object canvas = agg.attr("FigureCanvasAgg")(fig);
                canvas.attr("draw")();
                py::object renderer = canvas.attr("get_renderer")();
                py::buffer buffer = renderer.attr("buffer_rgba")();
                py::buffer_info info = buffer.request();

                int width, height;
                std::tie(width, height) = py::cast<std::tuple<int, int>>(canvas.attr("get_width_height")());

                mpl_texture = std::make_unique<beryl::renderer::Texture2D>(width, height, static_cast<uint8_t*>(info.ptr), GL_RGBA);
            }
            PyMgr.EndSession();
        } catch (py::error_already_set& e) {
            m_python_bind_error = e.what();
            mpl_texture.reset();
            beryl::logger::Error(std::format("Failed to create matplotlib texture (Python error): {}", m_python_bind_error));
            PyMgr.EndSession();
        } catch (const std::exception& e) {
            m_python_bind_error = e.what();
            mpl_texture.reset();
            beryl::logger::Error(std::format("Failed to create matplotlib texture (C++ error): {}", m_python_bind_error));
            PyMgr.EndSession();
        }
    }

    void OnRender() override
    {
        static float clear_color[4] = { 0.45f, 0.55f, 0.60f, 1.00f };
        static float fraction = 0.0f;
        static int counter = 0;

        ImGui::Begin("Render - Slider - Color - FPS");
        ImGui::SliderFloat("Range 0 to 1", &fraction, 0.0f, 1.0f);
        ImGui::ColorEdit3("Color selection", clear_color);

        if (ImGui::Button("Counter"))
            counter++;
        ImGui::SameLine();
        ImGui::Text("You have clicked the button %d times", counter);

        ImGui::Text("Application average - %.3f ms/frame - (%.1f FPS)", 
            1000.0f / ImGui::GetIO().Framerate, 
            ImGui::GetIO().Framerate);
        
        ImGui::Dummy(ImVec2(0.0f, 20.0f));

        if (ImGui::Button(m_show_mpl_window ? "Close Matplotlib Plot" 
            : "Show Matplotlib Plot")) 
        {
            m_show_mpl_window = !m_show_mpl_window;

            if (m_show_mpl_window && !mpl_texture)
                GenerateMplTexture();
        }
        ImGui::End();

        ImGui::ShowDemoWindow();

        ImGui::Begin("Image Texture");
        ImGui::Image((ImTextureID)(intptr_t)image_texture->GetTextureId(),
            { (float)image_texture->m_width, (float)image_texture->m_height});
        ImGui::End();

        if (m_show_mpl_window)
        {
            ImGui::Begin("Matplotlib Texture", &m_show_mpl_window);
            if (mpl_texture && mpl_texture->GetTextureId() != 0) {
                ImGui::Image((ImTextureID)(intptr_t)mpl_texture->GetTextureId(),
                    { (float)mpl_texture->m_width, (float)mpl_texture->m_height });
            }
            else {
                ImGui::Text("Matplotlib texture not available. Error: %s", m_python_bind_error.c_str());
            }
            ImGui::End();
        }

        m_framebuffer->Bind();
        m_framebuffer->Clear();
        m_vertex->Draw();
        m_framebuffer->Unbind();

        ImGui::Begin("Scene Shader");
        uint64_t textureID = m_framebuffer->GetTextureId();
        ImGui::Image((ImTextureID)(intptr_t)textureID, ImGui::GetContentRegionAvail(), ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
        ImGui::End();
    }
};
