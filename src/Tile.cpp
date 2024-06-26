#include "Tile.hpp"
#include "Map.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>

void readHeights(const std::filesystem::path &hgtFile, std::vector<float> &heights) {
    std::ifstream file(hgtFile, std::ios::binary);
    if (!file.is_open()) {
        throw std::filesystem::filesystem_error(
            "Failed to open file: " + hgtFile.string(),
            std::make_error_code(std::errc::io_error));
    }

    unsigned char buf[2];
    for (int h = 0; h < 1201 * 1201; h++) {
        file.read(reinterpret_cast<char *>(buf), sizeof(buf));
        heights[h] = (short)((buf[0] << 8) | buf[1]);
    }

    file.close();
}

void Tile::loadTile() {
    std::scoped_lock lock(tileMutex);
    if (tilesToLoad.empty())
        return;

    auto [longitude, latitude, hgtFile, heights] = tilesToLoad.front();
    tilesToLoad.pop();

    auto tile = Tile(longitude, latitude, heights);

    Map::tiles.push_back(tile);

    std::cout << "Loaded " << hgtFile << "\r";
}

Tile::Tile(int longitude, int latitude, const std::vector<float> &heights)
    : longitude(longitude), latitude(latitude) {
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 1201, 1201, 0, GL_RED, GL_FLOAT,
                 heights.data());
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    initialize();
}

void Tile::enqueueTile(int longitude, int latitude, const std::filesystem::path &hgtFile) {
    auto heights = std::vector<float>(1201 * 1201);
    readHeights(hgtFile, heights);

    std::scoped_lock lock(tileMutex);
    tilesToLoad.emplace(longitude, latitude, hgtFile, heights);
}

std::filesystem::path Tile::getShaderPath() const { return "shaders/Tile"; }

void Tile::initializeMesh() {
    indices.reserve(1201 * 2 * 1200 + 1200);
    updateIndices(1);
}

void Tile::updateIndices(unsigned step) {
    indices.clear();
    for (int y = 0; y < 1201 - step; y += step) {
        for (int x = 0; x < 1201; x += step) {
            indices.push_back(x + y * 1201);
            indices.push_back(x + (y + step) * 1201);
        }
        indices.push_back(UINT_MAX);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned),
                 indices.data(), GL_STATIC_DRAW);
}

void Tile::draw(const glm::mat4 &view, const glm::mat4 &projection) const {
    bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glPrimitiveRestartIndex(UINT_MAX);

    glUniformMatrix4fv(0, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(1, 1, GL_FALSE, &projection[0][0]);
    glUniform1fv(2, 1, &longitude);
    glUniform1fv(3, 1, &latitude);
    float flat = Map::flat ? 1.0f : 0.0f;
    glUniform1fv(4, 1, &flat);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawElements(GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_INT, nullptr);
}

int Tile::getShaderTypes() const {
    return ShaderType::VERTEX | ShaderType::FRAGMENT;
}

GLuint &Tile::getVao() const { return vao; }

GLuint &Tile::getVbo() const { return vbo; }

GLuint &Tile::getEbo() const { return ebo; }

GLuint &Tile::getShaderProgram() const { return shaderProgram; }

bool &Tile::isInitialized() { return initialized; }

std::vector<unsigned> Tile::indices;

std::vector<unsigned> &Tile::getIndices() { return indices; }

bool Tile::initialized = false;

GLuint Tile::vao;
GLuint Tile::vbo;
GLuint Tile::ebo;
GLuint Tile::shaderProgram;

std::queue<std::tuple<int, int, std::filesystem::path, std::vector<float>>> Tile::tilesToLoad;

std::mutex Tile::tileMutex;