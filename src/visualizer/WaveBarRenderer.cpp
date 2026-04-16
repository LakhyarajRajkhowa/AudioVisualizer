#include<algorithm>
#include "visualizer/WaveBarRenderer.h"
#include "visualizer/opengl/GLSLProgram.h"
#include <imgui/imgui.h>


static const float QUAD_VERTS[] = {
    // x      y
    -0.5f,  0.0f,
     0.5f,  0.0f,
     0.5f,  1.0f,

    -0.5f,  0.0f,
     0.5f,  1.0f,
    -0.5f,  1.0f,
};



void WaveBarRenderer::Init()
{
    if (_initialised) return;

    if (!resourceManager.GetShader("wavebar")) {
        auto shader = std::make_unique<Lengine::GLSLProgram>();
        shader->compileShaders(
            rootFolderPath + "/assets/shaders/wavebar.vert",
            rootFolderPath + "/assets/shaders/wavebar.frag"
        );
        shader->linkShaders();
        resourceManager.AddShader("wavebar", std::move(shader));
    }

    BuildQuadMesh();

    _initialised = true;
}

void WaveBarRenderer::BuildQuadMesh()
{
    glGenVertexArrays(1, &_quadVAO);
    glGenBuffers(1, &_quadVBO);
    glGenBuffers(1, &_instanceVBO);

    glBindVertexArray(_quadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, _quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(QUAD_VERTS), QUAD_VERTS, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, _instanceVBO);
    glBufferData(GL_ARRAY_BUFFER,
                 NUM_BARS * 2 * sizeof(float),
                 nullptr,
                 GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE,
                          2 * sizeof(float), (void*)0);
    glVertexAttribDivisor(1, 1);

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

    if (spectrum.empty())
        return;

    const float minFreq = 20.0f;
    const float maxFreq = 20000.0f;

    const int sampleRate = 44100;
    const int fftSize = static_cast<int>(spectrum.size()) * 2;

    // ─────────────────────────────────────────────
    // Build logarithmic frequency bands
    // ─────────────────────────────────────────────
    std::vector<float> bands(NUM_BARS, 0.0f);

    for (int i = 0; i < NUM_BARS; ++i)
    {
        float t0 = static_cast<float>(i) / NUM_BARS;
        float t1 = static_cast<float>(i + 1) / NUM_BARS;

        float f0 = minFreq * pow(maxFreq / minFreq, t0);
        float f1 = minFreq * pow(maxFreq / minFreq, t1);

        int bin0 = static_cast<int>((f0 / sampleRate) * fftSize);
        int bin1 = static_cast<int>((f1 / sampleRate) * fftSize);

        bin0 = std::clamp(bin0, 0, static_cast<int>(spectrum.size()) - 1);
        bin1 = std::clamp(bin1, 0, static_cast<int>(spectrum.size()) - 1);

        if (bin1 < bin0) std::swap(bin0, bin1);

        float sum = 0.0f;
        int count = 0;

        for (int b = bin0; b <= bin1; ++b)
        {
            sum += spectrum[b];
            count++;
        }

        float amp = (count > 0) ? (sum / count) : 0.0f;

        amp = pow(amp, 0.5f);   // boost quieter signals
        amp *= 5.0f;            // global gain

        float t = static_cast<float>(i) / (NUM_BARS - 1);
        float trebleTilt = 1.0f + (t * 2.0f);
        amp *= trebleTilt;

        bands[i] = amp;
    }

    static float smoothedMax = 1.0f;

    float currentMax = 0.0001f;
    for (float v : bands)
        currentMax = std::max(currentMax, v);

    // Smooth the max to avoid flickering
    smoothedMax = smoothedMax * 0.9f + currentMax * 0.1f;

    // Normalize amplitude to [0,1]
    for (float& v : bands)
        v /= smoothedMax;

    // ─────────────────────────────────────────────
    // Upload instance data
    // Layout: [barIndex, amplitude]
    // ─────────────────────────────────────────────
    for (int i = 0; i < NUM_BARS; ++i)
    {
        data.push_back(static_cast<float>(i));
        data.push_back(bands[i]);
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


    // Upload dynamic instance data
    UploadInstanceData(context.smoothedSpectrum,
                       context.bass, context.mid, context.treble);

    shader->use();

    shader->setFloat("uTime",       context.time);
    shader->setFloat("uBass",       context.bass);
    shader->setFloat("uMid",        context.mid);
    shader->setFloat("uTreble",     context.treble);
    shader->setInt  ("uNumBars",    NUM_BARS);

    float aspect = static_cast<float>(context.frameWidth) /
        static_cast<float>(context.frameHeight);
    float totalWidth = 2.0f * aspect;

    shader->setFloat("uTotalWidth", totalWidth);

    glm::mat4 projection = glm::ortho(
        -1.0f * aspect, 1.0f * aspect,   // left / right
        -1.0f,          1.0f,            // bottom / top
        -1.0f,          1.0f             // near / far
    );
    shader->setMat4("uProjection", projection);

    // Additive blending for the glow look
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);  

    glDisable(GL_DEPTH_TEST);

    glBindVertexArray(_quadVAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, NUM_BARS);
    glBindVertexArray(0);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    shader->unuse();
}
