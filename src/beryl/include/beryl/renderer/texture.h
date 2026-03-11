#pragma once

#include <string_view>

#include "glad/gl.h"

namespace beryl::renderer
{
    class Texture2D
    {
    public:
        uint8_t* m_data = nullptr;
        int m_width = 0;
        int m_height = 0;
        int m_channels = 0;
    
        Texture2D(std::string_view file_path, GLenum image_format, bool use_gl_texture = false);

        Texture2D(const uint8_t* image_data, uint32_t image_size, GLenum image_format,
            bool use_gl_texture = false);
        
        Texture2D(int width, int height, const uint8_t* raw_pixels, GLenum format);

        Texture2D(const Texture2D&) = delete;
        Texture2D& operator=(const Texture2D&) = delete;

        ~Texture2D();

        void Reload(std::string_view file_path);
        void Reload(const uint8_t* image_data, uint32_t image_size);
        void CleanUp();
        [[nodiscard]] bool IsValid() const;
        [[nodiscard]] GLuint GetTextureId() const;

    private:
        GLuint m_render_texture = 0;
        GLenum m_format = GL_RGBA;
        bool m_use_gl_texture = false;

        void CreateOpenGLResource(const uint8_t* pixels);
        void CreateTexture();
        void LoadFromFile(std::string_view path);
        void LoadFromMemory(const uint8_t* data, uint32_t size);
        void FreeCPUData();
    };
}