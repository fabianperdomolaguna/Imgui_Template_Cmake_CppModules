#pragma once

#include "renderer/texture.h"

class GlFramebuffer
{
    uint32_t FBO = 0;
	uint32_t RBO = 0;

	Texture* m_texture = nullptr;

public:
    GlFramebuffer(int width, int height);
    ~GlFramebuffer();

    void DeleteBuffers();
    void Bind();
    void Unbind();
    uint32_t get_texture();
};