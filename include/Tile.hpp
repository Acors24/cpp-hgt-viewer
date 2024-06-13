#ifndef TILE_HPP
#define TILE_HPP

#include "Drawable.hpp"

class Tile : public Drawable {
public:
    explicit Tile(int longitude, int latitude,
                  const std::filesystem::path &hgtFile);

    void draw(const glm::mat4 &view,
              const glm::mat4 &projection) const override;

    static void updateIndices(unsigned step);

private:
    float longitude;
    float latitude;

    static std::vector<unsigned> indices;

    static GLuint vao;
    static GLuint vbo;
    static GLuint ebo;
    static GLuint shaderProgram;
    GLuint texture;

    void initializeMesh() override;
    std::filesystem::path getShaderPath() const override;
    bool &isInitialized() override;

    std::vector<unsigned> &getIndices() override;

    GLuint &getVao() const override;
    GLuint &getVbo() const override;
    GLuint &getEbo() const override;
    GLuint &getShaderProgram() const override;

    static bool initialized;
};

#endif