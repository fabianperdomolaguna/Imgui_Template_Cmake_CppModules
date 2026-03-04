module;

#include <cctype>
#include <cstdint>
#include <format>
#include <string>

#include "glad/gl.h"

export module beryl.renderer:shader;

import beryl.logger;

export namespace beryl::renderer
{
	class Shader
	{
		uint32_t m_shader_id = 0;

		uint32_t Compile(uint32_t type, const std::string& source)
		{
			uint32_t id = glCreateShader(type);
			const char* src = source.c_str();
			glShaderSource(id, 1, &src, nullptr);
			glCompileShader(id);

			int status = 0;
			glGetShaderiv(id, GL_COMPILE_STATUS, &status);
			if (status == GL_FALSE) 
			{
				int length = 0;
				glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
				std::string message(length, '\0');
				glGetShaderInfoLog(id, length, &length, message.data());
							
				while (!message.empty() && (std::isspace(static_cast<unsigned char>(message.back())) != 0 ||
					message.back() == '\0'))
				{
					message.pop_back();
				}

				beryl::logger::Error(std::format("Shader compile error ({}): {}", 
						(type == GL_VERTEX_SHADER ? "Vertex" : "Fragment"), message));
					
				glDeleteShader(id);
				return 0;
			}

			return id;
		}

	public:
		Shader() = default;

		Shader(const Shader&) = delete;
		Shader& operator=(const Shader&) = delete;

		~Shader() 
		{
			if (m_shader_id != 0)
				glDeleteProgram(m_shader_id);
		}

		void Create(const std::string& vertex_src, const std::string& fragment_src)
		{
			if (m_shader_id != 0)
				glDeleteProgram(m_shader_id);

			uint32_t vs = Compile(GL_VERTEX_SHADER, vertex_src);
			uint32_t fs = Compile(GL_FRAGMENT_SHADER, fragment_src);

			if (vs == 0 || fs == 0) 
			{
				if (vs != 0) glDeleteShader(vs);
				if (fs != 0) glDeleteShader(fs);
				return;
			}

			m_shader_id = glCreateProgram();
			glAttachShader(m_shader_id, vs);
			glAttachShader(m_shader_id, fs);
			glLinkProgram(m_shader_id);

			int status = 0;
			glGetProgramiv(m_shader_id, GL_LINK_STATUS, &status);
			if (status == GL_FALSE) 
			{
				int length = 0;
				glGetProgramiv(m_shader_id, GL_INFO_LOG_LENGTH, &length);
				std::string message(length, '\0');
				glGetProgramInfoLog(m_shader_id, length, &length, message.data());
				
				beryl::logger::Error(std::format("Shader Linking failed: {}", message));
				
				glDeleteProgram(m_shader_id);
				m_shader_id = 0;
			}

			glDeleteShader(vs);
			glDeleteShader(fs);
		}

		void Use() const
		{
			if (m_shader_id != 0)
				glUseProgram(m_shader_id);
		}

		[[nodiscard]] uint32_t GetShaderId() const { return m_shader_id; }
	};
}