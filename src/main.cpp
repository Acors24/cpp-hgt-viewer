#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/trigonometric.hpp>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <random>

#include "Camera.hpp"
#include "Crosshair.hpp"
#include "EarthGrid.hpp"
#include "Map.hpp"
#include "Utils.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>

int width = 1280;
int height = 720;

void sizeCallback(GLFWwindow *, int width, int height) {
    ::width = width;
    ::height = height;
}

Camera camera;

void processInputs(GLFWwindow *window, float deltaTime, bool flat,
                   EarthGrid &grid, Crosshair &crosshair, Map &map) {
    static bool tabHeld = false;

    bool shiftPressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard(Key::W, deltaTime, flat, shiftPressed);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboard(Key::S, deltaTime, flat, shiftPressed);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard(Key::A, deltaTime, flat, shiftPressed);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard(Key::D, deltaTime, flat, shiftPressed);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera.processKeyboard(Key::W, deltaTime, flat, shiftPressed);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.processKeyboard(Key::S, deltaTime, flat, shiftPressed);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        camera.processKeyboard(Key::A, deltaTime, flat, shiftPressed);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        camera.processKeyboard(Key::D, deltaTime, flat, shiftPressed);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.processKeyboard(Key::SPACE, deltaTime, flat, shiftPressed);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.processKeyboard(Key::LEFT_CONTROL, deltaTime, flat,
                               shiftPressed);

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        Map::setLOD(Map::LOD::NATIVE);
    else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        Map::setLOD(Map::LOD::HIGH);
    else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        Map::setLOD(Map::LOD::MEDIUM);
    else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
        Map::setLOD(Map::LOD::LOW);
    else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
        Map::setLOD(Map::LOD::AWFUL);
    else if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
        Map::setLOD(Map::LOD::POTATO);
    else if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
        Map::setLOD(Map::LOD::NONE);
    else if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
        Map::setLOD(Map::LOD::USOS);
    else if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
        Map::setLOD(Map::LOD::AUTO);

    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
        if (!tabHeld) {
            Map::flat = !Map::flat;
            camera.setMode(Map::flat);
            tabHeld = true;
        }
    } else {
        tabHeld = false;
    }

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        grid.recompileShaders();
        crosshair.recompileShaders();
        map.recompileShaders();
    }
}

float mapScale = 10.0f;

void scrollCallback(GLFWwindow *, double xOffset, double yOffset) {
    camera.processScroll(xOffset, yOffset, Map::flat);
    if (Map::flat) {
        mapScale -= yOffset * 0.2f;
        if (mapScale < 0.1f)
            mapScale = 0.1f;
        if (mapScale > 40.0f)
            mapScale = 40.0f;
    }
}

void mouseCallback(GLFWwindow *window, double xPos, double yPos) {
    static bool first = true;
    static double previousX = 0;
    static double previousY = 0;

    if (first) {
        previousX = xPos;
        previousY = yPos;
        first = false;
    }

    auto xOffset = static_cast<float>(xPos - previousX);
    auto yOffset = static_cast<float>(yPos - previousY);

    previousX = xPos;
    previousY = yPos;

    bool shiftPressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
    camera.processMouseMovement(xOffset, yOffset, Map::flat, shiftPressed,
                                true);
}

