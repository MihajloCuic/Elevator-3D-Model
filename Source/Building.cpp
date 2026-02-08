#include "../Header/Building.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void Building::setModelAndDraw(unsigned int shader, const Mesh& mesh, glm::mat4 model) const {
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(mesh.VAO);
    glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Building::setMaterial(unsigned int shader, glm::vec3 color) const {
    glUniform3fv(glGetUniformLocation(shader, "solidColor"), 1, glm::value_ptr(color));
    glUniform1i(glGetUniformLocation(shader, "useTexture"), 0);
}

// Helper: draw a wall as a thin box so both sides are visible
void Building::drawWall(unsigned int shader, const Mesh& box,
    glm::vec3 center, float width, float height, float depth) const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, center);
    model = glm::scale(model, glm::vec3(width, height, depth));
    setModelAndDraw(shader, box, model);
}

void Building::DrawFloors(unsigned int shader, const Mesh& quad, const Mesh& box) const {
    float halfW = BUILDING_WIDTH / 2.0f;
    float elevHalfW = ELEVATOR_WIDTH / 2.0f;
    float elevHalfD = ELEVATOR_DEPTH / 2.0f;
    float elevFrontZ = SHAFT_CENTER_Z + elevHalfD;
    float elevBackZ = SHAFT_CENTER_Z - elevHalfD;

    for (int i = 0; i < NUM_FLOORS; i++) {
        float baseY = i * FLOOR_HEIGHT;
        float midY = baseY + FLOOR_HEIGHT / 2.0f;

        // Floor slab - split around elevator shaft to avoid clipping
        // The shaft occupies X: [SHAFT_CENTER_X - elevHalfW, SHAFT_CENTER_X + elevHalfW]
        //                   Z: [SHAFT_CENTER_Z - elevHalfD, SHAFT_CENTER_Z + elevHalfD]
        setMaterial(shader, glm::vec3(0.45f, 0.4f, 0.35f));

        // Floor: left of shaft
        float leftFloorW = (halfW + (SHAFT_CENTER_X - elevHalfW));
        if (leftFloorW > 0.01f) {
            float cx = (-halfW + SHAFT_CENTER_X - elevHalfW) / 2.0f;
            drawWall(shader, box,
                glm::vec3(cx, baseY - 0.05f, -BUILDING_DEPTH / 2.0f),
                leftFloorW, 0.1f, BUILDING_DEPTH);
        }

        // Floor: right of shaft
        float rightFloorW = (halfW - (SHAFT_CENTER_X + elevHalfW));
        if (rightFloorW > 0.01f) {
            float cx = (SHAFT_CENTER_X + elevHalfW + halfW) / 2.0f;
            drawWall(shader, box,
                glm::vec3(cx, baseY - 0.05f, -BUILDING_DEPTH / 2.0f),
                rightFloorW, 0.1f, BUILDING_DEPTH);
        }

        // Floor: in front of shaft (between shaft front and building front)
        float frontFloorD = BUILDING_DEPTH - (BUILDING_DEPTH + (SHAFT_CENTER_Z - elevHalfD));
        // Actually: from z=0 to z=elevFrontZ is in front
        float frontZ_start = 0.0f;
        float frontZ_end = elevFrontZ;
        float frontDepth = frontZ_start - frontZ_end; // this would be negative; let's use proper coords
        // Hallway floor: from z = -BUILDING_DEPTH to z = elevBackZ (behind shaft) - skip
        // Hallway floor: from z = elevFrontZ to z = 0 (in front of shaft)
        float hallFrontD = -elevFrontZ; // distance from elevFrontZ to z=0
        if (hallFrontD > 0.01f) {
            float hallFrontCZ = (elevFrontZ + 0.0f) / 2.0f;
            drawWall(shader, box,
                glm::vec3(SHAFT_CENTER_X, baseY - 0.05f, hallFrontCZ),
                ELEVATOR_WIDTH, 0.1f, hallFrontD);
        }

        // Floor: behind shaft (from z = -BUILDING_DEPTH to z = elevBackZ)
        float hallBackD = (-elevBackZ) - BUILDING_DEPTH; // should be positive
        float behindShaftD = elevBackZ - (-BUILDING_DEPTH);
        if (behindShaftD > 0.01f) {
            float hallBackCZ = ((-BUILDING_DEPTH) + elevBackZ) / 2.0f;
            drawWall(shader, box,
                glm::vec3(SHAFT_CENTER_X, baseY - 0.05f, hallBackCZ),
                ELEVATOR_WIDTH, 0.1f, behindShaftD);
        }

        // Ceiling - same split pattern
        setMaterial(shader, glm::vec3(0.85f, 0.85f, 0.82f));

        // Ceiling: left of shaft
        if (leftFloorW > 0.01f) {
            float cx = (-halfW + SHAFT_CENTER_X - elevHalfW) / 2.0f;
            drawWall(shader, box,
                glm::vec3(cx, baseY + FLOOR_HEIGHT - 0.025f, -BUILDING_DEPTH / 2.0f),
                leftFloorW, 0.05f, BUILDING_DEPTH);
        }

        // Ceiling: right of shaft
        if (rightFloorW > 0.01f) {
            float cx = (SHAFT_CENTER_X + elevHalfW + halfW) / 2.0f;
            drawWall(shader, box,
                glm::vec3(cx, baseY + FLOOR_HEIGHT - 0.025f, -BUILDING_DEPTH / 2.0f),
                rightFloorW, 0.05f, BUILDING_DEPTH);
        }

        // Ceiling: in front of shaft
        if (hallFrontD > 0.01f) {
            float hallFrontCZ = (elevFrontZ + 0.0f) / 2.0f;
            drawWall(shader, box,
                glm::vec3(SHAFT_CENTER_X, baseY + FLOOR_HEIGHT - 0.025f, hallFrontCZ),
                ELEVATOR_WIDTH, 0.05f, hallFrontD);
        }

        // Ceiling: behind shaft
        if (behindShaftD > 0.01f) {
            float hallBackCZ = ((-BUILDING_DEPTH) + elevBackZ) / 2.0f;
            drawWall(shader, box,
                glm::vec3(SHAFT_CENTER_X, baseY + FLOOR_HEIGHT - 0.025f, hallBackCZ),
                ELEVATOR_WIDTH, 0.05f, behindShaftD);
        }

        // Back wall (z = -BUILDING_DEPTH) - thin box
        setMaterial(shader, glm::vec3(0.75f, 0.72f, 0.68f));
        drawWall(shader, box,
            glm::vec3(0.0f, midY, -BUILDING_DEPTH),
            BUILDING_WIDTH, FLOOR_HEIGHT, WALL_THICKNESS);

        // Front wall (z = 0) - closes the building from the front
        setMaterial(shader, glm::vec3(0.73f, 0.7f, 0.66f));
        drawWall(shader, box,
            glm::vec3(0.0f, midY, 0.0f),
            BUILDING_WIDTH, FLOOR_HEIGHT, WALL_THICKNESS);

        // Left wall (x = -halfW)
        setMaterial(shader, glm::vec3(0.72f, 0.68f, 0.62f));
        drawWall(shader, box,
            glm::vec3(-halfW, midY, -BUILDING_DEPTH / 2.0f),
            WALL_THICKNESS, FLOOR_HEIGHT, BUILDING_DEPTH);

        // Right wall (x = +halfW)
        setMaterial(shader, glm::vec3(0.72f, 0.68f, 0.62f));
        drawWall(shader, box,
            glm::vec3(halfW, midY, -BUILDING_DEPTH / 2.0f),
            WALL_THICKNESS, FLOOR_HEIGHT, BUILDING_DEPTH);

        // Front wall with elevator opening - the wall at z=0 that separates
        // the hallway from the elevator shaft
        // We draw the wall around the elevator opening
        setMaterial(shader, glm::vec3(0.7f, 0.7f, 0.65f));

        // Wall segment between elevator front and z=0 (the approach wall)
        // This is the wall where the elevator door opening is
        float wallZ = elevFrontZ;

        // Left of elevator opening
        float leftWallW = (halfW + (SHAFT_CENTER_X - elevHalfW));
        if (leftWallW > 0.01f) {
            float centerX = (-halfW + SHAFT_CENTER_X - elevHalfW) / 2.0f;
            drawWall(shader, box,
                glm::vec3(centerX, midY, wallZ),
                leftWallW, FLOOR_HEIGHT, WALL_THICKNESS);
        }

        // Right of elevator opening
        float rightWallW = (halfW - (SHAFT_CENTER_X + elevHalfW));
        if (rightWallW > 0.01f) {
            float centerX = (SHAFT_CENTER_X + elevHalfW + halfW) / 2.0f;
            drawWall(shader, box,
                glm::vec3(centerX, midY, wallZ),
                rightWallW, FLOOR_HEIGHT, WALL_THICKNESS);
        }

        // Above elevator door opening
        float aboveH = FLOOR_HEIGHT - DOOR_HEIGHT;
        if (aboveH > 0.01f) {
            drawWall(shader, box,
                glm::vec3(SHAFT_CENTER_X, baseY + DOOR_HEIGHT + aboveH / 2.0f, wallZ),
                ELEVATOR_WIDTH, aboveH, WALL_THICKNESS);
        }
    }
}

