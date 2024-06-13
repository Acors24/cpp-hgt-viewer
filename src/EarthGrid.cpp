#include "EarthGrid.hpp"
#include <cmath>
#include <glm/common.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

EarthGrid::EarthGrid() { initialize(); }

void EarthGrid::initializeMesh() {
    const int RES = EarthGrid::RESOLUTION;
    float x;
    float y;
    float z;
    float hTheta;
    float vTheta;
    float radius;

    vertices.reserve(NUM_VERTICES);
    vertices.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
    for (int j = 1; j < RES; j++) {
        vTheta = static_cast<float>(j * M_PI / RES);
        radius = std::sin(vTheta);

        for (int k = 0; k < 2 * RES; k++) {
            hTheta = static_cast<float>(k * M_PI / RES);

            x = std::cos(hTheta) * radius;
            y = std::cos(vTheta);
            z = std::sin(hTheta) * radius;
            vertices.push_back(glm::vec3(x, y, z));
        }
    }
    vertices.push_back(glm::vec3(0.0f, -1.0f, 0.0f));

    indices.reserve(NUM_INDICES);
    for (int i = 0; i < 2 * RES; i++) {
        indices.push_back(0);
        indices.push_back(i + 1);
    }
    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(UINT_MAX);
    for (int j = 0; j < RES - 2; j++) {
        for (int i = 0; i < 2 * RES; i++) {
            indices.push_back(1 + j * (2 * RES) + i);
            indices.push_back(1 + (j + 1) * (2 * RES) + i);
        }
        indices.push_back(1 + j * (2 * RES));
        indices.push_back(1 + (j + 1) * (2 * RES));
        indices.push_back(UINT_MAX);
    }
    for (int i = 0; i < 2 * RES; i++) {
        indices.push_back(NUM_VERTICES - 2 - i);
        indices.push_back(NUM_VERTICES - 1);
    }
    indices.push_back(NUM_VERTICES - 2);
    indices.push_back(NUM_VERTICES - 1);
}

std::filesystem::path EarthGrid::getShaderPath() const {
    return std::filesystem::path("shaders/EarthGrid");
}

bool &EarthGrid::isInitialized() { return initialized; }

glm::mat4 EarthGrid::getModelMatrix() const {
    auto model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(EARTH_RADIUS));

    return model;
}

void EarthGrid::draw(const glm::mat4 &view, const glm::mat4 &projection) const {
    glm::mat4 model = getModelMatrix();

    bind();
    glUniformMatrix4fv(0, 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(1, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(2, 1, GL_FALSE, &projection[0][0]);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(1);
    glPrimitiveRestartIndex(UINT_MAX);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glDrawElements(GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_INT, nullptr);
    glDisable(GL_CULL_FACE);
}

void EarthGrid::sphericalToCartesian(float phi, float theta, float r,
                                     float &outX, float &outY, float &outZ) {
    outX = static_cast<float>(r * -cos(theta) * cos(phi));
    outY = static_cast<float>(r * sin(theta));
    outZ = static_cast<float>(r * cos(theta) * sin(phi));
}

void EarthGrid::cartesianToSpherical(float x, float y, float z, float &outPhi,
                                     float &outTheta, float &outR) {
    outR = std::sqrt(x * x + y * y + z * z);
    outTheta = std::asin(y / outR);
    outPhi = std::acos(x / outR / -std::cos(outTheta)) * glm::sign(z);
}

GLuint &EarthGrid::getVao() const { return vao; }

GLuint &EarthGrid::getVbo() const { return vbo; }

GLuint &EarthGrid::getEbo() const { return ebo; }

GLuint &EarthGrid::getShaderProgram() const { return shaderProgram; }

std::vector<unsigned> EarthGrid::indices;

std::vector<unsigned> &EarthGrid::getIndices() { return indices; }

bool EarthGrid::initialized = false;

GLuint EarthGrid::vao;
GLuint EarthGrid::vbo;
GLuint EarthGrid::ebo;
GLuint EarthGrid::shaderProgram;