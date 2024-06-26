#ifndef MAP_HPP
#define MAP_HPP

#include "Tile.hpp"
#include <filesystem>
#include <map>
#include <thread>

class Map {
public:
    enum class LOD {
        NATIVE,
        HIGH,
        MEDIUM,
        LOW,
        AWFUL,
        POTATO,
        NONE,
        USOS,

        AUTO
    };

    const static std::map<LOD, unsigned> sideLengths;

    Map(const std::vector<std::string> &dirNames,
                std::pair<int, int> xRange, std::pair<int, int> yRange);

    ~Map();

    static LOD lod;
    static bool flat;

    static int renderedTriangles;

    void update() const;

    void draw(const glm::mat4 &view, const glm::mat4 &projection) const;

    void recompileShaders();

    static void setLOD(LOD newLod);

private:
    static std::vector<Tile> tiles;
    static LOD effectiveLOD;

    std::thread enqueueThread;

    static void setRenderedTriangles();

    static void enqueueTiles(const std::filesystem::path &dirName,
                             std::pair<int, int> xRange, std::pair<int, int> yRange);

    static void enqueueTilesMulti(const std::vector<std::string> &dirNames,
                                  std::pair<int, int> xRange, std::pair<int, int> yRange);

    friend Tile;
};

#endif