void Building::DrawElevatorShaft(unsigned int shader, const Mesh& box) const {
    float totalH = NUM_FLOORS * FLOOR_HEIGHT;
    float elevHalfW = ELEVATOR_WIDTH / 2.0f;
    float elevHalfD = ELEVATOR_DEPTH / 2.0f;
    float elevFrontZ = SHAFT_CENTER_Z + elevHalfD;

    setMaterial(shader, glm::vec3(0.5f, 0.5f, 0.5f));

    // Left shaft wall
    drawWall(shader, box,
        glm::vec3(SHAFT_CENTER_X - elevHalfW, totalH / 2.0f, SHAFT_CENTER_Z),
        WALL_THICKNESS, totalH, ELEVATOR_DEPTH);

    // Right shaft wall
    drawWall(shader, box,
        glm::vec3(SHAFT_CENTER_X + elevHalfW, totalH / 2.0f, SHAFT_CENTER_Z),
        WALL_THICKNESS, totalH, ELEVATOR_DEPTH);

    // Back shaft wall
    drawWall(shader, box,
        glm::vec3(SHAFT_CENTER_X, totalH / 2.0f, SHAFT_CENTER_Z - elevHalfD),
        ELEVATOR_WIDTH, totalH, WALL_THICKNESS);

    // Front shaft wall - closes the shaft from the hallway side
    // Draw a full wall with door-sized openings on each floor
    setMaterial(shader, glm::vec3(0.55f, 0.55f, 0.52f));
    for (int i = 0; i < NUM_FLOORS; i++) {
        float baseY = i * FLOOR_HEIGHT;

        // Left of door opening
        float leftW = elevHalfW - DOOR_WIDTH;
        if (leftW > 0.01f) {
            drawWall(shader, box,
                glm::vec3(SHAFT_CENTER_X - DOOR_WIDTH - leftW / 2.0f,
                           baseY + DOOR_HEIGHT / 2.0f, elevFrontZ),
                leftW, DOOR_HEIGHT, WALL_THICKNESS);
        }

        // Right of door opening
        float rightW = elevHalfW - DOOR_WIDTH;
        if (rightW > 0.01f) {
            drawWall(shader, box,
                glm::vec3(SHAFT_CENTER_X + DOOR_WIDTH + rightW / 2.0f,
                           baseY + DOOR_HEIGHT / 2.0f, elevFrontZ),
                rightW, DOOR_HEIGHT, WALL_THICKNESS);
        }

        // Above door opening
        float aboveH = FLOOR_HEIGHT - DOOR_HEIGHT;
        if (aboveH > 0.01f) {
            drawWall(shader, box,
                glm::vec3(SHAFT_CENTER_X, baseY + DOOR_HEIGHT + aboveH / 2.0f, elevFrontZ),
                ELEVATOR_WIDTH, aboveH, WALL_THICKNESS);
        }
    }
}

