#include "Utils.hpp"

#include <GLFW/glfw3.h>
#include <cmath>
#include <glm/ext/scalar_constants.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>
#include <list>

namespace Utils {

void checkError(const std::string &message) {
    if (GLenum error = glGetError(); error != GL_NO_ERROR) {
        std::cerr << message << ": " << error << std::endl;
    }
}

inline float previousFrameTime = -1000.0f;
inline float frameTime = 0.0f;
inline constexpr unsigned int DELTA_SIZE = 100;
inline std::list<float> deltas(DELTA_SIZE, 0.1f);
inline float deltaAvg = 0.1f;

void markFrameTime() {
    previousFrameTime = frameTime;
    frameTime = static_cast<float>(glfwGetTime());

    float newDelta = getDeltaTime();
    deltaAvg = (deltaAvg * DELTA_SIZE + newDelta - deltas.front()) / DELTA_SIZE;
    deltas.pop_front();
    deltas.push_back(newDelta);
}

float getDeltaTime() { return frameTime - previousFrameTime; }

float getFPS() { return 1.0f / deltaAvg; }

void sphericalToMercator(float longitude, float latitude, float &x, float &y) {
    longitude = glm::radians(longitude);
    latitude = glm::radians(latitude);

    x = 1.0f / (2.0f * glm::pi<float>()) * longitude;
    y = 1.0f / (2.0f * glm::pi<float>()) *
        std::log(std::tan(glm::pi<float>() / 4.0f + latitude / 2.0f));
}

void mercatorToSpherical(float x, float y, float &longitude, float &latitude) {
    longitude = 2.0f * glm::pi<float>() * x;
    latitude = 2.0f * std::atan(std::exp(2.0f * glm::pi<float>() * y)) -
               glm::pi<float>() / 2.0f;

    longitude = glm::degrees(longitude);
    latitude = glm::degrees(latitude);
}

bool inRange(float value, float a, float b) {
    if (a < b) {
        return a <= value && value <= b;
    } else {
        return value <= b || a <= value;
    }
}

} // namespace Utils