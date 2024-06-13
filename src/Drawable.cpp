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

void Drawable::compileShaders() const {
    std::filesystem::path shaderPath = getShaderPath();
    std::string vertexShaderPath = shaderPath.string() + ".vs";
    std::string fragmentShaderPath = shaderPath.string() + ".fs";

    std::string vertexShaderSource;
    std::string fragmentShaderSource;

    std::ifstream vertexShaderFile(vertexShaderPath);
    std::ifstream fragmentShaderFile(fragmentShaderPath);

    if (!vertexShaderFile.is_open() || !fragmentShaderFile.is_open()) {
        std::cerr << "Failed to open shader files" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    vertexShaderSource =
        std::string(std::istreambuf_iterator<char>(vertexShaderFile),
                    std::istreambuf_iterator<char>());
    fragmentShaderSource =
        std::string(std::istreambuf_iterator<char>(fragmentShaderFile),
                    std::istreambuf_iterator<char>());

    const char *vs = vertexShaderSource.c_str();
    const char *fs = fragmentShaderSource.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vertexShader, 1, &vs, nullptr);
    glCompileShader(vertexShader);

    int success;
    std::string infoLog;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &success);
        infoLog.resize(success);
        glGetShaderInfoLog(vertexShader, infoLog.size(), nullptr, &infoLog[0]);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    glShaderSource(fragmentShader, 1, &fs, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &success);
        infoLog.resize(success);
        glGetShaderInfoLog(vertexShader, infoLog.size(), nullptr, &infoLog[0]);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    getShaderProgram() = glCreateProgram();
    GLuint shaderProgram = getShaderProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &success);
        infoLog.resize(success);
        glGetShaderInfoLog(vertexShader, infoLog.size(), nullptr, &infoLog[0]);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }

    glUseProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
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
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, getIndices().size() * sizeof(unsigned int),
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