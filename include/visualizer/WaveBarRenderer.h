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
//
//  GPU layout (per instance, divisor = 1):
//    layout(location = 2)  float  aBarIndex   – [0 .. NUM_BARS-1]
//    layout(location = 3)  float  aAmplitude  – normalised [0 .. 1]
// ─────────────────────────────────────────────────────────────────────────────
struct VisualizerSettings {
    float barAspect = 0.55f;
    float maxBarHeight = 0.1f; 
    float maxCap = 0.8f;        // The height where bars get "cut"
    float bassBoost = 0.15f;
    float pulseSpeed = 3.5f;
    float pulseSpread = 2.0f;   // Higher = more "wave-like" spread
    float yOffset = -0.9f;
    float totalWidth = 3.54f;
};

class WaveBarRenderer : public Renderer
{
public:
    // Number of spectrum bars to display.
    static constexpr int NUM_BARS = 12;

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
    GLuint _quadVBO = 0;   // unit quad geometry
    GLuint _instanceVBO = 0;   // per-bar data  (index, amplitude)

    bool _initialised = false;

    void BuildQuadMesh();
    void UploadInstanceData(const std::vector<float>& spectrum,
        float bass, float mid, float treble);
    VisualizerSettings _settings;
    void RenderGui();
};
