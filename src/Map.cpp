#include "Map.hpp"
#include "Utils.hpp"
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <thread>

void parseFilename(const std::string &filename, int &lon, int &lat) {
    lon = std::stoi(filename.substr(4, 3));
    lat = std::stoi(filename.substr(1, 2));

    if (filename[0] == 'S')
        lat *= -1;

    if (filename[3] == 'W')
        lon *= -1;
}

void Map::enqueueTiles(const std::filesystem::path &dirName, std::pair<int, int> xRange,
                  std::pair<int, int> yRange) {
    try {
        for (const auto &entry : std::filesystem::directory_iterator(dirName)) {
            if (entry.is_directory()) {
                enqueueTiles(entry.path(), xRange, yRange);
                continue;
            }

            std::string filename = entry.path().filename().string();
            if (filename.find_last_of(".hgt") == std::string::npos)
                continue;

            int lon;
            int lat;
            parseFilename(filename, lon, lat);

            if (!Utils::inRange(static_cast<float>(lon), static_cast<float>(xRange.first), static_cast<float>(xRange.second)) ||
                !Utils::inRange(static_cast<float>(lat), static_cast<float>(yRange.first), static_cast<float>(yRange.second)))
                continue;

            if (std::find_if(tiles.begin(), tiles.end(), [lon, lat](const Tile &tile) {
                    return tile.longitude == static_cast<float>(lon) && tile.latitude == static_cast<float>(lat);
                }) != tiles.end())
                continue;
                
            Tile::enqueueTile(lon, lat, entry.path());
        }
    } catch (const std::filesystem::filesystem_error &e) {
        std::cerr << e.what() << std::endl;
    }
}

void Map::enqueueTilesMulti(const std::vector<std::string> &dirNames, std::pair<int, int> xRange,
                  std::pair<int, int> yRange) {
    for (const auto &dirName : dirNames) {
        enqueueTiles(dirName, xRange, yRange);
    }
}

Map::Map(const std::vector<std::string> &dirNames, std::pair<int, int> xRange,
         std::pair<int, int> yRange) {
    enqueueThread = std::thread(enqueueTilesMulti, dirNames, xRange, yRange);
    setLOD(LOD::AUTO);
}

Map::~Map() {
    enqueueThread.join();
}

void Map::update() const {
    Tile::loadTile();
}

void Map::setRenderedTriangles() {
    if (lod == LOD::AUTO) {
        Map::renderedTriangles = 0;
        return;
    }

    int sideLength = Map::sideLengths.at(lod);
    Map::renderedTriangles = sideLength * sideLength * 2;
    Map::renderedTriangles *= Map::tiles.size();
}

void Map::draw(const glm::mat4 &view, const glm::mat4 &projection) const {
    for (const auto &tile : tiles) {
        tile.draw(view, projection);
    }
}

void Map::setLOD(LOD newLod) {
    Map::lod = newLod;

    if (newLod == LOD::AUTO) {
        Tile::setLOD(-1.0f);
        return;
    }

    Map::effectiveLOD = newLod;

    Tile::setLOD(static_cast<float>(sideLengths.at(newLod)));
    setRenderedTriangles();
}

void Map::recompileShaders() {
    tiles[0].recompileShaders();
}

Map::LOD Map::lod = Map::LOD::AUTO;
Map::LOD Map::effectiveLOD = Map::LOD::USOS;
bool Map::flat = true;
const std::map<Map::LOD, unsigned> Map::sideLengths = {
    {Map::LOD::NATIVE, 1200}, {Map::LOD::HIGH, 800},   {Map::LOD::MEDIUM, 400},
    {Map::LOD::LOW, 200},    {Map::LOD::AWFUL, 100}, {Map::LOD::POTATO, 60},
    {Map::LOD::NONE, 40},  {Map::LOD::USOS, 20}};
std::vector<Tile> Map::tiles;
int Map::renderedTriangles = 0;