#include "../Header/Camera.h"
#include "../Header/Constants.h"

Camera::Camera(glm::vec3 position, float yaw, float pitch)
    : Position(position), Yaw(yaw), Pitch(pitch),
      WorldUp(0.0f, 1.0f, 0.0f),
      MovementSpeed(PLAYER_SPEED),
      MouseSensitivity(MOUSE_SENSITIVITY)
{
    updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() const {
    return glm::lookAt(Position, Position + Front, Up);
}

glm::mat4 Camera::GetProjectionMatrix(float aspectRatio) const {
    return glm::perspective(glm::radians(CAMERA_FOV), aspectRatio, CAMERA_NEAR, CAMERA_FAR);
}

void Camera::ProcessKeyboard(int direction, float deltaTime) {
    float velocity = MovementSpeed * deltaTime;
    // Project front onto XZ plane for ground movement
    glm::vec3 frontXZ = glm::normalize(glm::vec3(Front.x, 0.0f, Front.z));
    glm::vec3 rightXZ = glm::normalize(glm::cross(frontXZ, WorldUp));

    switch (direction) {
        case CAM_FORWARD:  Position += frontXZ * velocity; break;
        case CAM_BACKWARD: Position -= frontXZ * velocity; break;
        case CAM_LEFT:     Position -= rightXZ * velocity; break;
        case CAM_RIGHT:    Position += rightXZ * velocity; break;
    }
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset) {
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    if (Pitch > 89.0f) Pitch = 89.0f;
    if (Pitch < -89.0f) Pitch = -89.0f;

    updateCameraVectors();
}

void Camera::updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
}
