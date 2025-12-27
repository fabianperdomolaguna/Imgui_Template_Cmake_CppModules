#include <format>
#include <string>
#include <memory>

#include "glad/gl.h"
#include "imgui.h"
#include "pybind11/embed.h"

#include "render_scene.h"
#include "python_manager.h"
#include "core/layer.h"
#include "logging/logger.h"
#include "image/image_reader.h"
#include "renderer/texture.h"
#include "renderer/framebuffer.h"

namespace py = pybind11;
auto& PyMgr = PythonManager::PyMgr();

SimpleRender::SimpleRender(std::string executable_path)
{
    m_executable_path = executable_path;
}

void SimpleRender::OnAttach()
{
    image_texture = std::make_unique<ImageTexture>(m_executable_path + "/cpp_python_logos.jpg", GL_RGBA, true);

    try {
        py::module::import("matplotlib").attr("use")("Agg");
        auto np = PyMgr.ImportModule("numpy");
        auto plt = PyMgr.ImportModule("matplotlib.pyplot");
        auto agg = PyMgr.ImportModule("matplotlib.backends.backend_agg");

        PyMgr.AddSystemPath(m_executable_path + "/scripts");
        auto add_module = PyMgr.ImportModule("add");

        auto add = add_module.attr("add");

        pybind11::object fig = plt.attr("figure")();
        plt.attr("plot")(np.attr("random").attr("randn")(100));
        plt.attr("title")("Random numbers");

        int width, height;
        pybind11::object canvas = agg.attr("FigureCanvasAgg")(fig);
        canvas.attr("draw")();
        py::object renderer = canvas.attr("get_renderer")();
        py::buffer buffer = renderer.attr("buffer_rgba")();
        py::buffer_info info = buffer.request();
        std::tie(width, height) = py::cast<std::tuple<int, int>>(canvas.attr("get_width_height")());

        mpl_texture = std::make_unique<Texture>(static_cast<uint8_t*>(info.ptr), width, height, GL_RGBA);
    } catch (py::error_already_set& e) {
        m_python_bind_error = e.what();
        mpl_texture.reset();
        LOG_ERROR(std::format("Failed to create matplotlib texture (Python error): {}", m_python_bind_error));
    } catch (const std::exception& e) {
        m_python_bind_error = e.what();
        mpl_texture.reset();
        LOG_ERROR(std::format("Failed to create matplotlib texture (C++ error): {}", m_python_bind_error));
    }

    m_vertex = std::make_unique<GlVertex>(vertex_shader_src, fragment_shader_src);
    m_framebuffer = std::make_unique<GlFramebuffer>(1600, 800);
    m_vertex->CreateBuffers();
}

void SimpleRender::OnRender()
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
    ImGui::End();

    ImGui::ShowDemoWindow();

    ImGui::Begin("Image Texture");
    ImGui::Image((ImTextureID)(intptr_t)image_texture->get_texture(),
        { (float)image_texture->m_width, (float)image_texture->m_height});
    ImGui::End();

    ImGui::Begin("Matplotlib Texture");
    if (mpl_texture && mpl_texture->get_texture() != 0) {
        ImGui::Image((ImTextureID)(intptr_t)mpl_texture->get_texture(),
            { (float)mpl_texture->m_width, (float)mpl_texture->m_height });
    }
    else {
        ImGui::Text("Matplotlib texture not available. Error: %s", m_python_bind_error.c_str());
    }
    ImGui::End();

    m_framebuffer->Bind();
    m_vertex->Draw();
    m_framebuffer->Unbind();

    ImGui::Begin("Scene Shader");
    uint64_t textureID = m_framebuffer->get_texture();
    ImGui::Image((ImTextureID)(intptr_t)textureID, ImGui::GetContentRegionAvail(), ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
    ImGui::End();
}

std::string SimpleRender::GetName() const
{
    return "Simple_Render";
}