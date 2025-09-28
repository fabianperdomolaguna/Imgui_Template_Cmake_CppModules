module;

#include <cstdint>
#include <string>

#include "glad/gl.h"

export module Vertex;

import Shaders;

float vertices[] = {
	-0.5f, -0.5f, 0.0f, // left  
	0.5f, -0.5f, 0.0f, // right 
	0.0f,  0.5f, 0.0f  // top   
};

export class GlVertex
{
	uint32_t VAO = 0;
	uint32_t VBO = 0;

public:
	Shader* m_shader = nullptr;

	GlVertex(std::string vertex_shader, std::string fragment_shader)
	{
		m_shader = new Shader();
		m_shader->Create(vertex_shader, fragment_shader);
	}

	~GlVertex()
	{
		delete m_shader;
		glDisableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
	}

	void CreateBuffers()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void Draw()
	{
		glBindVertexArray(VAO);

		glClearColor(0.02f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		m_shader->Use();
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glBindVertexArray(0);
	}
};