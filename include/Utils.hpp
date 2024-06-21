#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

namespace Utils {
    void checkError(const std::string &message);
    float getDeltaTime();
    void markFrameTime();
    float getFPS();
    void sphericalToMercator(float longitude, float latitude, float &x, float &y);
    void mercatorToSpherical(float x, float y, float &longitude, float &latitude);
    bool inRange(float value, float min, float max);
}

#endif