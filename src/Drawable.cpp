#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

#include "Drawable.hpp"

void Drawable::initialize() {
    if (isInitialized()) {
        return;
    }

    initializeMesh();
    compileShaders();
    setBuffers();

    isInitialized() = true;
}

void checkShaderError(GLuint shader, GLuint flag, bool isProgram,
                      const std::string &errorMessage) {
    int success;
    std::string infoLog;
    if (isProgram) {
        glGetProgramiv(shader, flag, &success);
    } else {
        glGetShaderiv(shader, flag, &success);
    }

    if (!success) {
        if (isProgram) {
            glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &success);
            infoLog.resize(success);
            glGetProgramInfoLog(shader, infoLog.size(), nullptr, &infoLog[0]);
        } else {
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &success);
            infoLog.resize(success);
            glGetShaderInfoLog(shader, infoLog.size(), nullptr, &infoLog[0]);
        }

        std::cerr << errorMessage << ": " << infoLog << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

GLuint compileShader(const std::filesystem::path &shaderPath, int shaderType) {
    std::string shaderSource;
    std::ifstream shaderFile(shaderPath);

    if (!shaderFile.is_open()) {
        std::cerr << "Failed to open shader file: " << shaderPath << std::endl;
        std::exit(EXIT_FAILURE);
    }

    shaderSource = std::string(std::istreambuf_iterator<char>(shaderFile),
                               std::istreambuf_iterator<char>());

    const char *shaderSourceC = shaderSource.c_str();

    GLuint shader = glCreateShader(shaderType);

    glShaderSource(shader, 1, &shaderSourceC, nullptr);
    glCompileShader(shader);

    std::string shaderTypeStr;
    switch (shaderType) {
    case GL_VERTEX_SHADER:
        shaderTypeStr = "VERTEX";
        break;
    case GL_TESS_CONTROL_SHADER:
        shaderTypeStr = "TESS_CONTROL";
        break;
    case GL_TESS_EVALUATION_SHADER:
        shaderTypeStr = "TESS_EVALUATION";
        break;
    case GL_GEOMETRY_SHADER:
        shaderTypeStr = "GEOMETRY";
        break;
    case GL_FRAGMENT_SHADER:
        shaderTypeStr = "FRAGMENT";
        break;
    default:
        shaderTypeStr = "UNKNOWN";
        break;
    }
    checkShaderError(shader, GL_COMPILE_STATUS, false,
                     "ERROR::SHADER::" + shaderTypeStr +
                         "::COMPILATION_FAILED");

    return shader;
}

void Drawable::compileShaders() const {
    std::string shaderPath = getShaderPath();
    getShaderProgram() = glCreateProgram();
    GLuint shaderProgram = getShaderProgram();
    int shaderTypes = getShaderTypes();

    if (shaderTypes & ShaderType::VERTEX) {
        GLuint vertexShader =
            compileShader(shaderPath + ".vert", GL_VERTEX_SHADER);
        glAttachShader(shaderProgram, vertexShader);
    }

    if (shaderTypes & ShaderType::TESS_CONTROL) {
        GLuint tessControlShader =
            compileShader(shaderPath + ".tesc", GL_TESS_CONTROL_SHADER);
        glAttachShader(shaderProgram, tessControlShader);
    }

    if (shaderTypes & ShaderType::TESS_EVALUATION) {
        GLuint tessEvaluationShader =
            compileShader(shaderPath + ".tese", GL_TESS_EVALUATION_SHADER);
        glAttachShader(shaderProgram, tessEvaluationShader);
    }

    if (shaderTypes & ShaderType::GEOMETRY) {
        GLuint geometryShader =
            compileShader(shaderPath + ".geom", GL_GEOMETRY_SHADER);
        glAttachShader(shaderProgram, geometryShader);
    }

    if (shaderTypes & ShaderType::FRAGMENT) {
        GLuint fragmentShader =
            compileShader(shaderPath + ".frag", GL_FRAGMENT_SHADER);
        glAttachShader(shaderProgram, fragmentShader);
    }

    glLinkProgram(shaderProgram);

    checkShaderError(shaderProgram, GL_LINK_STATUS, true,
                     "ERROR::SHADER::PROGRAM::LINKING_FAILED");

    glUseProgram(shaderProgram);
}

void Drawable::recompileShaders() const {
    glDeleteProgram(getShaderProgram());
    compileShaders();
}

void Drawable::setBuffers() {
    glGenVertexArrays(1, &getVao());
    glBindVertexArray(getVao());

    glGenBuffers(1, &getVbo());
    glBindBuffer(GL_ARRAY_BUFFER, getVbo());
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3),
                 &vertices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glGenBuffers(1, &getEbo());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, getEbo());
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 getIndices().size() * sizeof(unsigned int),
                 getIndices().data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Drawable::bind() const {
    glUseProgram(getShaderProgram());

    glBindVertexArray(getVao());
    glBindBuffer(GL_ARRAY_BUFFER, getVbo());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, getEbo());
}