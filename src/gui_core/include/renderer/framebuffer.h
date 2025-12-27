#pragma once

#include <memory>

#include "renderer/texture.h"

class GlFramebuffer
{
    uint32_t FBO = 0;
	uint32_t RBO = 0;

	std::unique_ptr<Texture> m_texture;

public:
    GlFramebuffer(int width, int height);
    ~GlFramebuffer();

    void DeleteBuffers();
    void Bind();
    void Unbind();
    uint32_t get_texture();
};