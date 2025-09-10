module;

#include <cstdint>

#include "glad/gl.h"

export module Texture;

export class Texture 
{
    uint32_t m_render_texture;

public:
    uint32_t m_width = 0, m_height = 0;
    uint32_t m_format = 0;

    Texture(uint8_t* data, int width, int height, uint32_t format)
    {
        m_width = width;
        m_height = height;
        m_format = format;

        glGenTextures(1, &m_render_texture);
	    glBindTexture(GL_TEXTURE_2D, m_render_texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, m_format, m_width, m_height, 0, m_format, GL_UNSIGNED_BYTE, data);
    }

    ~Texture()
    {
        glDeleteTextures(1, &m_render_texture);
    }

    uint32_t get_texture()
    {
        return m_render_texture;
    }
};