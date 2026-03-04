module;

#include <cstdint>
#include <format>
#include <string>
#include <string_view>

#include "glad/gl.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

export module beryl.renderer:texture2d;

import beryl.logger;

export namespace beryl::renderer
{
    class Texture2D
    {
    public:
        uint8_t* m_data = nullptr;
        int m_width = 0;
        int m_height = 0;
        int m_channels = 0;

        Texture2D(std::string_view file_path, GLenum image_format, bool use_gl_texture = false)
            : m_format(image_format), m_use_gl_texture(use_gl_texture)
        {
            LoadFromFile(file_path);
        }

        Texture2D(const uint8_t* image_data, uint32_t image_size, GLenum image_format,
            bool use_gl_texture = false)
            : m_format(image_format), m_use_gl_texture(use_gl_texture)
        {
            LoadFromMemory(image_data, image_size);
        }

        Texture2D(int width, int height, const uint8_t* raw_pixels, GLenum format)
            : m_width(width), m_height(height), m_format(format), m_use_gl_texture(true)
        {
            CreateOpenGLResource(raw_pixels);
        }

        Texture2D(const Texture2D&) = delete;
        Texture2D& operator=(const Texture2D&) = delete;

        ~Texture2D()
        {
            CleanUp();
        }

        void Reload(std::string_view file_path)
        {
            CleanUp();
            LoadFromFile(file_path);
        }

        void Reload(const uint8_t* image_data, uint32_t image_size)
        {
            CleanUp();
            LoadFromMemory(image_data, image_size);
        }

        void CleanUp()
        {
            if (m_render_texture != 0)
            {
                glDeleteTextures(1, &m_render_texture);
                m_render_texture = 0;
            }
        }

        [[nodiscard]] bool IsValid() const { return m_render_texture != 0; }

        [[nodiscard]] GLuint GetTextureId() const { return m_render_texture; }

    private:
        GLuint m_render_texture = 0;
        GLenum m_format = GL_RGBA;
        bool m_use_gl_texture = false;

        void CreateOpenGLResource(const uint8_t* pixels)
        {
            if (m_render_texture != 0) 
            {
                glDeleteTextures(1, &m_render_texture);
                m_render_texture = 0;
            }
            
            glGenTextures(1, &m_render_texture);
            glBindTexture(GL_TEXTURE_2D, m_render_texture);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(m_format), 
                        m_width, m_height, 0, m_format, 
                        GL_UNSIGNED_BYTE, pixels);
        }

        void CreateTexture()
        {
            if (m_data != nullptr) 
            {
                CreateOpenGLResource(m_data);
                FreeCPUData();
            }
        }

        void LoadFromFile(std::string_view path)
        {
            int required_channels = (m_format == GL_RGB) ? 3 : 4;
            m_data = stbi_load(std::string(path).c_str(), &m_width, &m_height, &m_channels, required_channels);

            if (m_data == nullptr)
            {
                beryl::logger::Error(std::format("Failed to load image: {}", path));
                return;
            }

            if (m_use_gl_texture)
                CreateTexture();
        }

        void LoadFromMemory(const uint8_t* data, uint32_t size) 
        {
            if (data == nullptr || size == 0) 
            {
                beryl::logger::Error("Invalid memory data provided to ImageTexture");
                return;
            }

            int required_channels = (m_format == GL_RGB) ? 3 : 4;
            m_data = stbi_load_from_memory(
                data, static_cast<int>(size), &m_width, &m_height, &m_channels, required_channels);

            if (m_data == nullptr) 
            {
                beryl::logger::Error("Failed to load image from memory");
                return;
            }

            if (m_use_gl_texture) 
                CreateTexture();
        }

        void FreeCPUData()
        {
            if (m_data != nullptr)
            {
                stbi_image_free(m_data);
                m_data = nullptr;
            }
        }
    };
}