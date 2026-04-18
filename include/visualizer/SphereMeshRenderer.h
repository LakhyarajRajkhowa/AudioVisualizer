#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "resources/ResourceManager.h"
#include "visualizer/Renderer.h"

class SpheremeshRenderer : public Renderer
{
public:
    explicit SpheremeshRenderer(ResourceManager& rm);

    void Init() override;
    void Render(RenderContext& context) override;
};