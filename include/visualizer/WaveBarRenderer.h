#pragma once

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "resources/ResourceManager.h"
#include "utils/Paths.h"
#include "visualizer/Renderer.h"

// ─────────────────────────────────────────────────────────────────────────────
//  WaveBarRenderer
//  Renders a row of instanced vertical bars whose heights are driven by the
//  smoothed FFT spectrum, with a holographic / VLC-style glow aesthetic.
// ─────────────────────────────────────────────────────────────────────────────


class WaveBarRenderer : public Renderer
{
public:
    // Number of spectrum bars to display.
    static constexpr int NUM_BARS = 16;

    explicit WaveBarRenderer(ResourceManager& rm)
        : Renderer(rm) {
    }

    ~WaveBarRenderer() override
    {
        if (_quadVAO) glDeleteVertexArrays(1, &_quadVAO);
        if (_quadVBO) glDeleteBuffers(1, &_quadVBO);
        if (_instanceVBO) glDeleteBuffers(1, &_instanceVBO);
    }

    // ── Renderer interface ────────────────────────────────────────────────────
    void Init() override;
    void Render(RenderContext& context) override;

private:
    // GPU objects
    GLuint _quadVAO = 0;
    GLuint _quadVBO = 0;  
    GLuint _instanceVBO = 0;   

    bool _initialised = false;

    void BuildQuadMesh();
    void UploadInstanceData(const std::vector<float>& spectrum,
        float bass, float mid, float treble);
    void RenderGui();
};
