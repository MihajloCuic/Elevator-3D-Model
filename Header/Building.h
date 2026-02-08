#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "Constants.h"
#include "Mesh.h"

class Building {
public:
    void DrawFloors(unsigned int shader, const Mesh& quad, const Mesh& box) const;
    void DrawElevatorShaft(unsigned int shader, const Mesh& box) const;
    void DrawElevatorCab(unsigned int shader, float elevatorY, float doorOpenAmount,
                         const Mesh& box, const Mesh& quad) const;
    void DrawLightFixtures(unsigned int shader, float elevatorY,
                           const Mesh& cylinder, const Mesh& sphere, const Mesh& cone) const;
    void DrawPlants(unsigned int shader,
                    const Mesh& cylinder, const Mesh& sphere, const Mesh& cone) const;
    void DrawFloorNumbers(unsigned int shader, const Mesh& box, unsigned int* floorTextures) const;

private:
    void setModelAndDraw(unsigned int shader, const Mesh& mesh, glm::mat4 model) const;
    void setMaterial(unsigned int shader, glm::vec3 color) const;
    void drawWall(unsigned int shader, const Mesh& box,
        glm::vec3 center, float width, float height, float depth) const;
};
