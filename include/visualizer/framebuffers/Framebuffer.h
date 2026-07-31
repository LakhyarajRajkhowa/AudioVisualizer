#pragma once
#include <cstdint>
#include <GL/glew.h>
#include "visualizer/opengl/GLTexture.h"

namespace Lengine {

    static GLenum GetFormatFromInternal(GLenum internalFormat)
    {
        switch (internalFormat)
        {
        case GL_RGBA8:
        case GL_RGBA16F:
            return GL_RGBA;

        case GL_RGB8:
        case GL_RGB16F:
            return GL_RGB;

        case GL_R32F:
            return GL_RED;

        default:
            return GL_RGBA;
        }
    }

    static GLenum GetTypeFromInternal(GLenum internalFormat)
    {
        switch (internalFormat)
        {
        case GL_RGBA8:
        case GL_RGB8:
            return GL_UNSIGNED_BYTE;

        case GL_RGBA16F:
        case GL_RGB16F:
        case GL_R32F:
            return GL_FLOAT;

        default:
            return GL_UNSIGNED_BYTE;
        }
    }

    struct FramebufferSpecification
    {
        uint32_t width = 1280;
        uint32_t height = 720;

        uint32_t samples = 1;            // 1 = no MSAA
        std::vector<GLenum> colorFormats = { GL_RGBA8 }; // GL_RGBA8 or GL_RGBA16F

        uint32_t colorAttachmentCount = 1;
        bool     useDepth = true;
    };

    class Framebuffer
    {
    public:
        Framebuffer(const FramebufferSpecification& spec);
        ~Framebuffer();

        void Bind() const;
        void Unbind() const;

        void Resize(uint32_t width, uint32_t height);
        void Destroy();

        uint32_t GetID() const { return m_FBO; }
        uint32_t GetColorAttachment(uint32_t index = 0) const;
        uint32_t GetDepthAttachment() const;
        uint32_t GetWidth() const { return m_Spec.width; }
        uint32_t GetHeight() const { return m_Spec.height; }
        uint32_t GetSamples() const { return m_Spec.samples; }

        const FramebufferSpecification& GetSpecification() const { return m_Spec; }

        void Invalidate(); // Create internally

    private:
        uint32_t m_FBO = 0;
        uint32_t m_DepthAttachment = 0;
        std::vector<uint32_t> m_ColorAttachments;

        FramebufferSpecification m_Spec;
    };


}
