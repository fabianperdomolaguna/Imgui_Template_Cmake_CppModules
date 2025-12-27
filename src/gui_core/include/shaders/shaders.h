#pragma once

#include <string>

class Shader
{
    uint32_t m_shader = 0;

    uint32_t Compile(unsigned int shader_type, const std::string& shader_source);

public:
    Shader();
	~Shader();

    void Create(const std::string& vertex_src, const std::string& fragment_src);
    void DeleteShader(uint32_t& vs, uint32_t& fs);
    void Use();
};