#include "Map.hpp"
#include "Utils.hpp"

void parseFilename(const std::string &filename, int &lon, int &lat) {
    lon = std::stoi(filename.substr(4, 3));
    lat = std::stoi(filename.substr(1, 2));

    if (filename[0] == 'S')
        lat *= -1;

    if (filename[3] == 'W')
        lon *= -1;
}

Map::Map(const std::filesystem::path &dirName, std::pair<int, int> xRange,
         std::pair<int, int> yRange) {
    for (const auto &entry : std::filesystem::directory_iterator(dirName)) {
        std::string filename = entry.path().filename().string();
        if (filename.substr(filename.find_last_of(".") + 1) != "hgt")
            continue;

        int lon;
        int lat;
        parseFilename(filename, lon, lat);

        if (lon < xRange.first || lon > xRange.second || lat < yRange.first ||
            lat > yRange.second)
            continue;

        tiles.emplace_back(lon, lat, entry.path());
    }
}

void Map::draw(const glm::mat4 &view, const glm::mat4 &projection) const {
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
        }
    }

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