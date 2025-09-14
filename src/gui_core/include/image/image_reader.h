#pragma once

#include <string>

class ImageTexture
{
    uint32_t m_render_texture = 0;

public:
    
	uint8_t* m_data = nullptr;
	int m_width = 0;
	int m_height = 0;
	int m_channels = 0;
	uint32_t m_format;
    bool m_texture;

ImageTexture(std::string file_path, uint32_t image_format, bool texture = false);
ImageTexture(const uint8_t* image_data, uint32_t image_size, uint32_t image_format, bool texture = false);

void CreateTexture();
void Reload(const std::string& file_path);
void Reload(const uint8_t* image_data, uint32_t image_size);

~ImageTexture();

bool IsValid() const;

uint32_t get_texture();

};