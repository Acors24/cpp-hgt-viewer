#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

namespace Utils {
    void checkError(const std::string &message);
    float getDeltaTime();
    void markFrameTime();
    float getFPS();
    void sphericalToMercator(float longitude, float latitude, float &x, float &y);
}

#endif