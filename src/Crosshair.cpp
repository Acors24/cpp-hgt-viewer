#include "Crosshair.hpp"
#include <cmath>

Crosshair::Crosshair() {
    initialize();
}

void Crosshair::initializeMesh() {
    vertices.reserve(4);
    vertices.push_back(glm::vec3(-1.0f, 0.0f, -1.0f));
    vertices.push_back(glm::vec3(1.0f, 0.0f, -1.0f));
    vertices.push_back(glm::vec3(0.0f, -1.0f, -1.0f));
    vertices.push_back(glm::vec3(0.0f, 1.0f, -1.0f));

    indices.reserve(4);
    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(3);
}

std::filesystem::path Crosshair::getShaderPath() const {
    return std::filesystem::path("shaders/Crosshair");
}

bool &Crosshair::isInitialized() {
    return initialized;
}

void Crosshair::draw(const glm::mat4 &view, const glm::mat4 &projection) const {
    bind();

    glLineWidth(1);
    glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, nullptr);
}

GLuint &Crosshair::getVao() const {
    return vao;
}

GLuint &Crosshair::getVbo() const {
    return vbo;
}

GLuint &Crosshair::getEbo() const {
    return ebo;
}

GLuint &Crosshair::getShaderProgram() const {
    return shaderProgram;
}

int Crosshair::getShaderTypes() const {
    return ShaderType::VERTEX | ShaderType::FRAGMENT;
}

std::vector<unsigned> Crosshair::indices;

std::vector<unsigned> &Crosshair::getIndices() {
    return indices;
}

bool Crosshair::initialized = false;

GLuint Crosshair::vao;
GLuint Crosshair::vbo;
GLuint Crosshair::ebo;
GLuint Crosshair::shaderProgram;