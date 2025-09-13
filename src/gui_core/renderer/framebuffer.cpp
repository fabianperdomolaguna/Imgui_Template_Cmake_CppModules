module;

#include <cstdint>
#include <memory>

#include "glad/gl.h"

export module Framebuffer;

import Texture;

export class GlFramebuffer
{
	uint32_t FBO = 0;
	uint32_t RBO = 0;

	std::unique_ptr<Texture> m_texture;

public:
	GlFramebuffer(int width, int height)
	{
		if (FBO)
			DeleteBuffers();

		glGenFramebuffers(1, &FBO);
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);

		m_texture = std::make_unique<Texture>(nullptr, width, height, GL_RGB);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture->get_texture(), 0);
		glGenRenderbuffers(1, &RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_texture->m_width, m_texture->m_height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	~GlFramebuffer() { DeleteBuffers(); }

	void DeleteBuffers()
	{
		glDeleteFramebuffers(GL_FRAMEBUFFER, &FBO);
		glDeleteRenderbuffers(1, &RBO);
		m_texture->DeleteTexture();
	}

	void Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glViewport(0, 0, m_texture->m_width, m_texture->m_height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	uint32_t get_texture()
	{
		return m_texture->get_texture();
	}
};