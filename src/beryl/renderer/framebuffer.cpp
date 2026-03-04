module;

#include <cstdint>
#include <memory>

#include "glad/gl.h"

export module beryl.renderer:framebuffer;

import :texture2d;
import beryl.utils.gl;
import beryl.logger;

export namespace beryl::renderer
{
	class Framebuffer
	{
		uint32_t m_FBO = 0;
		uint32_t m_RBO = 0;
		int m_width = 0;
		int m_height = 0;

		std::unique_ptr<beryl::renderer::Texture2D> m_texture;

	public:
		Framebuffer(int width, int height) : m_width(width), m_height(height)
		{
			Recreate();
		}

		Framebuffer(const Framebuffer&) = delete;
		Framebuffer& operator=(const Framebuffer&) = delete;

		~Framebuffer() 
		{ 
			DeleteBuffers();
		}

		void Recreate()
		{
			if (m_FBO != 0)
				DeleteBuffers();

			glGenFramebuffers(1, &m_FBO);
			glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

			m_texture = std::make_unique<beryl::renderer::Texture2D>(m_width, m_height, nullptr, GL_RGB);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
				m_texture->GetTextureId(), 0);
			glGenRenderbuffers(1, &m_RBO);
			glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
			glRenderbufferStorage(
				GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) 
				beryl::logger::Error("Framebuffer configuration failed!");

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		void DeleteBuffers()
		{
			if (m_FBO != 0) glDeleteFramebuffers(1, &m_FBO);
			if (m_RBO != 0) glDeleteRenderbuffers(1, &m_RBO);

			m_FBO = 0;
			m_RBO = 0;
		}

		void Bind()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
			glViewport(0, 0, m_texture->m_width, m_texture->m_height);
		}

		void Unbind()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		void Clear()
		{
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		[[nodiscard]] uint32_t GetTextureId() const { return m_texture ? m_texture->GetTextureId() : 0; }
	};
}