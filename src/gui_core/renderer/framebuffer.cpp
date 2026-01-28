module;

#include <cstdint>
#include <format>
#include <memory>

#include "glad/gl.h"

#include "logger.h"

export module Framebuffer;

import GlUtils;
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
		glBindRenderbuffer(GL_RENDERBUFFER, RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_texture->m_width, m_texture->m_height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
			LOG_ERROR(std::format("Framebuffer failed: {} - {}", GlFramebufferStatusToString(status), (unsigned)status));

		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
			LOG_ERROR(std::format("GL error during framebuffer creation: {} - {}", GlErrorToString(error), (unsigned)error));

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	~GlFramebuffer() 
	{ 
		DeleteBuffers();
		m_texture.reset();
	}

	void DeleteBuffers()
	{
		if (FBO != 0) {
			glDeleteFramebuffers(1, &FBO);
			FBO = 0;
		}

		if (RBO != 0) {
			glDeleteRenderbuffers(1, &RBO);
			RBO = 0;
		}
		
		if (m_texture)
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