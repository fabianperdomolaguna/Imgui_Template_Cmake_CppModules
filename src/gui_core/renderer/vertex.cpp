#include <cstdint>
#include <string>

#include "glad/gl.h"

#include "shaders/shaders.h"
#include "renderer/vertex.h"
#include "renderer/gl_error_utils.h"
#include "logging/logger.h"

GlVertex::GlVertex(std::string vertex_shader, std::string fragment_shader)
{
	m_shader = new Shader();
	m_shader->Create(vertex_shader, fragment_shader);
}

GlVertex::~GlVertex()
{
	delete m_shader;
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void GlVertex::CreateBuffers()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
		LOG_ERROR(std::format("Vertex attributes configuring errors: {} - {}", GlErrorToString(error), (unsigned)error));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void GlVertex::Draw()
{
	m_shader->Use();
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);
}