int run(const std::vector<std::string> &dirNames, std::pair<int, int> xRange,
        std::pair<int, int> yRange, std::tuple<float, float, float> startPos) {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return 1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    std::string windowTitle = "Height map viewer -";
    for (const auto &dirName : dirNames)
        windowTitle += " " + dirName;
    GLFWwindow *window =
        glfwCreateWindow(1280, 720, windowTitle.c_str(), nullptr, nullptr);

    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window\n";
        return 1;
    }

    glfwMakeContextCurrent(window);

    glfwGetFramebufferSize(window, &width, &height);
    glfwSetFramebufferSizeCallback(window, sizeCallback);
    glfwSetWindowSizeCallback(window, sizeCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PRIMITIVE_RESTART);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    std::get<2>(startPos) += EarthGrid::EARTH_RADIUS;
    camera = Camera(startPos);

    EarthGrid grid;
    Crosshair crosshair;
    Map map(dirNames, xRange, yRange);

    float deltaTime;
    do {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        deltaTime = Utils::getDeltaTime();

        processInputs(window, deltaTime, Map::flat, grid, crosshair, map);

        glm::mat4 viewMatrix = camera.getViewMatrix();

        glViewport(0, 0, width, height);

        float aspect = (float)width / (float)height;

        map.update();

        if (Map::flat) {
            glm::mat4 orthoMatrix =
                glm::ortho(-mapScale * aspect, mapScale * aspect, -mapScale,
                           mapScale, -10.0f, 10.0f);
            map.draw(viewMatrix, orthoMatrix);
            glClear(GL_DEPTH_BUFFER_BIT);
            crosshair.draw(viewMatrix, orthoMatrix);
        } else {
            glm::mat4 perspectiveMatrix = glm::perspective(
                glm::radians(camera.fov), aspect, 0.001f, 10000.0f);
            map.draw(viewMatrix, perspectiveMatrix);
            grid.draw(viewMatrix, perspectiveMatrix);
        }

        std::cout << std::setw(10) << std::left << std::scientific
                  << std::setprecision(3)
                  << static_cast<float>(Map::renderedTriangles) / deltaTime
                  << " triangles/sec. " << std::fixed << std::setw(4)
                  << std::right << Utils::getFPS() << " FPS";
        std::cout << " | " << camera.longitude << " " << camera.latitude;
        std::cout << "  \r" << std::flush;

        glfwSwapBuffers(window);
        glfwPollEvents();
        Utils::markFrameTime();
    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
             glfwWindowShouldClose(window) == 0);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

std::pair<int, int> extractLonRange(std::vector<std::string> &args) {
    std::pair<int, int> lonRange = {-180, 180};
    if (auto arg = std::find(args.begin(), args.end(), "-lon"); arg != args.end()) {
        int consumed = 1;
        try {
            lonRange.first = std::stoi(*(arg + 1));
            consumed += 1;
            lonRange.second = std::stoi(*(arg + 2));
            consumed += 1;
        } catch (std::out_of_range &) {
            std::cerr << "Missing argument for -lon\n";
        } catch (std::invalid_argument &) {
            std::cerr << "Invalid argument for -lon\n";
        }
        args.erase(arg, arg + consumed);
    }
    return lonRange;
}

std::pair<int, int> extractLatRange(std::vector<std::string> &args) {
    std::pair<int, int> latRange = {-90, 90};
    if (auto arg = std::find(args.begin(), args.end(), "-lat"); arg != args.end()) {
        int consumed = 1;
        try {
            latRange.first = std::stoi(*(arg + 1));
            consumed += 1;
            latRange.second = std::stoi(*(arg + 2));
            consumed += 1;
        } catch (std::out_of_range &) {
            std::cerr << "Missing argument for -lat\n";
        } catch (std::invalid_argument &) {
            std::cerr << "Invalid argument for -lat\n";
        }
        args.erase(arg, arg + consumed);
    }
    return latRange;
}

std::tuple<float, float, float> extractStartPos(std::vector<std::string> &args) {
    float startLon = 0.0f;
    float startLat = 0.0f;
    float startAlt = 1.0f;
    if (auto arg = std::find(args.begin(), args.end(), "-start"); arg != args.end()) {
        int consumed = 1;
        try {
            startLon = std::stof(*(arg + 1));
            consumed += 1;
            startLat = std::stof(*(arg + 2));
            consumed += 1;
            startAlt = std::stof(*(arg + 3));
            consumed += 1;
        } catch (std::out_of_range &) {
            std::cerr << "Missing argument for -start\n";
        } catch (std::invalid_argument &) {
            std::cerr << "Invalid argument for -start\n";
        }
        args.erase(arg, arg + consumed);
    }
    return {startLon, startLat, startAlt};
}

bool testSMConversion();
bool testCSConversion();

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Usage:\n";
        std::cout << "\t" << argv[0]
                  << " <directory/with/hgt/files> [-lon <x1> "
                     "<x2>] [-lat <y1> <y2>] [-start <lon> <lat> <alt>]\n";
        std::cout << "Coordinates are given in degrees.\n";
        std::cout << "\t-lon and -lat specify the range of the loaded tiles, "
                     "out of the ones in the specified directory.\n";
        std::cout << "\t-start specifies the starting position of the camera "
                     "in the 2D view.\n";
        std::cout << "Altitude is given in kilometers above sea level.\n";
        std::cout << "The order of the arguments does not matter.\n";

        return 1;
    }

    std::vector<std::string> args(argv, argv + argc);
    args.erase(args.begin());

    auto lonRange = extractLonRange(args);
    auto latRange = extractLatRange(args);
    auto startPos = extractStartPos(args);

    return run(args, lonRange, latRange, startPos);
}

