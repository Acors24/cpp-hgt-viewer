#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class Key {
    W,
    S,
    A,
    D,
    SPACE,
    LEFT_CONTROL,
    TAB,
};

class Camera {
public:
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 up;
    glm::vec3 front;
    glm::vec3 right;
    float yaw = -90.0f;
    float pitch = 0.0f;
    float speed = 0.2f;
    float sensitivity = 0.05f;
    float fov = 90.0f;

    float longitude = 0.0f;
    float latitude = 0.0f;
    float altitude = 0.0f;

    Camera();
    explicit Camera(std::tuple<float, float, float> position);

    glm::mat4 getViewMatrix() const;

    void processMouseMovement(float xoffset, float yoffset, bool flat,
                              bool slow, bool constrainPitch);

    void processScroll(float xoffset, float yoffset, bool flat);

    void processKeyboard(Key key, float deltaTime, bool flat, bool fast);

    void updateCameraVectors(bool flat);

    void setMode(bool flat);
};

#endif
