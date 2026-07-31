#pragma once

#include <unordered_map>
#include <string>
#include <memory>

#include "resources/Mesh.h"
#include "visualizer/opengl/GLSLProgram.h"

class ResourceManager
{
public:

    // -------- Mesh Management --------

    bool AddMesh(const std::string& name, std::unique_ptr<Mesh> mesh)
    {
        return meshes.emplace(name, std::move(mesh)).second;
    }

    bool RemoveMesh(const std::string& name)
    {
        return meshes.erase(name) > 0;
    }

    Mesh* GetMesh(const std::string& name)
    {
        auto it = meshes.find(name);
        if (it != meshes.end())
            return it->second.get();
        return nullptr;
    }

    const std::unordered_map<std::string, std::unique_ptr<Mesh>>& GetMeshes() const
    {
        return meshes;
    }


    // -------- Shader Management --------

    bool AddShader(const std::string& name, std::unique_ptr<Lengine::GLSLProgram> shader)
    {
        return shaders.emplace(name, std::move(shader)).second;
    }

    bool RemoveShader(const std::string& name)
    {
        return shaders.erase(name) > 0;
    }

    Lengine::GLSLProgram* GetShader(const std::string& name)
    {
        auto it = shaders.find(name);
        if (it != shaders.end())
            return it->second.get();
        return nullptr;
    }

    const std::unordered_map<std::string, std::unique_ptr<Lengine::GLSLProgram>>& GetShaders() const
    {
        return shaders;
    }


private:

    std::unordered_map<std::string, std::unique_ptr<Mesh>> meshes;
    std::unordered_map<std::string, std::unique_ptr<Lengine::GLSLProgram>> shaders;
};