void Building::DrawElevatorCab(unsigned int shader, float elevatorY, float doorOpenAmount,
                                const Mesh& box, const Mesh& quad) const {
    float elevHalfW = ELEVATOR_WIDTH / 2.0f;
    float elevHalfD = ELEVATOR_DEPTH / 2.0f;
    float elevFrontZ = SHAFT_CENTER_Z + elevHalfD;
    float midY = elevatorY + ELEVATOR_HEIGHT / 2.0f;

    // Elevator floor
    setMaterial(shader, glm::vec3(0.35f, 0.3f, 0.25f));
    drawWall(shader, box,
        glm::vec3(SHAFT_CENTER_X, elevatorY + 0.02f, SHAFT_CENTER_Z),
        ELEVATOR_WIDTH - 0.02f, 0.04f, ELEVATOR_DEPTH - 0.02f);

    // Elevator ceiling
    setMaterial(shader, glm::vec3(0.82f, 0.82f, 0.78f));
    drawWall(shader, box,
        glm::vec3(SHAFT_CENTER_X, elevatorY + ELEVATOR_HEIGHT - 0.02f, SHAFT_CENTER_Z),
        ELEVATOR_WIDTH - 0.02f, 0.04f, ELEVATOR_DEPTH - 0.02f);

    // Elevator back wall (inside)
    setMaterial(shader, glm::vec3(0.6f, 0.58f, 0.55f));
    drawWall(shader, box,
        glm::vec3(SHAFT_CENTER_X, midY, SHAFT_CENTER_Z - elevHalfD + 0.05f),
        ELEVATOR_WIDTH - 0.04f, ELEVATOR_HEIGHT - 0.04f, 0.05f);

    // Elevator left wall (inside)
    setMaterial(shader, glm::vec3(0.62f, 0.6f, 0.57f));
    drawWall(shader, box,
        glm::vec3(SHAFT_CENTER_X - elevHalfW + 0.05f, midY, SHAFT_CENTER_Z),
        0.05f, ELEVATOR_HEIGHT - 0.04f, ELEVATOR_DEPTH - 0.04f);

    // Elevator right wall (inside) - button panel goes here
    setMaterial(shader, glm::vec3(0.62f, 0.6f, 0.57f));
    drawWall(shader, box,
        glm::vec3(SHAFT_CENTER_X + elevHalfW - 0.05f, midY, SHAFT_CENTER_Z),
        0.05f, ELEVATOR_HEIGHT - 0.04f, ELEVATOR_DEPTH - 0.04f);

    // Doors - two fixed-width panels that slide left/right into the walls
    setMaterial(shader, glm::vec3(0.6f, 0.62f, 0.65f));
    float doorSlide = doorOpenAmount * DOOR_WIDTH; // how far each door has slid

    // Left door panel - slides left (into left wall)
    // When closed: centered at SHAFT_CENTER_X - DOOR_WIDTH/2
    // When open: slid left by doorSlide
    {
        float leftDoorCenterX = SHAFT_CENTER_X - DOOR_WIDTH / 2.0f - doorSlide;
        drawWall(shader, box,
            glm::vec3(leftDoorCenterX, elevatorY + DOOR_HEIGHT / 2.0f, elevFrontZ),
            DOOR_WIDTH, DOOR_HEIGHT, 0.06f);
    }

    // Right door panel - slides right (into right wall)
    // When closed: centered at SHAFT_CENTER_X + DOOR_WIDTH/2
    // When open: slid right by doorSlide
    {
        float rightDoorCenterX = SHAFT_CENTER_X + DOOR_WIDTH / 2.0f + doorSlide;
        drawWall(shader, box,
            glm::vec3(rightDoorCenterX, elevatorY + DOOR_HEIGHT / 2.0f, elevFrontZ),
            DOOR_WIDTH, DOOR_HEIGHT, 0.06f);
    }
}

