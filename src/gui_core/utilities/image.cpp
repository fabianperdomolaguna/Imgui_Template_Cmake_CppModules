module;

#include <cstdint>
#include <string>

#include "glad/glad.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

export module Image;

struct ImageData
{
    uint8_t* data = nullptr;
    int width;
    int height;
    int channels;
    uint32_t format;
};

export ImageData ReadImage(std::string file_path)
{
    ImageData image_data;

    if (stbi_is_hdr(file_path.c_str()))
    {
        image_data.data = (uint8_t*)stbi_loadf(file_path.c_str(), 
            &image_data.width, &image_data.height, &image_data.channels, 4);
        image_data.format = GL_RGBA32F;
    }
    else
    {
        image_data.data = stbi_load(file_path.c_str(), 
            &image_data.width, &image_data.height, &image_data.channels, 4);
        image_data.format = GL_RGBA;
    }

    return image_data;
}