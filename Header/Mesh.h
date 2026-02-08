#pragma once
#include <GL/glew.h>

struct Mesh {
    unsigned int VAO, VBO, EBO;
    int indexCount;
};

// All meshes use vertex layout: position(3f) + normal(3f) + texcoord(2f)
// All meshes are unit-sized, centered at origin. Scale with model matrix.

Mesh createQuadMesh();       // 1x1 quad on XZ plane at Y=0, normal +Y
Mesh createBoxMesh();        // 1x1x1 cube, normals outward
Mesh createCylinderMesh(int segments = 16);  // radius 1, height 1, along Y
Mesh createSphereMesh(int rings = 12, int segments = 24); // radius 1
Mesh createConeMesh(int segments = 16);  // base radius 1, height 1, along Y
void deleteMesh(Mesh& mesh);
