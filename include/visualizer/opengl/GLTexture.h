#pragma once
#include <GL/glew.h>
#include <string>
#include <vector>
#include <memory>
namespace Lengine {
    struct ImageData {
        int width;
        int height;
        int channels;
        std::vector<unsigned char> pixels;
    };


    struct  GLTexture
    {
        GLuint id;
        int width;
        int height;
        int channels = 0;

        std::shared_ptr<ImageData> imageCPU;

        bool srgb = false;
 
        bool pendingGPUUpload = false;

    };
}
