#include "Map.hpp"
#include "Utils.hpp"
#include <thread>

void parseFilename(const std::string &filename, int &lon, int &lat) {
    lon = std::stoi(filename.substr(4, 3));
    lat = std::stoi(filename.substr(1, 2));

    if (filename[0] == 'S')
        lat *= -1;

    if (filename[3] == 'W')
        lon *= -1;
}

void enqueueTiles(const std::filesystem::path &dirName, std::pair<int, int> xRange,
                  std::pair<int, int> yRange) {
    for (const auto &entry : std::filesystem::directory_iterator(dirName)) {
        std::string filename = entry.path().filename().string();
        if (filename.find_last_of(".hgt") == std::string::npos)
            continue;

        int lon;
        int lat;
        parseFilename(filename, lon, lat);

        if (!Utils::inRange(static_cast<float>(lon), static_cast<float>(xRange.first), static_cast<float>(xRange.second)) ||
            !Utils::inRange(static_cast<float>(lat), static_cast<float>(yRange.first), static_cast<float>(yRange.second)))
            continue;
            
        Tile::enqueueTile(lon, lat, entry.path());
    }
}

Map::Map(const std::filesystem::path &dirName, std::pair<int, int> xRange,
         std::pair<int, int> yRange) {
    enqueueThread = std::thread(enqueueTiles, dirName, xRange, yRange);
}

Map::~Map() {
    enqueueThread.join();
}

void Map::update() const {
    if (Map::lod == LOD::AUTO) {
        auto fps = 1.0f / Utils::getDeltaTime();
        bool changed = false;
        if (fps < 20 && Map::effectiveLOD != LOD::USOS) {
            Map::effectiveLOD =
                static_cast<LOD>(static_cast<int>(Map::effectiveLOD) + 1);
            changed = true;
        } else if (fps > 60 && Map::effectiveLOD != LOD::NATIVE) {
            Map::effectiveLOD =
                static_cast<LOD>(static_cast<int>(Map::effectiveLOD) - 1);
            changed = true;
        }

        if (changed) {
            Tile::updateIndices(lodIndices.at(Map::effectiveLOD));
            setRenderedTriangles();
        }
    }

    Tile::loadTile();
}

void Map::setRenderedTriangles() {
    int sideLength = 1200 / Map::lodIndices.at(effectiveLOD);
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
        return;
    }

    Map::effectiveLOD = newLod;

    Tile::updateIndices(lodIndices.at(newLod));
    setRenderedTriangles();
}

void Map::recompileShaders() {
    tiles[0].recompileShaders();
}

Map::LOD Map::lod = Map::LOD::AUTO;
Map::LOD Map::effectiveLOD = Map::LOD::POTATO;
bool Map::flat = true;
const std::map<Map::LOD, unsigned> Map::lodIndices = {
    {Map::LOD::NATIVE, 1}, {Map::LOD::HIGH, 2},   {Map::LOD::MEDIUM, 4},
    {Map::LOD::LOW, 8},    {Map::LOD::AWFUL, 16}, {Map::LOD::POTATO, 30},
    {Map::LOD::NONE, 60},  {Map::LOD::USOS, 120}};
std::vector<Tile> Map::tiles;
int Map::renderedTriangles = 0;