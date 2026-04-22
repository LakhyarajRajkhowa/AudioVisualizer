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
    if (spectrum.empty()) return;

    std::vector<float> data;
    data.reserve(NUM_BARS * 2);

    // Copy log spectrum directly
    std::vector<float> bands = spectrum;

    // Optional: normalize (keep this part)
    static float smoothedMax = 1.0f;

    float currentMax = 0.0001f;
    for (float v : bands)
        currentMax = std::max(currentMax, v);

    smoothedMax = smoothedMax * 0.9f + currentMax * 0.1f;

    for (float& v : bands)
        v /= smoothedMax;

    // Upload
    for (int i = 0; i < NUM_BARS; ++i)
    {
        data.push_back((float)i);
        data.push_back(i < bands.size() ? bands[i] : 0.0f);
    }

    glBindBuffer(GL_ARRAY_BUFFER, _instanceVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0,
        data.size() * sizeof(float),
        data.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void WaveBarRenderer::Render(RenderContext& context)
{
    Lengine::GLSLProgram* shader = resourceManager.GetShader("wavebar");
    if (!shader || !_initialised) return;


    UploadInstanceData(context.logSpectrum,
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
