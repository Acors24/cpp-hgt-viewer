#ifndef EARTHGRID_HPP
#define EARTHGRID_HPP

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Drawable.hpp"

class EarthGrid : public Drawable {
public:
    EarthGrid();

    void draw(const glm::mat4 &view,
              const glm::mat4 &projection) const override;

    static void sphericalToCartesian(float phi, float theta, float r,
                                     float &outX, float &outY, float &outZ);

    static void cartesianToSpherical(float x, float y, float z, float &outPhi,
                                     float &outTheta, float &outR);

    constexpr static float EARTH_RADIUS = 6378.0f;

private:
    static std::vector<unsigned> indices;

    static GLuint vao;
    static GLuint vbo;
    static GLuint ebo;
    static GLuint shaderProgram;

    constexpr static int RESOLUTION = 180;
    constexpr static int NUM_VERTICES = (RESOLUTION * 2) * (RESOLUTION - 1) + 2;
    constexpr static int NUM_INDICES =
        ((RESOLUTION * 2 + 1) * 2 + 1) * RESOLUTION - 1;
    glm::mat4 getModelMatrix() const;

    void initializeMesh() override;
    std::filesystem::path getShaderPath() const override;
    bool &isInitialized() override;

    std::vector<unsigned> &getIndices() override;

    GLuint &getVao() const override;
    GLuint &getVbo() const override;
    GLuint &getEbo() const override;
    GLuint &getShaderProgram() const override;

    int getShaderTypes() const override;

    static bool initialized;
};

#endif