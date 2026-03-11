#pragma once

#include <string>
#include <memory>

#include "beryl/core/layer.h"
#include "beryl/renderer/texture.h"
#include "beryl/renderer/vertex.h"
#include "beryl/renderer/framebuffer.h"

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

namespace app::layer
{
    class SimpleRender : public beryl::core::Layer
    {
        private:
        std::unique_ptr<beryl::renderer::Texture2D> m_image_texture;
        std::unique_ptr<beryl::renderer::Texture2D> m_mpl_texture;

        std::unique_ptr<beryl::renderer::Vertex> m_vertex;
        std::unique_ptr<beryl::renderer::Framebuffer> m_framebuffer;

        bool m_show_mpl_window = false;

    public:
        SimpleRender();

        void OnAttach() override;
        void OnRender() override;

    };
}