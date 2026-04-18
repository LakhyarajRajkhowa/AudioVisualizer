#pragma once

#include <vector>

#include "resources/ResourceManager.h"
#include "utils/Paths.h"

// ─────────────────────────────────────────────────────────────────────────────
//  Add HOLOGRAM_WAVES alongside your existing modes
// ─────────────────────────────────────────────────────────────────────────────
enum class RenderMode {
    SPHERICAL_WAVES  = 1,
    HOLOGRAM_WAVES   = 2,   // VLC / WMP-style holographic bar visualizer
    RING_WAVES       = 3,   // ← NEW: concentric rotating arc rings (VLC-style)
};


struct RenderContext {
    float bass;
    float mid;
    float treble;

    std::vector<float> smoothedSpectrum;

    RenderMode mode = RenderMode::SPHERICAL_WAVES;

    float time;

    uint32_t frameWidth;
    uint32_t frameHeight;
};

class Renderer
{
protected:
    ResourceManager& resourceManager;

public:
    explicit Renderer(ResourceManager& rm)
        : resourceManager(rm) {}

    virtual ~Renderer() = default;

    virtual void Init()   = 0;
    virtual void Render(RenderContext& context) = 0;
};



