#include <iostream>
#include <string>
#include <memory>

#include "glad/gl.h"
#include "imgui.h"
#include "pybind11/embed.h"

#include "render_scene.h"
#include "core/layer.h"
#include "image/image_reader.h"
#include "renderer/texture.h"
#include "renderer/framebuffer.h"

namespace py = pybind11;

SimpleRender::SimpleRender(std::string executable_path)
{
    m_executable_path = executable_path;
}

void SimpleRender::OnAttach()
{
    image_texture = std::make_unique<ImageTexture>(m_executable_path + "/cpp_python_logos.jpg", GL_RGBA, true);

    try {
        py::module::import("matplotlib").attr("use")("Agg");
        py::module np = py::module::import("numpy");
        py::module plt = py::module::import("matplotlib.pyplot");
        py::module agg = py::module::import("matplotlib.backends.backend_agg");

        py::module sys = py::module::import("sys");
        sys.attr("path").attr("insert")(0, m_executable_path + "/scripts");
        py::module add_module = py::module::import("add");

        auto add = add_module.attr("add");
        std::cout << "Add result from Python script: " << py::cast<int>(add(2, 3, 5)) << std::endl;

        pybind11::object fig = plt.attr("figure")();
        plt.attr("plot")(np.attr("random").attr("randn")(100));
        plt.attr("title")("Random numbers");

        int width, height;
        pybind11::object canvas = agg.attr("FigureCanvasAgg")(fig);
        canvas.attr("draw")();
        std::string data = py::cast<std::string>(canvas.attr("get_renderer")().attr("buffer_rgba")().attr("tobytes")());
        uint8_t* data_ptr = reinterpret_cast<uint8_t*>(const_cast<char*>(data.data()));
        std::tie(width, height) = py::cast<std::tuple<int, int>>(canvas.attr("get_width_height")());

        mpl_texture = std::make_unique<Texture>(data_ptr, width, height, GL_RGBA);

    } catch (py::error_already_set& err) {
        std::cout << err.what() << std::endl;
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
    ImGui::Image(reinterpret_cast<void*>((uint64_t)image_texture->get_texture()), 
        { (float)image_texture->m_width, (float)image_texture->m_height});
    ImGui::End();

    ImGui::Begin("Matplotlib Texture");
    ImGui::Image(reinterpret_cast<void*>((uint64_t)mpl_texture->get_texture()), 
        { (float)mpl_texture->m_width, (float)mpl_texture->m_height});
    ImGui::End();

    m_framebuffer->Bind();
    m_vertex->Draw();
    m_framebuffer->Unbind();

    ImGui::Begin("Scene Shader");
    uint64_t textureID = m_framebuffer->get_texture();
    ImGui::Image(reinterpret_cast<void*>(textureID), ImGui::GetContentRegionAvail(), ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
    ImGui::End();
}

std::string SimpleRender::GetName() const
{
    return "Simple_Render";
}