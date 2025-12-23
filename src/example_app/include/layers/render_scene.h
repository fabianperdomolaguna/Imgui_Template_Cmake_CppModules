#pragma once

#include <string>
#include <memory>

#include "core/layer.h"
#include "image/image_reader.h"
#include "renderer/texture.h"
#include "renderer/vertex.h"
#include "renderer/framebuffer.h"

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

class SimpleRender : public Layer
{
    std::unique_ptr<ImageTexture> image_texture;
    std::unique_ptr<Texture> mpl_texture;
    std::string m_executable_path;
    std::string m_python_error = "";

    std::unique_ptr<GlVertex> m_vertex;
    std::unique_ptr<GlFramebuffer> m_framebuffer;
    
public:
    SimpleRender(std::string executable_path);

    void OnAttach() override;
    void OnRender() override;
    std::string GetName() const override;
};