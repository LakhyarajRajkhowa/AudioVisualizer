#pragma once

#include <vector>
#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "utils/ObjLoader.h"

class Mesh {
public:
    Mesh(const std::string name, std::string objPath);
    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    bool IsValid() const { return valid; }

    std::string name;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;

private:
    bool valid = false;
};
