#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <glm/glm.hpp>

#include <tiny_obj_loader.h>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

inline void loadOBJ(const std::string& path,
    std::vector<Vertex>& vertices,
    std::vector<unsigned int>& indices)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn, err;

    bool success = tinyobj::LoadObj(
        &attrib,
        &shapes,
        &materials,
        &warn,
        &err,
        path.c_str(),
        nullptr,
        true // triangulate
    );

    if (!warn.empty())
        std::cout << warn << std::endl;

    if (!err.empty())
        std::cerr << err << std::endl;

    if (!success) {
        std::cerr << "Failed to load OBJ: " << path << std::endl;
        return;
    }

    vertices.clear();
    indices.clear();

    for (const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            Vertex vertex{};

            // position
            vertex.position = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };

            // normal
            if (index.normal_index >= 0)
            {
                vertex.normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]
                };
            }

            // texcoord
            if (index.texcoord_index >= 0)
            {
                vertex.texCoord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    attrib.texcoords[2 * index.texcoord_index + 1]
                };
            }

            vertices.push_back(vertex);
            indices.push_back(static_cast<unsigned int>(indices.size()));
        }
    }

    std::cout << "Loaded OBJ (tinyobj): " << path
        << " | Vertices: " << vertices.size()
        << " | Indices: " << indices.size()
        << std::endl;
}