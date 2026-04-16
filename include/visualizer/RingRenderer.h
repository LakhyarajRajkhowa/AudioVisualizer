#pragma once

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "resources/ResourceManager.h"
#include "utils/Paths.h"
#include "Renderer.h"

// ─────────────────────────────────────────────────────────────────────────────
//  RingRenderer
//
//  Renders NUM_RINGS concentric arcs, each mapped to a frequency band.
//  Inner rings = high frequency (green), outer rings = low frequency (red).
//  Each ring's radius expands with amplitude; arcs rotate slowly over time.
//
//  Geometry strategy:
//    Each ring is a circle tessellated into SEGMENTS vertices.
//    A per-ring UBO / instanced draw uploads:
//      - ring index  (determines base radius + colour)
//      - amplitude   (scales radius outward)
//      - rotation    (per-ring angular offset, driven by time + ring index)
//    We draw each ring as GL_LINE_STRIP with a gap (arc, not full circle).
// ─────────────────────────────────────────────────────────────────────────────

class RingRenderer : public Renderer
{
public:
    // Tweak these to match the look
    static constexpr int   NUM_RINGS       = 32;    // number of concentric rings
    static constexpr int   SEGMENTS        = 256;   // vertices per ring circle
    static constexpr float BASE_RADIUS_MIN = 0.08f; // innermost ring base radius
    static constexpr float BASE_RADIUS_MAX = 0.72f; // outermost ring base radius
    static constexpr float ARC_FRACTION    = 0.72f; // fraction of circle that is solid arc [0..1]

    explicit RingRenderer(ResourceManager& rm)
        : Renderer(rm) {}

    ~RingRenderer() override;

    void Init()   override;
    void Render(RenderContext& context) override;

private:
    // One VAO/VBO pair shared for the unit-circle geometry;
    // per-ring data (amplitude, rotation, colour) is uploaded as a UBO each draw.
    GLuint _circleVAO = 0;
    GLuint _circleVBO = 0;   // SEGMENTS+1 unit-circle points (x,y) + texcoord t

    bool _initialised = false;

    void BuildCircleGeometry();

    // Draw a single ring
    void DrawRing(
        Lengine::GLSLProgram* shader,
        int   ringIndex,
        float amplitude,
        float baseRadius,
        float rotation,
        float time
    );
};
