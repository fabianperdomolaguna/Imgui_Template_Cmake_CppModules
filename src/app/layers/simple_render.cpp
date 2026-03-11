#include "app/layers/simple_render.h"

#include <filesystem>

#include "glad/gl.h"
#include "imgui.h"

#include "beryl/utils/path_utils.h"
#include "beryl/utils/imgui_utils.h"
#include "app/visualization/random_plot.h"

app::layer::SimpleRender::SimpleRender() : Layer("SimpleRender") {}

void app::layer::SimpleRender::OnAttach()
{
    std::filesystem::path executable_path = beryl::utils::path::GetExecutablePath();
    m_image_texture = std::make_unique<beryl::renderer::Texture2D>(
        (executable_path / "cpp_python_logos.jpg").string(), GL_RGBA, true);

    m_vertex = std::make_unique<beryl::renderer::Vertex>(
        vertex_shader_src, fragment_shader_src, vertices);
    m_framebuffer = std::make_unique<beryl::renderer::Framebuffer>(1600, 800);
}

void app::layer::SimpleRender::OnRender()
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
        1000.0F / ImGui::GetIO().Framerate, 
        ImGui::GetIO().Framerate);
    
    ImGui::Dummy(ImVec2(0.0f, 20.0f));

    if (ImGui::Button(m_show_mpl_window ? "Close Matplotlib Plot" 
        : "Show Matplotlib Plot")) 
    {
        m_show_mpl_window = !m_show_mpl_window;

        if (!m_show_mpl_window)
            m_mpl_texture.reset();

        if (m_show_mpl_window)
        {
            auto [pixels, width, height, success] = app::visualization::MplRandomPlot();
            if (success)
                m_mpl_texture = std::make_unique<beryl::renderer::Texture2D>(
                    width, height, pixels.data(), GL_RGBA);
        }
    }
    ImGui::End();

    ImGui::ShowDemoWindow();

    ImGui::Begin("Image Texture");
    ImGui::Image(
        beryl::utils::imgui::GetImID(m_image_texture->GetTextureId()),
        { static_cast<float>(m_image_texture->m_width),
            static_cast<float>(m_image_texture->m_height) }
    );
    ImGui::End();

    if (m_show_mpl_window)
    {
        ImGui::Begin("Matplotlib Texture", &m_show_mpl_window);
        if (m_mpl_texture && m_mpl_texture->GetTextureId() != 0) 
        {
            ImGui::Image(
                beryl::utils::imgui::GetImID(m_mpl_texture->GetTextureId()),
                { static_cast<float>(m_mpl_texture->m_width), 
                    static_cast<float>(m_mpl_texture->m_height) }
            );
        }
        else {
            ImGui::Text("Matplotlib texture not available");
        }
        ImGui::End();
    }

    m_framebuffer->Bind();
    m_framebuffer->Clear();
    m_vertex->Draw();
    m_framebuffer->Unbind();

    ImGui::Begin("Scene Shader");
    ImGui::Image(
        beryl::utils::imgui::GetImID(m_framebuffer->GetTextureId()), 
        ImGui::GetContentRegionAvail(),
        ImVec2{ 0.0f, 1.0f }, 
        ImVec2{ 1.0f, 0.0f }
    );
    ImGui::End();
}