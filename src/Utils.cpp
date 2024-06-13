#include "Utils.hpp"

#include <cmath>
#include <epoxy/gl.h>
#include <GLFW/glfw3.h>
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

float getDeltaTime() {
    return frameTime - previousFrameTime;
}


float getFPS() {
    return 1.0f / deltaAvg;
}

void sphericalToMercator(float longitude, float latitude, float &x, float &y) {
    x = longitude / 180.0f;
    y = std::log(std::tan((latitude + 90.0f) * M_PI / 360.0f));
}

} // namespace Utils