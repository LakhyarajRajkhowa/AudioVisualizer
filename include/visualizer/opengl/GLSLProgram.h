#pragma once

#include <string>
#include <iostream>
#include <stdexcept>

#include <SDL/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Lengine {

    inline void fatalError(const std::string& errorString)
    {
        throw std::runtime_error(errorString);
    }

    class GLSLProgram
    {

    public:
        GLSLProgram();
        ~GLSLProgram();

        void compileShaders(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath);
        void compileShaders_3(
            const std::string& vertexShaderFilePath,
            const std::string& geometryShaderFilePath,
            const std::string& fragmentShaderFilePath
           );

        void linkShaders();
        void addAtrribute(const std::string& attributeName);

        GLint getUnifromLocation(const std::string& uniformName);

        void use();
        void unuse();
        void setMat4(const std::string& name, const glm::mat4& mat);
        void setMat3(const std::string& name, const glm::mat3& mat);
        void setVec3(const std::string& name, const glm::vec3& vec);
        void setVec4(const std::string& name, const glm::vec4& vec);
        void setFloat(const std::string& name, float value);
        void setInt(const std::string& name, int value);
        void setBool(const std::string& name, bool state);
          
    private:

        void compileShader(const std::string& filePath, GLuint id);

        int _numAttributes;
        GLuint _programID;
        GLuint _vertexShaderID;
        GLuint _fragmentShaderID;
        GLuint _geometryShaderID = 0;


    };
}



