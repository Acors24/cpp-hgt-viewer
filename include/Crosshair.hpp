#ifndef CROSSHAIR_HPP
#define CROSSHAIR_HPP

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Drawable.hpp"

class Crosshair : public Drawable {
public:
    Crosshair();
    
    void draw(const glm::mat4 &view,
              const glm::mat4 &projection) const override;

private:
    static std::vector<unsigned> indices;

    static GLuint vao;
    static GLuint vbo;
    static GLuint ebo;
    static GLuint shaderProgram;

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