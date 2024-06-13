#ifndef MAP_HPP
#define MAP_HPP

#include "Tile.hpp"
#include <filesystem>
#include <map>

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

    const static std::map<LOD, unsigned> lodIndices;

    Map(const std::filesystem::path &dirName,
                std::pair<int, int> xRange, std::pair<int, int> yRange);

    static LOD lod;
    static bool flat;

    void draw(const glm::mat4 &view, const glm::mat4 &projection) const;

    void recompileShaders();

    static void setLOD(LOD _lod);

private:
    std::vector<Tile> tiles;
    static LOD effectiveLOD;

    friend Tile;
};

#endif