#pragma once

#include <vector>

#include "resources/ResourceManager.h"
#include "utils/Paths.h"


enum class RenderMode {
    SPHERICAL_WAVES  = 0,
    HOLOGRAM_WAVES   = 1,   
    RING_WAVES       = 2,  
};


struct RenderContext {
    float bass;
    float mid;
    float treble;

    std::vector<float> rawSamples;
    std::vector<float> rawSpectrum;
    std::vector<float> smoothedSpectrum;
    std::vector<float> logSpectrum;

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



