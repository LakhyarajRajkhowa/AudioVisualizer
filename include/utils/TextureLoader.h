#pragma once

#include <stb_image.h>

#include <string>
#include <iostream>
#include <GL/glew.h>

inline unsigned int LoadTexture(const std::string& path)
{
    int width = 0;
    int height = 0;
    int channels = 0;

    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (!data)
    {
        const char* reason = stbi_failure_reason();
        std::cerr << "[Texture Error] Failed to load '" << path << "': "
                  << (reason ? reason : "unknown image decoding error") << '\n';
        return 0;
    }

    GLenum format = GL_RGB;
    switch (channels)
    {
        case 1: format = GL_RED;  break;
        case 2: format = GL_RG;   break;
        case 3: format = GL_RGB;  break;
        case 4: format = GL_RGBA; break;
        default:
            std::cerr << "[Texture Error] Unsupported channel count ("
                      << channels << ") in: " << path << '\n';
            stbi_image_free(data);
            return 0;
    }

    GLuint texture = 0;
    glGenTextures(1, &texture);
    if (texture == 0)
    {
        std::cerr << "[Texture Error] OpenGL could not allocate a texture for: "
                  << path << '\n';
        stbi_image_free(data);
        return 0;
    }

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(format),
                 width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    return texture;
}
