#include<algorithm>
#include "visualizer/WaveBarRenderer.h"
#include "visualizer/opengl/GLSLProgram.h"
#include <imgui/imgui.h>

// ─────────────────────────────────────────────────────────────────────────────
//  Unit quad: two triangles filling [-0.5, 0.5] in X, [0, 1] in Y.
//  The vertex shader scales Y by amplitude and translates X by bar index.
// ─────────────────────────────────────────────────────────────────────────────
static const float QUAD_VERTS[] = {
    // x      y
    -0.5f,  0.0f,
     0.5f,  0.0f,
     0.5f,  1.0f,

    -0.5f,  0.0f,
     0.5f,  1.0f,
    -0.5f,  1.0f,
};

void WaveBarRenderer::RenderGui()
{
    ImGui::Begin("Visualizer Style Editor");

    if (ImGui::CollapsingHeader("Geometry & Limits", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::SliderFloat("Total Width", &_settings.totalWidth, 0.5f, 4.0f);
        ImGui::SliderFloat("Bar Gap", &_settings.barAspect, 0.0f, 1.0f);
        ImGui::SliderFloat("Y Offset", &_settings.yOffset, -1.0f, 1.0f);
        ImGui::Separator();
        ImGui::SliderFloat("Height Scale", &_settings.maxBarHeight, 0.0f, 2.0f);
        ImGui::SliderFloat("Height CAP (Cut)", &_settings.maxCap, 0.01f, 2.0f);
    }

    if (ImGui::CollapsingHeader("Animation", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::SliderFloat("Bass Sensitivity", &_settings.bassBoost, 0.0f, 1.0f);
        ImGui::SliderFloat("Pulse Speed", &_settings.pulseSpeed, 0.0f, 10.0f);
        ImGui::SliderFloat("Pulse Spread", &_settings.pulseSpread, 0.0f, 10.0f);
    }

    if (ImGui::Button("Reset Defaults")) {
        _settings = VisualizerSettings();
    }

    ImGui::End();
}

void WaveBarRenderer::Init()
{
    if (_initialised) return;

    // ── Shader ────────────────────────────────────────────────────────────────
    if (!resourceManager.GetShader("wavebar")) {
        auto shader = std::make_unique<Lengine::GLSLProgram>();
        shader->compileShaders(
            rootFolderPath + "/assets/shaders/wavebar.vert",
            rootFolderPath + "/assets/shaders/wavebar.frag"
        );
        shader->linkShaders();
        resourceManager.AddShader("wavebar", std::move(shader));
    }

    // ── Geometry ─────────────────────────────────────────────────────────────
    BuildQuadMesh();

    _initialised = true;
}

void WaveBarRenderer::BuildQuadMesh()
{
    glGenVertexArrays(1, &_quadVAO);
    glGenBuffers(1, &_quadVBO);
    glGenBuffers(1, &_instanceVBO);

    glBindVertexArray(_quadVAO);

    // ── Static quad vertices (location 0 = position) ─────────────────────────
    glBindBuffer(GL_ARRAY_BUFFER, _quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(QUAD_VERTS), QUAD_VERTS, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    // ── Per-instance data (location 1 = barIndex, location 2 = amplitude) ────
    // Layout: [barIndex, amplitude] per instance → stride = 2 floats
    glBindBuffer(GL_ARRAY_BUFFER, _instanceVBO);
    glBufferData(GL_ARRAY_BUFFER,
                 NUM_BARS * 2 * sizeof(float),
                 nullptr,
                 GL_DYNAMIC_DRAW);

    // barIndex  – attrib 1
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE,
                          2 * sizeof(float), (void*)0);
    glVertexAttribDivisor(1, 1);

    // amplitude – attrib 2
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE,
                          2 * sizeof(float), (void*)(sizeof(float)));
    glVertexAttribDivisor(2, 1);

    glBindVertexArray(0);
}

void WaveBarRenderer::UploadInstanceData(
    const std::vector<float>& spectrum,
    float bass, float mid, float treble)
{
    std::vector<float> data;
    data.reserve(NUM_BARS * 2);

    const int specSize = static_cast<int>(spectrum.size());
    // Only sample the first 70% of the spectrum where music actually lives
    const int effectiveSpecSize = static_cast<int>(specSize * 0.70f);

    for (int i = 0; i < NUM_BARS; ++i)
    {
        float t = static_cast<float>(i) / static_cast<float>(NUM_BARS - 1);

        // Map bars to the effective spectrum range
        float mapping = (t * 0.8f) + (std::sqrt(t) * 0.2f);
        int bin = static_cast<int>(mapping * (effectiveSpecSize - 1));
        bin = std::clamp(bin, 0, effectiveSpecSize - 1);

        float amp = spectrum[bin];

        // TREBLE TILT: Gradually increase gain for bars further to the right
        // to compensate for naturally lower high-frequency energy.
        float trebleTilt = 1.0f + (t * 4.0f);
        amp *= trebleTilt;

        data.push_back(static_cast<float>(i));
        data.push_back(amp);
    }

    glBindBuffer(GL_ARRAY_BUFFER, _instanceVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0,
        static_cast<GLsizeiptr>(data.size() * sizeof(float)),
        data.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void WaveBarRenderer::Render(RenderContext& context)
{
    Lengine::GLSLProgram* shader = resourceManager.GetShader("wavebar");
    if (!shader || !_initialised) return;

    RenderGui();


    // Upload dynamic instance data
    UploadInstanceData(context.smoothedSpectrum,
                       context.bass, context.mid, context.treble);

    shader->use();

    // ── Uniforms ──────────────────────────────────────────────────────────────
    shader->setFloat("uTime",       context.time);
    shader->setFloat("uBass",       context.bass);
    shader->setFloat("uMid",        context.mid);
    shader->setFloat("uTreble",     context.treble);
    shader->setInt  ("uNumBars",    NUM_BARS);

    shader->setFloat("uBarAspect", _settings.barAspect);
    shader->setFloat("uMaxBarHeight", _settings.maxBarHeight);
    shader->setFloat("uBassBoost", _settings.bassBoost);
    shader->setFloat("uPulseSpeed", _settings.pulseSpeed);
    shader->setFloat("uYOffset", _settings.yOffset);
    shader->setFloat("uTotalWidth", _settings.totalWidth);
    shader->setFloat("uMaxCap", _settings.maxCap);
    shader->setFloat("uPulseSpread", _settings.pulseSpread);
    // Orthographic projection: NDC [-1,1] in both axes
    // We lay the bars across X=[-1, 1], Y=[0..maxHeight]
    float aspect = static_cast<float>(context.frameWidth) /
                   static_cast<float>(context.frameHeight);

    glm::mat4 projection = glm::ortho(
        -1.0f * aspect, 1.0f * aspect,   // left / right
        -1.0f,          1.0f,            // bottom / top
        -1.0f,          1.0f             // near / far
    );
    shader->setMat4("uProjection", projection);

    // ── Draw ─────────────────────────────────────────────────────────────────
    // Additive blending for the glow look
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);   // additive

    glDisable(GL_DEPTH_TEST);

    glBindVertexArray(_quadVAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, NUM_BARS);
    glBindVertexArray(0);

    // Restore defaults
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    shader->unuse();
}
