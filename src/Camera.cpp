#include "Camera.hpp"
#include "EarthGrid.hpp"
#include <cmath>
#include <glm/trigonometric.hpp>

Camera::Camera() : Camera(glm::vec3(0.0f)) {}

Camera::Camera(const glm::vec3 &position) : position(position) {
    updateCameraVectors(false);
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + front, up);
}

void Camera::processMouseMovement(float xOffset, float yOffset, bool flat,
                                  bool slow, bool constrainPitch) {
    xOffset *= sensitivity * (slow ? 0.05f : 1.0f);
    yOffset *= sensitivity * (slow ? 0.05f : 1.0f);

    if (!flat) {
        yaw += xOffset;
        pitch -= yOffset;

        if (yaw < -180.0f)
            yaw += 360.0f;
        if (yaw > 180.0f)
            yaw -= 360.0f;

        const float limit = 89.9f;

        if (constrainPitch) {
            if (pitch > limit)
                pitch = limit;
            if (pitch < -limit)
                pitch = -limit;
        }
    } else {
        longitude += xOffset * 0.1f;
        latitude -= yOffset * 0.1f;

        if (longitude < 180.0f)
            longitude += 360.0f;
        if (longitude > 180.0f)
            longitude -= 360.0f;

        const float latitudeLimit = glm::degrees(std::tan(glm::radians(85.0f)));
        if (latitude < -latitudeLimit)
            latitude = -latitudeLimit;
        if (latitude > latitudeLimit)
            latitude = latitudeLimit;

        position.x = longitude;
        position.y = latitude;
    }

    updateCameraVectors(flat);
}

void Camera::processScroll(float, float yOffset, bool) {
    fov -= yOffset;

    if (fov > 180.0f)
        fov = 180.0f;

    if (fov < 0.1f)
        fov = 0.1f;
}

void Camera::processKeyboard(Key key, float, bool flat, bool fast) {
    if (!flat) {
        auto deltaPos = glm::vec3(0.0f);

        glm::vec3 otherFront = glm::cross(up, right);

        if (key == Key::W)
            deltaPos += otherFront * speed;
        if (key == Key::S)
            deltaPos -= otherFront * speed;
        if (key == Key::A)
            deltaPos -= right * speed;
        if (key == Key::D)
            deltaPos += right * speed;
        if (key == Key::SPACE)
            deltaPos += up * speed;
        if (key == Key::LEFT_CONTROL)
            deltaPos -= up * speed;

        position += deltaPos * (fast ? 30.0f : 1.0f);
    }

    updateCameraVectors(flat);
}

void Camera::updateCameraVectors(bool flat) {
    front = glm::normalize(
        glm::vec3(cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
                  sin(glm::radians(pitch)),
                  sin(glm::radians(yaw)) * cos(glm::radians(pitch))));

    if (flat) {
        up = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f));
    } else {
        up = glm::normalize(position);
    }
    right = glm::normalize(glm::cross(front, up));
}

void Camera::setMode(bool flat) {
    if (flat) {
        EarthGrid::cartesianToSpherical(position.x, position.y, position.z,
                                        longitude, latitude, altitude);
        longitude = glm::degrees(longitude);
        latitude = glm::degrees(latitude);
        latitude = glm::degrees(std::tan(glm::radians(latitude)));
        altitude -= EarthGrid::EARTH_RADIUS;
        position.x = longitude;
        position.y = latitude;
        position.z = 0.0f;
        yaw = -90.0f;
        pitch = 0.0f;
    } else {
        latitude = glm::degrees(std::atan(glm::radians(latitude)));
        EarthGrid::sphericalToCartesian(glm::radians(longitude),
                                        glm::radians(latitude),
                                        EarthGrid::EARTH_RADIUS + altitude,
                                        position.x, position.y, position.z);
    }
    updateCameraVectors(flat);
}