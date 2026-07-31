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

    std::string name;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    GLuint VAO, VBO, EBO;

private:

   
};

