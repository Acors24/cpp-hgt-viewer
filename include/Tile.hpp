#ifndef TILE_HPP
#define TILE_HPP

#include "Drawable.hpp"
#include <mutex>
#include <queue>

class Tile : public Drawable {
public:
    Tile(int longitude, int latitude, const std::vector<float> &heights);

    void draw(const glm::mat4 &view,
              const glm::mat4 &projection) const override;

    static void updateIndices(unsigned step);

    static std::queue<std::tuple<int, int, std::filesystem::path, std::vector<float>>> tilesToLoad;

    static void loadTile();

    static void enqueueTile(int longitude, int latitude,
                            const std::filesystem::path &hgtFile);

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

    static std::mutex tileMutex;

    friend class Map;
};

#endif