bool testSMConversion() {
    std::random_device rd;
    std::mt19937 gen(rd());
    float x;
    float y;
    float x2;
    float y2;
    float lon;
    float lat;
    float lon2;
    float lat2;
    
    std::uniform_int_distribution lonGen(-180, 180);
    std::uniform_int_distribution latGen(-90, 90);
    for (int t = 0; t < 1000; t++) {
        lon = static_cast<float>(lonGen(gen));
        lat = static_cast<float>(latGen(gen));
        Utils::sphericalToMercator(lon, lat, x, y);
        Utils::mercatorToSpherical(x, y, lon2, lat2);
        if (std::abs(lon - lon2) > 0.001 || std::abs(lat - lat2) > 0.001) {
            std::cerr << "Conversion failed for:\n\tlon: " << lon << " lat: " << lat
                      << "\n\tx: " << x << " y: " << y
                      << "\n\tlon2: " << lon2 << " lat2: " << lat2 << "\n";
            return false;
        }
    }

    std::uniform_real_distribution xGen(-0.5f, 0.5f);
    std::uniform_real_distribution yGen(-glm::pi<float>() / 2.0f,
                                        glm::pi<float>() / 2.0f);
    for (int t = 0; t < 1000; t++) {
        x = xGen(gen);
        y = yGen(gen);
        Utils::mercatorToSpherical(x, y, lon, lat);
        Utils::sphericalToMercator(lon, lat, x2, y2);
        if (std::abs(x - x2) > 0.001 || std::abs(y - y2) > 0.001) {
            std::cerr << "Conversion failed for:\n\tx: " << x << " y: " << y
                      << "\n\tlon: " << lon << " lat: " << lat
                      << "\n\tx2: " << x2 << " y2: " << y2 << "\n";
            return false;
        }       
    }

    return true;
}

bool testCSConversion() {
    std::random_device rd;
    std::mt19937 gen(rd());
    float x;
    float y;
    float z;
    float x2;
    float y2;
    float z2;
    float lon;
    float lat;
    float alt;
    float lon2;
    float lat2;
    float alt2;
    
    std::uniform_int_distribution lonGen(-175, 175);
    std::uniform_int_distribution latGen(-85, 85);
    std::uniform_int_distribution altGen(10, 10000);
    for (int t = 0; t < 1000; t++) {
        lon = static_cast<float>(lonGen(gen));
        lat = static_cast<float>(latGen(gen));
        alt = static_cast<float>(altGen(gen));
        EarthGrid::sphericalToCartesian(lon, lat,
                                        alt, x, y, z);
        EarthGrid::cartesianToSpherical(x, y, z, lon2, lat2, alt2);
        if (std::abs(lon - lon2) > 0.001 || std::abs(lat - lat2) > 0.001 || std::abs(alt - alt2) > 0.001) {
            std::cerr << "Test " << t << " failed\n";
            std::cerr << "Conversion failed for:\n\tlon: " << lon << " lat: " << lat << " alt: " << alt
                      << "\n\tx: " << x << " y: " << y << " z: " << z
                      << "\n\tlon2: " << lon2 << " lat2: " << lat2 << " alt2: " << alt2 << "\n";
            return false;
        }
    }

    std::uniform_real_distribution xGen(-1.0f, 1.0f);
    std::uniform_real_distribution yGen(-1.0f, 1.0f);
    std::uniform_real_distribution zGen(-1.0f, 1.0f);
    for (int t = 0; t < 1000; t++) {
        x = xGen(gen);
        y = yGen(gen);
        z = zGen(gen);
        EarthGrid::cartesianToSpherical(x, y, z, lon, lat, alt);
        EarthGrid::
        sphericalToCartesian(lon, lat,
                             alt, x2, y2, z2);
        if (std::abs(x - x2) > 0.001 || std::abs(y - y2) > 0.001 || std::abs(z - z2) > 0.001) {
            std::cerr << "Conversion failed for:\n\tx: " << x << " y: " << y << " z: " << z
                      << "\n\tlon: " << lon << " lat: " << lat << " alt: " << alt
                      << "\n\tx2: " << x2 << " y2: " << y2 << " z2: " << z2 << "\n";
            return false;
        }
    }

    return true;
}