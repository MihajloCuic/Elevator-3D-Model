#include "../Header/Mesh.h"
#include <vector>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static Mesh buildMesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices) {
    Mesh mesh;
    mesh.indexCount = (int)indices.size();

    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO);
    glGenBuffers(1, &mesh.EBO);

    glBindVertexArray(mesh.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // position (location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal (location 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texcoord (location 2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    return mesh;
}

void deleteMesh(Mesh& mesh) {
    glDeleteVertexArrays(1, &mesh.VAO);
    glDeleteBuffers(1, &mesh.VBO);
    glDeleteBuffers(1, &mesh.EBO);
    mesh.VAO = mesh.VBO = mesh.EBO = 0;
    mesh.indexCount = 0;
}

Mesh createQuadMesh() {
    // 1x1 quad on XZ plane, centered at origin, normal pointing +Y
    std::vector<float> vertices = {
        // pos                normal          texcoord
        -0.5f, 0.0f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
         0.5f, 0.0f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
         0.5f, 0.0f,  0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
        -0.5f, 0.0f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
    };
    std::vector<unsigned int> indices = { 0, 1, 2, 0, 2, 3 };
    return buildMesh(vertices, indices);
}

Mesh createBoxMesh() {
    // Unit cube from (-0.5,-0.5,-0.5) to (0.5,0.5,0.5)
    // 24 vertices (4 per face), 36 indices
    std::vector<float> vertices = {
        // Front face (z = +0.5, normal +Z)
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
        // Back face (z = -0.5, normal -Z)
         0.5f, -0.5f, -0.5f,  0.0f, 0.0f,-1.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,-1.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f,-1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 0.0f,-1.0f,  0.0f, 1.0f,
        // Top face (y = +0.5, normal +Y)
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
        // Bottom face (y = -0.5, normal -Y)
        -0.5f, -0.5f, -0.5f,  0.0f,-1.0f, 0.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,-1.0f, 0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,-1.0f, 0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,-1.0f, 0.0f,  0.0f, 1.0f,
        // Right face (x = +0.5, normal +X)
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
        // Left face (x = -0.5, normal -X)
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, -1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
    };
    std::vector<unsigned int> indices;
    for (int face = 0; face < 6; face++) {
        unsigned int base = face * 4;
        indices.push_back(base + 0);
        indices.push_back(base + 1);
        indices.push_back(base + 2);
        indices.push_back(base + 0);
        indices.push_back(base + 2);
        indices.push_back(base + 3);
    }
    return buildMesh(vertices, indices);
}

Mesh createCylinderMesh(int segments) {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    // Side vertices
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * (float)M_PI * i / segments;
        float x = cos(angle);
        float z = sin(angle);
        float u = (float)i / segments;

        // Bottom vertex
        vertices.insert(vertices.end(), { x, -0.5f, z,  x, 0.0f, z,  u, 0.0f });
        // Top vertex
        vertices.insert(vertices.end(), { x,  0.5f, z,  x, 0.0f, z,  u, 1.0f });
    }

    // Side indices
    for (int i = 0; i < segments; i++) {
        unsigned int b = i * 2;
        indices.insert(indices.end(), { b, b + 1, b + 3, b, b + 3, b + 2 });
    }

    // Top cap
    unsigned int topCenter = (unsigned int)(vertices.size() / 8);
    vertices.insert(vertices.end(), { 0.0f, 0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  0.5f, 0.5f });
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * (float)M_PI * i / segments;
        float x = cos(angle);
        float z = sin(angle);
        vertices.insert(vertices.end(), { x, 0.5f, z,  0.0f, 1.0f, 0.0f,  0.5f + 0.5f * x, 0.5f + 0.5f * z });
    }
    for (int i = 0; i < segments; i++) {
        indices.insert(indices.end(), { topCenter, topCenter + 1 + i, topCenter + 2 + i });
    }

    // Bottom cap
    unsigned int botCenter = (unsigned int)(vertices.size() / 8);
    vertices.insert(vertices.end(), { 0.0f, -0.5f, 0.0f,  0.0f, -1.0f, 0.0f,  0.5f, 0.5f });
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * (float)M_PI * i / segments;
        float x = cos(angle);
        float z = sin(angle);
        vertices.insert(vertices.end(), { x, -0.5f, z,  0.0f, -1.0f, 0.0f,  0.5f + 0.5f * x, 0.5f + 0.5f * z });
    }
    for (int i = 0; i < segments; i++) {
        indices.insert(indices.end(), { botCenter, botCenter + 2 + i, botCenter + 1 + i });
    }

    return buildMesh(vertices, indices);
}

Mesh createSphereMesh(int rings, int segments) {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    for (int y = 0; y <= rings; y++) {
        for (int x = 0; x <= segments; x++) {
            float xSeg = (float)x / segments;
            float ySeg = (float)y / rings;
            float xPos = cos(xSeg * 2.0f * (float)M_PI) * sin(ySeg * (float)M_PI);
            float yPos = cos(ySeg * (float)M_PI);
            float zPos = sin(xSeg * 2.0f * (float)M_PI) * sin(ySeg * (float)M_PI);

            vertices.insert(vertices.end(), { xPos, yPos, zPos,  xPos, yPos, zPos,  xSeg, ySeg });
        }
    }

    for (int y = 0; y < rings; y++) {
        for (int x = 0; x < segments; x++) {
            unsigned int i0 = y * (segments + 1) + x;
            unsigned int i1 = i0 + 1;
            unsigned int i2 = i0 + (segments + 1);
            unsigned int i3 = i2 + 1;
            indices.insert(indices.end(), { i0, i2, i1, i1, i2, i3 });
        }
    }

    return buildMesh(vertices, indices);
}

Mesh createConeMesh(int segments) {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    // Apex
    unsigned int apex = 0;
    vertices.insert(vertices.end(), { 0.0f, 0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  0.5f, 1.0f });

    // Side vertices (base ring)
    float slopeY = 1.0f / sqrtf(2.0f); // normalized slope
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * (float)M_PI * i / segments;
        float x = cos(angle);
        float z = sin(angle);
        // Normal for cone side: outward + slightly up
        float nx = x * slopeY;
        float nz = z * slopeY;
        float ny = slopeY;
        vertices.insert(vertices.end(), { x, -0.5f, z,  nx, ny, nz,  (float)i / segments, 0.0f });
    }

    // Side triangles
    for (int i = 0; i < segments; i++) {
        indices.insert(indices.end(), { apex, 1 + (unsigned int)i, 2 + (unsigned int)i });
    }

    // Bottom cap
    unsigned int botCenter = (unsigned int)(vertices.size() / 8);
    vertices.insert(vertices.end(), { 0.0f, -0.5f, 0.0f,  0.0f, -1.0f, 0.0f,  0.5f, 0.5f });
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * (float)M_PI * i / segments;
        float x = cos(angle);
        float z = sin(angle);
        vertices.insert(vertices.end(), { x, -0.5f, z,  0.0f, -1.0f, 0.0f,  0.5f + 0.5f * x, 0.5f + 0.5f * z });
    }
    for (int i = 0; i < segments; i++) {
        indices.insert(indices.end(), { botCenter, botCenter + 2 + (unsigned int)i, botCenter + 1 + (unsigned int)i });
    }

    return buildMesh(vertices, indices);
}
