#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include "Constants.h"

struct PointLight {
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float constant;
    float linear;
    float quadratic;
    bool active;
};

class LightManager {
public:
    std::vector<PointLight> lights;

    // Returns light index
    int AddFloorLight(int floorIndex);
    int AddElevatorLight(float elevatorY);
    int AddButtonGlow(glm::vec3 position);

    void UpdateLightPosition(int index, glm::vec3 newPos);
    void SetLightActive(int index, bool active);

    void UploadToShader(unsigned int shaderProgram) const;
};
