#pragma once

#include <vector>

#include "resources/ResourceManager.h"
#include "utils/Paths.h"

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

        shader->use();

        // Send audio uniforms
        shader->setFloat("uBass", context.bass);
        shader->setFloat("uMid", context.mid);
        shader->setFloat("uTreble", context.treble);

        // Send time
        shader->setFloat("uTime", context.time);

        // Example MVP (replace later with your camera)
        glm::mat4 model = glm::mat4(1.0f);

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

        // Wireframe look like visualizers
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(4.0f);

        glBindVertexArray(mesh->VAO);

         
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh->indices.size()), GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
};