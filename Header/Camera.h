#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float Yaw;
    float Pitch;
    float MovementSpeed;
    float MouseSensitivity;

    Camera(glm::vec3 position = glm::vec3(0.0f), float yaw = -90.0f, float pitch = 0.0f);

    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix(float aspectRatio) const;

    // Move on XZ plane only (Y is controlled externally)
    void ProcessKeyboard(int direction, float deltaTime);
    void ProcessMouseMovement(float xoffset, float yoffset);

    // Get normalized direction from camera center (for raycasting)
    glm::vec3 GetFrontDirection() const { return Front; }

private:
    void updateCameraVectors();
};

// Direction constants for ProcessKeyboard
enum CameraMovement {
    CAM_FORWARD = 0,
    CAM_BACKWARD,
    CAM_LEFT,
    CAM_RIGHT
};
