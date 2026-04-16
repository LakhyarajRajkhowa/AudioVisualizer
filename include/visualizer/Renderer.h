#pragma once

#include <vector>

#include "resources/ResourceManager.h"
#include "utils/Paths.h"

#include <imgui/imgui.h>

enum class RenderMode {
    SPHERICAL_WAVES = 1
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

struct WaveParams
{
    float bassWeight = 1.5f;
    float midWeight = 1.0f;
    float trebleWeight = 0.5f;

    float waveFrequency = 4.0f;
    float displacementScale = 0.01f;

    float rotationSpeed = 0.3f;
    float scale = 1.0f;
};

class Renderer
{
protected:
    ResourceManager& resourceManager;

public:
    Renderer(ResourceManager& rm)
        : resourceManager(rm) {}

    virtual ~Renderer() = default;

    virtual void Init() = 0;
    virtual void Render(RenderContext& context) = 0;
};



class SpheremeshRenderer : public Renderer
{
public:
    SpheremeshRenderer(ResourceManager& rm)
        : Renderer(rm) {}

    WaveParams params;

    void TuneParams(WaveParams& params) {
        ImGui::Begin("Tune");

        ImGui::SliderFloat("Bass Weight", &params.bassWeight, 0.0f, 5.0f);
        ImGui::SliderFloat("Mid Weight", &params.midWeight, 0.0f, 20.0f);
        ImGui::SliderFloat("Treble Weight", &params.trebleWeight, 0.0f, 5.0f);

        ImGui::SliderFloat("Wave Frequency", &params.waveFrequency, 0.1f, 20.0f);
        ImGui::SliderFloat("Displacement", &params.displacementScale, 0.0f, 0.1f);

        ImGui::SliderFloat("Rotation Speed", &params.rotationSpeed, 0.0f, 5.0f);
        ImGui::SliderFloat("Scale", &params.scale, 0.0f, 10.0f);



        ImGui::End();
    }

    void Init() override
    {
        if (!resourceManager.GetShader("sphere")) {
            auto shader = std::make_unique<Lengine::GLSLProgram>();
            shader->compileShaders(rootFolderPath + "/assets/shaders/sphere.vert", rootFolderPath + "assets/shaders/sphere.frag");
            shader->linkShaders();

            resourceManager.AddShader("sphere", std::move(shader));
        }

        if (!resourceManager.GetMesh("sphere")) {
            // intialize mesh
            auto mesh = std::make_unique<Mesh>("sphere", rootFolderPath + "assets/meshes/sphere.obj");

            // add mesh to resource manager
            resourceManager.AddMesh("sphere", std::move(mesh));
        }

    }
    void Render(RenderContext& context) override
    {
        Lengine::GLSLProgram* shader = resourceManager.GetShader("sphere");
        Mesh* mesh = resourceManager.GetMesh("sphere");

        if (!shader || !mesh)
            return;

        TuneParams(params);

        shader->use();

        // Audio
        shader->setFloat("uBass", context.bass);
        shader->setFloat("uMid", context.mid);
        shader->setFloat("uTreble", context.treble);

        // Time
        shader->setFloat("uTime", context.time);

        // Tunable parameters
        shader->setFloat("uBassWeight", params.bassWeight);
        shader->setFloat("uMidWeight", params.midWeight);
        shader->setFloat("uTrebleWeight", params.trebleWeight);
        shader->setFloat("uWaveFrequency", params.waveFrequency);
        shader->setFloat("uDisplacementScale", params.displacementScale);
        glm::mat4 model = glm::mat4(1.0f);

        float rotationSpeed = 0.3f;
        float angle = context.time * rotationSpeed;

        model = glm::rotate(model, angle, glm::vec3(0, 1, 0));
        model = glm::rotate(model, angle * 0.5f, glm::vec3(1, 0, 0));

        model = glm::scale(model, glm::vec3(params.scale));

        glm::mat4 view = glm::lookAt(
            glm::vec3(0, 0, 3),
            glm::vec3(0, 0, 0),
            glm::vec3(0, 1, 0)
        );

        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            (float)context.frameWidth / (float)context.frameHeight,
            0.1f,
            100.0f
        );

        glm::mat4 mvp = projection * view * model;

        shader->setMat4("uMVP", mvp);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(4.0f);

        glBindVertexArray(mesh->VAO);

        glDrawElements(GL_TRIANGLES,
            static_cast<GLsizei>(mesh->indices.size()),
            GL_UNSIGNED_INT,
            0);

        glBindVertexArray(0);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
};