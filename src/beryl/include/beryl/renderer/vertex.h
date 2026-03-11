#pragma once

#include <memory>
#include <span>
#include <string>

#include "beryl/renderer/shader.h"

namespace beryl::renderer
{
    class Vertex
	{
	private:
		uint32_t m_VAO = 0;
		uint32_t m_VBO = 0;
		uint32_t m_VertexCount = 0;

        std::unique_ptr<beryl::renderer::Shader> m_shader;

    public:
		Vertex(const std::string& vertex_path, const std::string& fragment_path,
			std::span<const float> vertices);

        Vertex(const Vertex&) = delete;
		Vertex& operator=(const Vertex&) = delete;

        ~Vertex();

        void SetupBuffers(std::span<const float> vertices);
        void Draw() const;
    };
}