#include "visualizer/SphereMeshRenderer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "utils/Paths.h"

SpheremeshRenderer::SpheremeshRenderer(ResourceManager& rm)
    : Renderer(rm)
{}

void SpheremeshRenderer::Init()
{
    if (!resourceManager.GetShader("sphere")) {
        auto shader = std::make_unique<Lengine::GLSLProgram>();
        shader->compileShaders(
            rootFolderPath + "/assets/shaders/sphere.vert",
            rootFolderPath + "/assets/shaders/sphere.frag"
        );
        shader->linkShaders();
        resourceManager.AddShader("sphere", std::move(shader));
    }

    if (!resourceManager.GetMesh("sphere")) {
        auto mesh = std::make_unique<Mesh>(
            "sphere",
            rootFolderPath + "assets/meshes/sphere.obj"
        );
        resourceManager.AddMesh("sphere", std::move(mesh));
    }
}

void SpheremeshRenderer::Render(RenderContext& context)
{
    Lengine::GLSLProgram* shader = resourceManager.GetShader("sphere");
    Mesh* mesh = resourceManager.GetMesh("sphere");

    if (!shader) {
        std::cout << "Sphere Shader not found\n";
        return;
    }

    if (!mesh) {
        std::cout << "Sphere Mesh not found\n";
        return;
    }

    shader->use();

    // Audio uniforms
    shader->setFloat("uBass", context.bass);
    shader->setFloat("uMid", context.mid);
    shader->setFloat("uTreble", context.treble);

    // Time
    shader->setFloat("uTime", context.time);

    // MVP matrices
    glm::mat4 model = glm::mat4(1.0f);

    glm::mat4 view = glm::lookAt(
        glm::vec3(0, 0, 3),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 1, 0)
    );

    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f),
        static_cast<float>(context.frameWidth) /
        static_cast<float>(context.frameHeight),
        0.1f,
        100.0f
    );

    shader->setMat4("uMVP", projection * view * model);

    // Wireframe mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(4.0f);

    glBindVertexArray(mesh->VAO);

    glDrawElements(
        GL_TRIANGLES,
        static_cast<GLsizei>(mesh->indices.size()),
        GL_UNSIGNED_INT,
        0
    );

    glBindVertexArray(0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}