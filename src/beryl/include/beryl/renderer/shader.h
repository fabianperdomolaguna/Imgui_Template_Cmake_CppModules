#pragma once

#include <cstdint>
#include <string>

namespace beryl::renderer
{
    class Shader
	{
		uint32_t m_shader_id = 0;

        uint32_t Compile(uint32_t type, const std::string& source);
    
    public:
		Shader() = default;

		Shader(const Shader&) = delete;
		Shader& operator=(const Shader&) = delete;

        ~Shader();

        void Create(const std::string& vertex_src, const std::string& fragment_src);
        void Use() const;
        [[nodiscard]] uint32_t GetShaderId() const;
    };
}