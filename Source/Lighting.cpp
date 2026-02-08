#include "../Header/Lighting.h"
#include <glm/gtc/type_ptr.hpp>
#include <string>

int LightManager::AddFloorLight(int floorIndex) {
    PointLight light;
    light.position = glm::vec3(0.0f, floorIndex * FLOOR_HEIGHT + FLOOR_HEIGHT - 0.2f, -BUILDING_DEPTH / 2.0f + 2.0f);
    light.ambient = glm::vec3(0.12f, 0.12f, 0.1f);
    light.diffuse = glm::vec3(0.8f, 0.75f, 0.6f);
    light.specular = glm::vec3(0.5f, 0.5f, 0.4f);
    light.constant = 1.0f;
    light.linear = 0.09f;
    light.quadratic = 0.032f;
    light.active = true;
    lights.push_back(light);
    return (int)lights.size() - 1;
}

int LightManager::AddElevatorLight(float elevatorY) {
    PointLight light;
    light.position = glm::vec3(SHAFT_CENTER_X, elevatorY + ELEVATOR_HEIGHT - 0.2f, SHAFT_CENTER_Z);
    light.ambient = glm::vec3(0.15f, 0.15f, 0.12f);
    light.diffuse = glm::vec3(0.9f, 0.85f, 0.7f);
    light.specular = glm::vec3(0.6f, 0.6f, 0.5f);
    light.constant = 1.0f;
    light.linear = 0.14f;
    light.quadratic = 0.07f;
    light.active = true;
    lights.push_back(light);
    return (int)lights.size() - 1;
}

int LightManager::AddButtonGlow(glm::vec3 position) {
    PointLight light;
    light.position = position;
    light.ambient = glm::vec3(0.0f);
    light.diffuse = glm::vec3(0.6f, 0.4f, 0.1f);
    light.specular = glm::vec3(0.0f);
    light.constant = 1.0f;
    light.linear = 1.4f;
    light.quadratic = 3.6f;
    light.active = false;
    lights.push_back(light);
    return (int)lights.size() - 1;
}

void LightManager::UpdateLightPosition(int index, glm::vec3 newPos) {
    if (index >= 0 && index < (int)lights.size()) {
        lights[index].position = newPos;
    }
}

void LightManager::SetLightActive(int index, bool active) {
    if (index >= 0 && index < (int)lights.size()) {
        lights[index].active = active;
    }
}

void LightManager::UploadToShader(unsigned int shaderProgram) const {
    int count = (int)lights.size();
    if (count > MAX_LIGHTS) count = MAX_LIGHTS;

    glUniform1i(glGetUniformLocation(shaderProgram, "numLights"), count);

    for (int i = 0; i < count; i++) {
        std::string base = "lights[" + std::to_string(i) + "].";
        glUniform3fv(glGetUniformLocation(shaderProgram, (base + "position").c_str()), 1, glm::value_ptr(lights[i].position));
        glUniform3fv(glGetUniformLocation(shaderProgram, (base + "ambient").c_str()), 1, glm::value_ptr(lights[i].ambient));
        glUniform3fv(glGetUniformLocation(shaderProgram, (base + "diffuse").c_str()), 1, glm::value_ptr(lights[i].diffuse));
        glUniform3fv(glGetUniformLocation(shaderProgram, (base + "specular").c_str()), 1, glm::value_ptr(lights[i].specular));
        glUniform1f(glGetUniformLocation(shaderProgram, (base + "constant").c_str()), lights[i].constant);
        glUniform1f(glGetUniformLocation(shaderProgram, (base + "linear").c_str()), lights[i].linear);
        glUniform1f(glGetUniformLocation(shaderProgram, (base + "quadratic").c_str()), lights[i].quadratic);
        glUniform1i(glGetUniformLocation(shaderProgram, (base + "active").c_str()), lights[i].active ? 1 : 0);
    }
}
