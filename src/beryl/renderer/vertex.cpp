module;

#include <cstdint>
#include <memory>
#include <span>
#include <string>

#include "glad/gl.h"

export module beryl.renderer:vertex;

import :shader;
import beryl.utils.gl;
import beryl.logger;

export namespace beryl::renderer
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
			std::span<const float> vertices)
		{
			m_shader = std::make_unique<beryl::renderer::Shader>();
			m_shader->Create(vertex_path, fragment_path);

			SetupBuffers(vertices);
		}

		Vertex(const Vertex&) = delete;
		Vertex& operator=(const Vertex&) = delete;

		~Vertex()
		{
			if (m_VBO != 0) glDeleteBuffers(1, &m_VBO);
			if (m_VAO != 0) glDeleteVertexArrays(1, &m_VAO);
		}

		void SetupBuffers(std::span<const float> vertices)
		{
			m_VertexCount = static_cast<uint32_t>(vertices.size() / 3);

			glGenVertexArrays(1, &m_VAO);
			glGenBuffers(1, &m_VBO);

			glBindVertexArray(m_VAO);

			glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
			glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size_bytes()),
				vertices.data(), GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
			glEnableVertexAttribArray(0);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}

		void Draw() const
		{
			if (m_shader) m_shader->Use();

			glBindVertexArray(m_VAO);
			glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_VertexCount));
			glBindVertexArray(0);
		}
	};
}