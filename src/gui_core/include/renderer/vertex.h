#pragma once

#include <string>

#include "shaders/shaders.h"

static float vertices[] = {
			-0.5f, -0.5f, 0.0f, // left  
			 0.5f, -0.5f, 0.0f, // right 
			 0.0f,  0.5f, 0.0f  // top   
};

class GlVertex
{
    uint32_t VAO = 0;
	uint32_t VBO = 0;

public:
	Shader* m_shader = nullptr;

    GlVertex(std::string vertex_shader, std::string fragment_shader);
    ~GlVertex();

    void CreateBuffers();
    void Draw();
};