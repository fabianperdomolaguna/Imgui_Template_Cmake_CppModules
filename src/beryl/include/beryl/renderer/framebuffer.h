#pragma once

#include <cstdint>
#include <memory>

#include "beryl/renderer/texture.h"

namespace beryl::renderer
{
	class Framebuffer
	{
		uint32_t m_FBO = 0;
		uint32_t m_RBO = 0;
		int m_width = 0;
		int m_height = 0;

		std::unique_ptr<beryl::renderer::Texture2D> m_texture;
    
    public:
		Framebuffer(int width, int height);

        Framebuffer(const Framebuffer&) = delete;
		Framebuffer& operator=(const Framebuffer&) = delete;

		~Framebuffer();

        void Recreate();
        void DeleteBuffers();
        void Bind();
        void Unbind();
        void Clear();
        [[nodiscard]] uint32_t GetTextureId() const;
    };
} 