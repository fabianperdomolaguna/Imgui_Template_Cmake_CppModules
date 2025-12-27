#include <format>
#include <cstdint>
#include <string>

#include "glad/gl.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "image/image_reader.h"
#include "logging/logger.h"

ImageTexture::ImageTexture(std::string file_path, uint32_t image_format, bool texture)
{
	m_data = stbi_load(file_path.c_str(), &m_width, &m_height, &m_channels, 4);
	m_format = image_format;
	m_texture = texture;

    if (!m_data) 
    {
        LOG_ERROR(std::format("Failed to load image: {}", file_path));
        return;
	}

    if (texture)
		CreateTexture();
}

ImageTexture::ImageTexture(const uint8_t* image_data, uint32_t image_size, uint32_t image_format, bool texture)
{
    m_data = stbi_load_from_memory(image_data, image_size, &m_width, &m_height, &m_channels, 4);
    m_format = image_format;
	m_texture = texture;

    if (!m_data)
    {
        LOG_ERROR("Failed to load image from memory");
        return;
    }

    if (texture)
        CreateTexture();
}

void ImageTexture::CreateTexture()
{
    if (!m_data) return;

    DeleteGLResource();

    glGenTextures(1, &m_render_texture);
    glBindTexture(GL_TEXTURE_2D, m_render_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, m_format, m_width, m_height, 0, m_format, GL_UNSIGNED_BYTE, m_data);

    FreeCPUData();
}

void ImageTexture::FreeCPUData()
{
    if (m_data) {
        stbi_image_free(m_data);
        m_data = nullptr;
    }
}

void ImageTexture::DeleteGLResource()
{
    if (m_render_texture != 0) {
        glDeleteTextures(1, &m_render_texture);
        m_render_texture = 0;
    }
}

void ImageTexture::Reload(const std::string& file_path)
{
    if (m_render_texture)
    {
        glDeleteTextures(1, &m_render_texture);
        m_render_texture = 0;
    }

    if (m_data)
    {
        stbi_image_free(m_data);
        m_data = nullptr;
    }

    m_data = stbi_load(file_path.c_str(), &m_width, &m_height, &m_channels, 4);

    if (!m_data)
    {
        LOG_ERROR(std::format("Failed to reload image from file: {}", file_path));
        return;
    }

    if (m_texture)
        CreateTexture();
}

void ImageTexture::Reload(const uint8_t* image_data, uint32_t image_size)
{
    if (m_render_texture)
    {
        glDeleteTextures(1, &m_render_texture);
        m_render_texture = 0;
    }

    if (m_data)
    {
        stbi_image_free(m_data);
        m_data = nullptr;
    }

    m_data = stbi_load_from_memory(image_data, image_size, &m_width, &m_height, &m_channels, 4);

    if (!m_data)
    {
        LOG_ERROR("Failed to reload image from memory");
        return;
    }

    if (m_texture)
        CreateTexture();
}

ImageTexture::~ImageTexture()
{
    FreeCPUData();
    DeleteGLResource();
}

bool ImageTexture::IsValid() const { return m_render_texture != 0; }

uint32_t ImageTexture::get_texture()
{
    return m_render_texture;
}