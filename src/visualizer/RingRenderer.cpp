#include "visualizer/RingRenderer.h"
#include "visualizer/opengl/GLSLProgram.h"

#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ── Vertex layout: vec2 position (unit circle) + float t [0..1] along arc ───
struct RingVertex {
    float x, y;  // unit circle pos (cos,sin)
    float t;     // normalised arc parameter [0..1]
};

void RingRenderer::BuildCircleGeometry()
{
    // Build SEGMENTS+1 vertices around a unit circle.
    // Index 0..SEGMENTS gives us the full circle as a LINE_STRIP.
    // We only *draw* ARC_FRACTION of them (the gap is handled by the draw count).

    std::vector<RingVertex> verts;
    verts.reserve(SEGMENTS + 1);

    for (int i = 0; i <= SEGMENTS; ++i)
    {
        float t     = static_cast<float>(i) / static_cast<float>(SEGMENTS);
        float angle = t * 2.0f * static_cast<float>(M_PI);
        verts.push_back({ std::cos(angle), std::sin(angle), t });
    }

    glGenVertexArrays(1, &_circleVAO);
    glGenBuffers(1, &_circleVBO);

    glBindVertexArray(_circleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, _circleVBO);
    glBufferData(GL_ARRAY_BUFFER,
                 verts.size() * sizeof(RingVertex),
                 verts.data(),
                 GL_STATIC_DRAW);

    // location 0 – vec2 position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                          sizeof(RingVertex), (void*)offsetof(RingVertex, x));

    // location 1 – float t
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE,
                          sizeof(RingVertex), (void*)offsetof(RingVertex, t));

    glBindVertexArray(0);
}

RingRenderer::~RingRenderer()
{
    if (_circleVAO) glDeleteVertexArrays(1, &_circleVAO);
    if (_circleVBO) glDeleteBuffers(1, &_circleVBO);
}

void RingRenderer::Init()
{
    if (_initialised) return;

    if (!resourceManager.GetShader("ring")) {
        auto shader = std::make_unique<Lengine::GLSLProgram>();
        shader->compileShaders(
            rootFolderPath + "/assets/shaders/ring.vert",
            rootFolderPath + "/assets/shaders/ring.frag"
        );
        shader->linkShaders();
        resourceManager.AddShader("ring", std::move(shader));
    }

    BuildCircleGeometry();
    _initialised = true;
}

// ── Map a spectrum bin (log-spaced) to a frequency band amplitude ────────────
static float SampleSpectrum(const std::vector<float>& spec, float normT)
{
    if (spec.empty()) return 0.0f;
    // Log mapping: inner rings get high-freq, outer get low-freq
    // normT: 0 = innermost (high freq), 1 = outermost (low freq)
    float logT = 1.0f - normT;                         // flip: inner=high
    float logBin = std::pow(logT, 1.8f);               // log curve
    int   bin    = static_cast<int>(logBin * (static_cast<int>(spec.size()) - 1));
    bin = std::clamp(bin, 0, static_cast<int>(spec.size()) - 1);
    return spec[bin];
}

void RingRenderer::DrawRing(
    Lengine::GLSLProgram* shader,
    int   ringIndex,
    float amplitude,
    float baseRadius,
    float rotation,
    float time)
{
    // Normalised ring position [0=inner .. 1=outer]
    float normT = static_cast<float>(ringIndex) / static_cast<float>(NUM_RINGS - 1);

    // Radius expands outward with amplitude; inner rings expand more noticeably
    float ampScale  = 1.0f + amplitude * (0.35f - normT * 0.20f);
    float radius    = baseRadius * ampScale;

    // Arc span: louder = slightly longer arc
    float arcFrac   = ARC_FRACTION + amplitude * 0.15f;
    arcFrac         = std::clamp(arcFrac, 0.1f, 0.98f);
    int   drawCount = static_cast<int>(arcFrac * SEGMENTS) + 1;

    // Colour: green (inner/high-freq quiet) → yellow → orange → red (outer/low-freq loud)
    // Mix based on both radial position and amplitude
    float heatT     = normT * 0.65f + amplitude * 0.35f;
    heatT           = std::clamp(heatT, 0.0f, 1.0f);

    // Line width: inner rings thinner, outer rings slightly thicker
    float lineW     = 1.0f + amplitude * 2.5f + normT * 0.8f;

    shader->setFloat("uRadius",   radius);
    shader->setFloat("uRotation", rotation);
    shader->setFloat("uHeatT",    heatT);
    shader->setFloat("uAmplitude",amplitude);
    shader->setFloat("uNormT",    normT);
    shader->setFloat("uArcFrac",  arcFrac);
    shader->setFloat("uTime",     time);

    glLineWidth(std::clamp(lineW, 1.0f, 6.0f));

    glBindVertexArray(_circleVAO);
    glDrawArrays(GL_LINE_STRIP, 0, drawCount);
    glBindVertexArray(0);
}

void RingRenderer::Render(RenderContext& context)
{
    Lengine::GLSLProgram* shader = resourceManager.GetShader("ring");
    if (!shader || !_initialised) return;

    const float aspect = static_cast<float>(context.frameWidth)
                       / static_cast<float>(context.frameHeight);

    // Orthographic keeping circles circular
    glm::mat4 projection = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);

    shader->use();
    shader->setMat4("uProjection", projection);
    shader->setFloat("uTime", context.time);

    // Additive blending for the glow overlap
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDisable(GL_DEPTH_TEST);

    for (int i = 0; i < NUM_RINGS; ++i)
    {
        float normT = static_cast<float>(i) / static_cast<float>(NUM_RINGS - 1);

        // Sample amplitude for this ring's frequency band
        float amplitude = SampleSpectrum(context.smoothedSpectrum, normT);

        // Mix in band energies for inner/outer groups
        if (normT < 0.25f)
            amplitude = amplitude * 0.6f + context.treble * 0.4f;
        else if (normT < 0.55f)
            amplitude = amplitude * 0.7f + context.mid    * 0.3f;
        else
            amplitude = amplitude * 0.65f + context.bass  * 0.35f;

        amplitude = std::clamp(amplitude, 0.0f, 1.0f);

        // Base radius spread linearly from inner to outer
        float baseRadius = BASE_RADIUS_MIN
                         + normT * (BASE_RADIUS_MAX - BASE_RADIUS_MIN);

        // Each ring rotates at a different speed; outer rings rotate faster
        // and in alternating directions for the organic swirl look.
        float direction  = (i % 2 == 0) ? 1.0f : -1.0f;
        float rotSpeed   = 0.18f + normT * 0.35f + amplitude * 0.12f;
        float phaseOffset= static_cast<float>(i) * 0.31f;  // spread start angles
        float rotation   = direction * context.time * rotSpeed + phaseOffset;

        DrawRing(shader, i, amplitude, baseRadius, rotation, context.time);
    }

    // Restore state
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glLineWidth(1.0f);

    shader->unuse();
}
