#include <cstdint>
#include <format>

#include "glad/gl.h"

#include "renderer/texture.h"
#include "renderer/gl_error_utils.h"
#include "logging/logger.h"

Texture::Texture(uint8_t* data, int width, int height, uint32_t format)
{
    m_width = width;
    m_height = height;
    m_format = format;

    glGenTextures(1, &m_render_texture);
	glBindTexture(GL_TEXTURE_2D, m_render_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, m_format, m_width, m_height, 0, m_format, GL_UNSIGNED_BYTE, data);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
        LOG_ERROR(std::format("glTexImage2D failed: {} - {}", GlErrorToString(error), (unsigned)error));
}

 Texture::~Texture()
{
    glDeleteTextures(1, &m_render_texture);
}

void Texture::DeleteTexture()
{
    glDeleteTextures(1, &m_render_texture);
}

uint32_t Texture::get_texture()
{
    return m_render_texture;
}