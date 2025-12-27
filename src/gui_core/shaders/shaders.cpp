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
		if (shader_id == 0) {
			LOG_ERROR("glCreateShader failed");
			return 0;
		}

		const char* c_source = shader_source.c_str();
		glShaderSource(shader_id, 1, &c_source, nullptr);
		glCompileShader(shader_id);

		int compile_status;
		glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compile_status);
		if (compile_status != GL_TRUE) {
			int info_length = 0;
			glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_length);
			std::string message;
			if (info_length > 0) {
				message.resize(info_length);
				glGetShaderInfoLog(shader_id, info_length, nullptr, message.data());
				while (!message.empty() && (std::isspace(static_cast<unsigned char>(message.back())) || message.back() == '\0')) {
					message.pop_back();
				}
			}
			LOG_ERROR(std::format("Shader compilation failed (type {}): {}", shader_type, message));
			glDeleteShader(shader_id);
			return 0;
		}

		return shader_id;
	}

public:
	Shader() {}

	~Shader() { glDeleteProgram(m_shader); }

	void Create(const std::string& vertex_src, const std::string& fragment_src)
	{
		m_shader = glCreateProgram();
		if (m_shader == 0) {
			LOG_ERROR("glCreateProgram failed");
			return;
		}

		uint32_t vs = Compile(GL_VERTEX_SHADER, vertex_src);
		uint32_t fs = Compile(GL_FRAGMENT_SHADER, fragment_src);

		if (vs == 0 || fs == 0) {
			LOG_ERROR("Shader program creation aborted due to compilation errors");
			DeleteShader(vs, fs);
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
			DeleteShader(vs, fs);
			return;
		}

		glValidateProgram(m_shader);

		glDeleteShader(vs);
		glDeleteShader(fs);
	}

	void DeleteShader(uint32_t& vs, uint32_t& fs)
	{
		if (vs != 0) glDeleteShader(vs);
		if (fs != 0) glDeleteShader(fs);
		glDeleteProgram(m_shader);
		m_shader = 0;
	}

	void Use()
	{
		if (m_shader != 0) glUseProgram(m_shader);
	}
};