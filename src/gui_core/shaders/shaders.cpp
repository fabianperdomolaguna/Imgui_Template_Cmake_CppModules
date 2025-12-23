module;

#include <cstdint>
#include <string>

#include "glad/gl.h"

#include "logger.h"

export module Shaders;

export class Shader
{
	uint32_t m_shader = 0;

	uint32_t Compile(unsigned int shader_type, const std::string& shader_source)
	{
		uint32_t shader_id = glCreateShader(shader_type);

		const char* c_source = shader_source.c_str();
		glShaderSource(shader_id, 1, &c_source, nullptr);
		glCompileShader(shader_id);

		return shader_id;
	}

public:
	Shader() {}

	~Shader() { glDeleteProgram(m_shader); }

	void Create(const std::string& vertex_src, const std::string& fragment_src)
	{
		m_shader = glCreateProgram();

		uint32_t vs = Compile(GL_VERTEX_SHADER, vertex_src);
		uint32_t fs = Compile(GL_FRAGMENT_SHADER, fragment_src);

		if (vs == 0 || fs == 0) {
			LOG_ERROR("Shader program creation aborted due to compilation errors");
			return;
		}

		glAttachShader(m_shader, vs);
		glAttachShader(m_shader, fs);
		glLinkProgram(m_shader);

		int link_status;
		glGetProgramiv(m_shader, GL_LINK_STATUS, &link_status);
		if (link_status == GL_FALSE) {
			std::string message(256, '\0');
			glGetProgramInfoLog(m_shader, message.size(), nullptr, message.data());
			while (!message.empty() && std::isspace(message.back()) || message.back() == '\0') message.pop_back();
			LOG_ERROR(std::format("Shader Linking failed: {}", message));
		}

		glValidateProgram(m_shader);

		glDeleteShader(vs);
		glDeleteShader(fs);
	}

	void Use()
	{
		glUseProgram(m_shader);
	}
};