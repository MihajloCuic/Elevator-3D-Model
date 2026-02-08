#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include "Constants.h"
#include "Mesh.h"

struct Button3D {
    glm::vec3 center;     // world position
    glm::vec3 normal;     // face direction
    float halfW, halfH;
    int type;             // 0=floor, 1=close, 2=open, 3=stop, 4=ventilation
    int floorIndex;       // only for type 0
    bool active;
    int glowLightIdx;     // index in LightManager, -1 if none
    glm::vec3 activeColor;
    glm::vec3 inactiveColor;
};

class ButtonPanel {
public:
    std::vector<Button3D> buttons;

    void Init();
    void UpdatePositions(float elevatorY);

    // Returns button index or -1
    int Raycast(glm::vec3 rayOrigin, glm::vec3 rayDir, float maxDist = 3.0f) const;

    void Draw(unsigned int shader, const Mesh& box, unsigned int* btnTextures) const;

private:
    // Local offsets from elevator center, computed once in Init
    struct ButtonOffset {
        float dx, dy, dz;
    };
    std::vector<ButtonOffset> offsets;
};
