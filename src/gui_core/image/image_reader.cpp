module;

#include <iostream>
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
    bool success;
};

export ImageData ReadImage(std::string file_path)
{
    ImageData image_data;

    image_data.data = stbi_load(file_path.c_str(), 
        &image_data.width, &image_data.height, &image_data.channels, 4);
    image_data.format = GL_RGBA;

    if (!image_data.data) {
        std::cout << "Failed to load image: " << file_path << std::endl;
        image_data.success = false;
        return ImageData{};
    }

    image_data.success = true;

    return image_data;
}