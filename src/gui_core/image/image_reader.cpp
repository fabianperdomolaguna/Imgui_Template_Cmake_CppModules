module;

#include <iostream>
#include <cstdint>
#include <string>

#include "glad/gl.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

export module Image;

export class ImageTexture
{
    uint32_t m_render_texture = 0;

public:
    
	uint8_t* m_data = nullptr;
	int m_width = 0;
	int m_height = 0;
	int m_channels = 0;
	uint32_t m_format;
    bool m_texture;

    ImageTexture(std::string file_path, uint32_t image_format, bool texture = false)
    {
		m_data = stbi_load(file_path.c_str(), &m_width, &m_height, &m_channels, 4);
		m_format = image_format;
		m_texture = texture;

        if (!m_data) 
        {
            std::cout << "Failed to load image: " << file_path << std::endl;
            return;
		}

        if (texture)
			create_texture();
    }

    ImageTexture(const uint8_t* image_data, uint32_t image_size, uint32_t image_format, bool texture = false)
    {
        m_data = stbi_load_from_memory(image_data, image_size, &m_width, &m_height, &m_channels, 4);
        m_format = image_format;
		m_texture = texture;

        if (!m_data)
        {
            std::cout << "Failed to load image from memory" << std::endl;
            return;
        }

        if (texture)
            create_texture();
	}

    void create_texture()
    {
        glGenTextures(1, &m_render_texture);
        glBindTexture(GL_TEXTURE_2D, m_render_texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, m_format, m_width, m_height, 0, m_format, GL_UNSIGNED_BYTE, m_data);

        stbi_image_free(m_data);
        m_data = nullptr;
	}

    void reload(const std::string& file_path)
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
            std::cout << "Failed to reload image from file: " << file_path << std::endl;
            return;
        }

        if (m_texture)
            create_texture();
    }

    void reload(const uint8_t* image_data, uint32_t image_size)
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
            std::cout << "Failed to reload image from memory: " << std::endl;
            return;
        }

        if (m_texture)
            create_texture();
    }

    ~ImageTexture()
    {
        if (m_render_texture) 
        {
            glDeleteTextures(1, &m_render_texture);
        }
        else 
        {
            stbi_image_free(m_data);
        }
    }

    bool is_valid() const { return m_render_texture != 0; }

    uint32_t get_texture()
    {
        return m_render_texture;
    }
};