void Building::DrawLightFixtures(unsigned int shader, float elevatorY,
                                  const Mesh& cylinder, const Mesh& sphere, const Mesh& cone) const {
    for (int i = 0; i < NUM_FLOORS; i++) {
        float baseY = i * FLOOR_HEIGHT;
        float fixtureY = baseY + FLOOR_HEIGHT - 0.05f;
        float lightZ = -BUILDING_DEPTH / 2.0f;

        // Mounting rod
        setMaterial(shader, glm::vec3(0.3f, 0.3f, 0.3f));
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, fixtureY - 0.1f, lightZ));
            model = glm::scale(model, glm::vec3(0.04f, 0.25f, 0.04f));
            setModelAndDraw(shader, cylinder, model);
        }

        // Shade (inverted cone)
        setMaterial(shader, glm::vec3(0.9f, 0.85f, 0.7f));
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, fixtureY - 0.3f, lightZ));
            model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(0.3f, 0.18f, 0.3f));
            setModelAndDraw(shader, cone, model);
        }
    }

    // Elevator light fixture
    {
        float fixtureY = elevatorY + ELEVATOR_HEIGHT - 0.05f;

        setMaterial(shader, glm::vec3(0.3f, 0.3f, 0.3f));
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(SHAFT_CENTER_X, fixtureY - 0.08f, SHAFT_CENTER_Z));
            model = glm::scale(model, glm::vec3(0.03f, 0.18f, 0.03f));
            setModelAndDraw(shader, cylinder, model);
        }

        setMaterial(shader, glm::vec3(0.9f, 0.85f, 0.7f));
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(SHAFT_CENTER_X, fixtureY - 0.22f, SHAFT_CENTER_Z));
            model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(0.22f, 0.14f, 0.22f));
            setModelAndDraw(shader, cone, model);
        }
    }
}

void Building::DrawPlants(unsigned int shader,
                           const Mesh& cylinder, const Mesh& sphere, const Mesh& cone) const {
    struct PlantInfo {
        int floor;
        int type;
        glm::vec3 pos;
    };

    PlantInfo plants[] = {
        {0, 0, glm::vec3( 3.5f, 0.0f, -2.5f)},
        {1, 1, glm::vec3(-3.5f, 0.0f, -3.5f)},
        {2, 2, glm::vec3( 4.0f, 0.0f, -6.0f)},
        {3, 0, glm::vec3(-4.0f, 0.0f, -3.0f)},
        {4, 1, glm::vec3( 3.5f, 0.0f, -5.0f)},
        {5, 2, glm::vec3(-3.5f, 0.0f, -6.5f)},
        {6, 0, glm::vec3( 4.0f, 0.0f, -4.0f)},
        {7, 1, glm::vec3(-4.0f, 0.0f, -5.5f)},
    };

    for (const auto& p : plants) {
        float baseY = p.floor * FLOOR_HEIGHT;
        glm::vec3 potPos = p.pos + glm::vec3(0.0f, baseY, 0.0f);

        setMaterial(shader, glm::vec3(0.55f, 0.32f, 0.15f));

        if (p.type == 0) {
            // Type A: tall conical
            {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, potPos + glm::vec3(0.0f, 0.2f, 0.0f));
                model = glm::scale(model, glm::vec3(0.32f, 0.4f, 0.32f));
                setModelAndDraw(shader, cylinder, model);
            }
            setMaterial(shader, glm::vec3(0.1f, 0.5f, 0.15f));
            {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, potPos + glm::vec3(0.0f, 0.95f, 0.0f));
                model = glm::scale(model, glm::vec3(0.55f, 1.1f, 0.55f));
                setModelAndDraw(shader, cone, model);
            }
        } else if (p.type == 1) {
            // Type B: round bush
            {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, potPos + glm::vec3(0.0f, 0.15f, 0.0f));
                model = glm::scale(model, glm::vec3(0.28f, 0.3f, 0.28f));
                setModelAndDraw(shader, cylinder, model);
            }
            setMaterial(shader, glm::vec3(0.12f, 0.55f, 0.1f));
            {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, potPos + glm::vec3(0.0f, 0.85f, 0.0f));
                model = glm::scale(model, glm::vec3(0.65f, 0.65f, 0.65f));
                setModelAndDraw(shader, sphere, model);
            }
        } else {
            // Type C: wide flat
            {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, potPos + glm::vec3(0.0f, 0.18f, 0.0f));
                model = glm::scale(model, glm::vec3(0.38f, 0.36f, 0.38f));
                setModelAndDraw(shader, cylinder, model);
            }
            setMaterial(shader, glm::vec3(0.18f, 0.58f, 0.18f));
            {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, potPos + glm::vec3(0.0f, 0.48f, 0.0f));
                model = glm::scale(model, glm::vec3(0.75f, 0.16f, 0.75f));
                setModelAndDraw(shader, cylinder, model);
            }
            setMaterial(shader, glm::vec3(0.14f, 0.48f, 0.1f));
            {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, potPos + glm::vec3(0.0f, 0.7f, 0.0f));
                model = glm::scale(model, glm::vec3(0.32f, 0.32f, 0.32f));
                setModelAndDraw(shader, cone, model);
            }
        }
    }
}

// Draw floor number display above each elevator opening
void Building::DrawFloorNumbers(unsigned int shader, const Mesh& box, unsigned int* floorTextures) const {
    // We'll draw colored indicators next to elevator doors on each floor
    // This is handled via texture in Main.cpp
}
