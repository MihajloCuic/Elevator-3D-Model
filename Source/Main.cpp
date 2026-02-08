#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>

#include "../Header/Util.h"
#include "../Header/Constants.h"
#include "../Header/Camera.h"
#include "../Header/Mesh.h"
#include "../Header/Elevator.h"
#include "../Header/Lighting.h"
#include "../Header/Building.h"
#include "../Header/ButtonPanel.h"

// ============ GLOBALS ============
Camera camera(glm::vec3(0.0f, FLOOR_HEIGHT + PLAYER_HEIGHT, -3.0f), -90.0f, 0.0f);
Elevator elevator;
Building building;
ButtonPanel buttonPanel;
LightManager lightManager;

bool playerInElevator = false;
int playerFloor = 1; // Start at PR (ground floor)

float deltaTime = 0.0f;
float lastX = 0.0f, lastY = 0.0f;
bool firstMouse = true;

bool depthTestEnabled = true;
bool cullingEnabled = true;

bool keys[1024] = { false };
int elevatorLightIdx = -1;

int screenWidth = 0, screenHeight = 0;

// Textures
unsigned int btnTextures[12] = { 0 };
unsigned int floorTextures[8] = { 0 };
unsigned int studentInfoTex = 0;

// ============ CALLBACKS ============
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
        depthTestEnabled = !depthTestEnabled;

    if (key == GLFW_KEY_F2 && action == GLFW_PRESS)
        cullingEnabled = !cullingEnabled;

    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) keys[key] = true;
        else if (action == GLFW_RELEASE) keys[key] = false;
    }
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = (float)xposIn;
    float ypos = (float)yposIn;

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        if (playerInElevator) {
            int hitBtn = buttonPanel.Raycast(camera.Position, camera.Front, 3.0f);
            if (hitBtn >= 0) {
                Button3D& btn = buttonPanel.buttons[hitBtn];
                if (btn.type == 0) {
                    elevator.RequestFloor(btn.floorIndex);
                } else if (btn.type == 1) {
                    elevator.CloseDoors();
                } else if (btn.type == 2) {
                    elevator.OpenDoors();
                } else if (btn.type == 3) {
                    elevator.ToggleStop();
                } else if (btn.type == 4) {
                    elevator.ToggleVentilation();
                }
            }
        }
    }
}

// ============ TEXTURE LOADING ============
unsigned int loadAndSetupTexture(const char* path) {
    unsigned int tex = loadImageToTexture(path);
    if (tex != 0) {
        glBindTexture(GL_TEXTURE_2D, tex);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    return tex;
}

// ============ PLAYER MOVEMENT ============
void processPlayerMovement() {
    if (keys[GLFW_KEY_W]) camera.ProcessKeyboard(CAM_FORWARD, deltaTime);
    if (keys[GLFW_KEY_S]) camera.ProcessKeyboard(CAM_BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A]) camera.ProcessKeyboard(CAM_LEFT, deltaTime);
    if (keys[GLFW_KEY_D]) camera.ProcessKeyboard(CAM_RIGHT, deltaTime);

    float halfW = BUILDING_WIDTH / 2.0f - 0.3f;
    float elevHalfW = ELEVATOR_WIDTH / 2.0f;
    float elevHalfD = ELEVATOR_DEPTH / 2.0f;
    float elevFrontZ = SHAFT_CENTER_Z + elevHalfD;

    if (!playerInElevator) {
        // Constrain to building floor area
        camera.Position.x = glm::clamp(camera.Position.x, -halfW, halfW);
        camera.Position.z = glm::clamp(camera.Position.z, -BUILDING_DEPTH + 0.3f, -0.3f);

        // Check if near the elevator shaft area
        bool nearShaftX = camera.Position.x > SHAFT_CENTER_X - elevHalfW - 0.15f &&
                          camera.Position.x < SHAFT_CENTER_X + elevHalfW + 0.15f;
        bool nearShaftZ = camera.Position.z < elevFrontZ + 0.15f;

        if (nearShaftX && nearShaftZ) {
            if (elevator.AreDoorsOpen() && elevator.IsAtFloor(playerFloor)) {
                // Doors open and elevator is here - check if player walked inside
                bool insideElev =
                    camera.Position.x > SHAFT_CENTER_X - elevHalfW + 0.25f &&
                    camera.Position.x < SHAFT_CENTER_X + elevHalfW - 0.25f &&
                    camera.Position.z < elevFrontZ - 0.1f &&
                    camera.Position.z > SHAFT_CENTER_Z - elevHalfD + 0.25f;
                if (insideElev) {
                    playerInElevator = true;
                }
            } else {
                // Doors closed or elevator not here - block entry
                camera.Position.z = elevFrontZ + 0.15f;
            }
        }

        // Call elevator with C key
        if (keys[GLFW_KEY_C]) {
            elevator.CallToFloor(playerFloor);
            keys[GLFW_KEY_C] = false;
        }

        camera.Position.y = playerFloor * FLOOR_HEIGHT + PLAYER_HEIGHT;
    } else {
        // Inside elevator: constrain to cab bounds
        camera.Position.x = glm::clamp(camera.Position.x,
            SHAFT_CENTER_X - elevHalfW + 0.3f,
            SHAFT_CENTER_X + elevHalfW - 0.3f);

        if (elevator.AreDoorsOpen()) {
            // Allow moving toward door to exit
            camera.Position.z = glm::clamp(camera.Position.z,
                SHAFT_CENTER_Z - elevHalfD + 0.3f,
                elevFrontZ + 0.1f);

            // Exit when past door threshold
            if (camera.Position.z > elevFrontZ - 0.05f) {
                playerInElevator = false;
                playerFloor = elevator.currentFloor;
                camera.Position.z = elevFrontZ + 0.3f;
            }
        } else {
            // Doors closed: stay inside
            camera.Position.z = glm::clamp(camera.Position.z,
                SHAFT_CENTER_Z - elevHalfD + 0.3f,
                elevFrontZ - 0.15f);
        }

        // Follow elevator Y
        camera.Position.y = elevator.currentY + PLAYER_HEIGHT;
    }
}

// ============ DRAW TEXTURED QUAD 3D HELPER ============
void drawTexturedQuad3D(unsigned int shader, const Mesh& box, unsigned int tex,
                         glm::vec3 pos, glm::vec3 scale) {
    glUniform1i(glGetUniformLocation(shader, "useTexture"), 1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glUniform1i(glGetUniformLocation(shader, "diffuseTexture"), 0);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, pos);
    model = glm::scale(model, scale);
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(box.VAO);
    glDrawElements(GL_TRIANGLES, box.indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUniform1i(glGetUniformLocation(shader, "useTexture"), 0);
}

// ============ MAIN ============
int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Fullscreen
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height,
        "Elevator 3D - Mihajlo Cuic SV43/2022", monitor, NULL);
    if (window == NULL) return endProgram("Prozor nije uspeo da se kreira.");

    glfwMakeContextCurrent(window);
    screenWidth = mode->width;
    screenHeight = mode->height;

    if (glewInit() != GLEW_OK) return endProgram("GLEW nije uspeo da se inicijalizuje.");

    // Callbacks
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // OpenGL state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Load shaders
    unsigned int basicShader = createShader("Shaders/basic.vert", "Shaders/basic.frag");
    unsigned int hudShader = createShader("Shaders/hud.vert", "Shaders/hud.frag");

    // Generate meshes
    Mesh quadMesh = createQuadMesh();
    Mesh boxMesh = createBoxMesh();
    Mesh cylinderMesh = createCylinderMesh(16);
    Mesh sphereMesh = createSphereMesh(12, 24);
    Mesh coneMesh = createConeMesh(16);

    // Load textures
    studentInfoTex = loadAndSetupTexture("Resources/student_info.png");
    for (int i = 0; i < 12; i++) {
        std::string path = "Resources/btn_" + std::to_string(i) + ".png";
        btnTextures[i] = loadAndSetupTexture(path.c_str());
    }
    for (int i = 0; i < 8; i++) {
        std::string path = "Resources/floor_" + std::to_string(i) + ".png";
        floorTextures[i] = loadAndSetupTexture(path.c_str());
    }

    // Initialize button panel
    buttonPanel.Init();

    // Setup lights
    for (int i = 0; i < NUM_FLOORS; i++) {
        lightManager.AddFloorLight(i);
    }
    elevatorLightIdx = lightManager.AddElevatorLight(elevator.currentY);

    // Set default material properties
    glUseProgram(basicShader);
    glUniform3f(glGetUniformLocation(basicShader, "materialSpecular"), 0.3f, 0.3f, 0.3f);
    glUniform1f(glGetUniformLocation(basicShader, "materialShininess"), 32.0f);
    glUniform1f(glGetUniformLocation(basicShader, "alpha"), 1.0f);
    glUniform3f(glGetUniformLocation(basicShader, "emissiveColor"), 0.0f, 0.0f, 0.0f);
    glUniform1f(glGetUniformLocation(basicShader, "emissiveStrength"), 0.0f);

    lastX = screenWidth / 2.0f;
    lastY = screenHeight / 2.0f;

    // ============ RENDER LOOP ============
    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        double currentTime = glfwGetTime();
        deltaTime = (float)(currentTime - lastTime);

        // Frame limiter at 75 FPS - MUST poll events even when skipping frame
        if (deltaTime < TARGET_FRAME_TIME) {
            glfwPollEvents();
            continue;
        }
        lastTime = currentTime;

        glfwPollEvents();

        // --- INPUT & MOVEMENT ---
        processPlayerMovement();

        // --- UPDATE ---
        elevator.Update(deltaTime);
        buttonPanel.UpdatePositions(elevator.currentY);
        lightManager.UpdateLightPosition(elevatorLightIdx,
            glm::vec3(SHAFT_CENTER_X, elevator.currentY + ELEVATOR_HEIGHT - 0.2f, SHAFT_CENTER_Z));

        // Update button active states
        for (size_t i = 0; i < buttonPanel.buttons.size(); i++) {
            Button3D& btn = buttonPanel.buttons[i];
            if (btn.type == 0) {
                btn.active = elevator.floorRequests[btn.floorIndex];
            } else if (btn.type == 3) {
                btn.active = elevator.stopped;
            } else if (btn.type == 4) {
                btn.active = elevator.ventilationOn;
            } else {
                btn.active = false;
            }
        }

        // --- RENDER ---
        glClearColor(0.02f, 0.02f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (depthTestEnabled) glEnable(GL_DEPTH_TEST);
        else glDisable(GL_DEPTH_TEST);

        if (cullingEnabled) glEnable(GL_CULL_FACE);
        else glDisable(GL_CULL_FACE);

        float aspect = (float)screenWidth / (float)screenHeight;
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = camera.GetProjectionMatrix(aspect);

        glUseProgram(basicShader);
        glUniformMatrix4fv(glGetUniformLocation(basicShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(basicShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3fv(glGetUniformLocation(basicShader, "viewPos"), 1, glm::value_ptr(camera.Position));
        glUniform1f(glGetUniformLocation(basicShader, "alpha"), 1.0f);

        // Upload lights
        lightManager.UploadToShader(basicShader);

        // Draw building (using boxMesh for walls)
        building.DrawFloors(basicShader, quadMesh, boxMesh);
        building.DrawElevatorShaft(basicShader, boxMesh);
        building.DrawElevatorCab(basicShader, elevator.currentY, elevator.doorOpenAmount, boxMesh, quadMesh);
        building.DrawLightFixtures(basicShader, elevator.currentY, cylinderMesh, sphereMesh, coneMesh);
        building.DrawPlants(basicShader, cylinderMesh, sphereMesh, coneMesh);

        // Draw button panel with textures
        buttonPanel.Draw(basicShader, boxMesh, btnTextures);

        // Draw floor indicator display inside elevator (on back wall)
        {
            int dispFloor = elevator.currentFloor;
            if (dispFloor >= 0 && dispFloor < 8 && floorTextures[dispFloor] != 0) {
                float elevHalfD = ELEVATOR_DEPTH / 2.0f;
                glm::vec3 dispPos(
                    SHAFT_CENTER_X,
                    elevator.currentY + ELEVATOR_HEIGHT * 0.75f,
                    SHAFT_CENTER_Z - elevHalfD + 0.08f
                );
                drawTexturedQuad3D(basicShader, boxMesh, floorTextures[dispFloor],
                    dispPos, glm::vec3(0.5f, 0.25f, 0.02f));
            }
        }

        // Draw light bulbs as emissive spheres
        for (int i = 0; i < NUM_FLOORS; i++) {
            float bulbY = i * FLOOR_HEIGHT + FLOOR_HEIGHT - 0.35f;
            glm::vec3 bulbColor(1.0f, 0.95f, 0.8f);
            glUniform3fv(glGetUniformLocation(basicShader, "solidColor"), 1, glm::value_ptr(bulbColor));
            glUniform1i(glGetUniformLocation(basicShader, "useTexture"), 0);
            glUniform3fv(glGetUniformLocation(basicShader, "emissiveColor"), 1, glm::value_ptr(bulbColor));
            glUniform1f(glGetUniformLocation(basicShader, "emissiveStrength"), 1.0f);

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, bulbY, -BUILDING_DEPTH / 2.0f));
            model = glm::scale(model, glm::vec3(0.07f, 0.07f, 0.07f));
            glUniformMatrix4fv(glGetUniformLocation(basicShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glBindVertexArray(sphereMesh.VAO);
            glDrawElements(GL_TRIANGLES, sphereMesh.indexCount, GL_UNSIGNED_INT, 0);
        }
        // Elevator bulb
        {
            float bulbY = elevator.currentY + ELEVATOR_HEIGHT - 0.28f;
            glm::vec3 bulbColor(1.0f, 0.95f, 0.8f);
            glUniform3fv(glGetUniformLocation(basicShader, "solidColor"), 1, glm::value_ptr(bulbColor));
            glUniform3fv(glGetUniformLocation(basicShader, "emissiveColor"), 1, glm::value_ptr(bulbColor));
            glUniform1f(glGetUniformLocation(basicShader, "emissiveStrength"), 1.0f);

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(SHAFT_CENTER_X, bulbY, SHAFT_CENTER_Z));
            model = glm::scale(model, glm::vec3(0.06f, 0.06f, 0.06f));
            glUniformMatrix4fv(glGetUniformLocation(basicShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glBindVertexArray(sphereMesh.VAO);
            glDrawElements(GL_TRIANGLES, sphereMesh.indexCount, GL_UNSIGNED_INT, 0);
        }
        // Reset emissive
        {
            glm::vec3 zero(0.0f);
            glUniform3fv(glGetUniformLocation(basicShader, "emissiveColor"), 1, glm::value_ptr(zero));
            glUniform1f(glGetUniformLocation(basicShader, "emissiveStrength"), 0.0f);
        }

        glBindVertexArray(0);

        // ============ HUD OVERLAY ============
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

        glUseProgram(hudShader);

        // Crosshair
        {
            glm::vec3 crossColor = elevator.ventilationColorActive
                ? glm::vec3(0.3f, 0.8f, 1.0f)
                : glm::vec3(1.0f, 1.0f, 1.0f);
            float crossAlpha = elevator.ventilationColorActive ? 1.0f : 0.7f;

            glUniform1i(glGetUniformLocation(hudShader, "uIsTexture"), 0);
            glUniform3fv(glGetUniformLocation(hudShader, "uColor"), 1, glm::value_ptr(crossColor));
            glUniform1f(glGetUniformLocation(hudShader, "uAlpha"), crossAlpha);

            float crossSize = 0.018f;
            float crossThick = 0.003f;

            // Horizontal
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::scale(model, glm::vec3(crossSize * 2.0f, crossThick * 2.0f, 1.0f));
            glUniformMatrix4fv(glGetUniformLocation(hudShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glBindVertexArray(quadMesh.VAO);
            glDrawElements(GL_TRIANGLES, quadMesh.indexCount, GL_UNSIGNED_INT, 0);

            // Vertical
            model = glm::mat4(1.0f);
            model = glm::scale(model, glm::vec3(crossThick * 2.0f, crossSize * 2.0f, 1.0f));
            glUniformMatrix4fv(glGetUniformLocation(hudShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glDrawElements(GL_TRIANGLES, quadMesh.indexCount, GL_UNSIGNED_INT, 0);
        }

        // Aimed button highlight
        if (playerInElevator) {
            int aimed = buttonPanel.Raycast(camera.Position, camera.Front, 3.0f);
            if (aimed >= 0) {
                glUniform1i(glGetUniformLocation(hudShader, "uIsTexture"), 0);
                glUniform3f(glGetUniformLocation(hudShader, "uColor"), 1.0f, 1.0f, 0.0f);
                glUniform1f(glGetUniformLocation(hudShader, "uAlpha"), 0.3f);

                glm::mat4 model = glm::mat4(1.0f);
                model = glm::scale(model, glm::vec3(0.05f, 0.05f, 1.0f));
                glUniformMatrix4fv(glGetUniformLocation(hudShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
                glBindVertexArray(quadMesh.VAO);
                glDrawElements(GL_TRIANGLES, quadMesh.indexCount, GL_UNSIGNED_INT, 0);
            }
        }

        // Student info texture (bottom-right, semi-transparent)
        if (studentInfoTex != 0) {
            glUniform1i(glGetUniformLocation(hudShader, "uIsTexture"), 1);
            glUniform1f(glGetUniformLocation(hudShader, "uAlpha"), 0.6f);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, studentInfoTex);
            glUniform1i(glGetUniformLocation(hudShader, "uTexture"), 0);

            float infoW = 0.3f;
            float infoH = 0.06f;
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(1.0f - infoW, -1.0f + infoH, 0.0f));
            model = glm::scale(model, glm::vec3(infoW * 2.0f, infoH * 2.0f, 1.0f));
            glUniformMatrix4fv(glGetUniformLocation(hudShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glBindVertexArray(quadMesh.VAO);
            glDrawElements(GL_TRIANGLES, quadMesh.indexCount, GL_UNSIGNED_INT, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        // Floor indicator HUD (top-left corner)
        {
            int dispFloor = playerInElevator ? elevator.currentFloor : playerFloor;
            if (dispFloor >= 0 && dispFloor < 8 && floorTextures[dispFloor] != 0) {
                glUniform1i(glGetUniformLocation(hudShader, "uIsTexture"), 1);
                glUniform1f(glGetUniformLocation(hudShader, "uAlpha"), 0.85f);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, floorTextures[dispFloor]);
                glUniform1i(glGetUniformLocation(hudShader, "uTexture"), 0);

                float w = 0.08f;
                float h = 0.05f;
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(-1.0f + w + 0.02f, 1.0f - h - 0.02f, 0.0f));
                model = glm::scale(model, glm::vec3(w * 2.0f, h * 2.0f, 1.0f));
                glUniformMatrix4fv(glGetUniformLocation(hudShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
                glBindVertexArray(quadMesh.VAO);
                glDrawElements(GL_TRIANGLES, quadMesh.indexCount, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
            }
        }

        // Status indicator: blue bar = in elevator, green bar = on floor
        {
            glUniform1i(glGetUniformLocation(hudShader, "uIsTexture"), 0);
            if (playerInElevator) {
                glUniform3f(glGetUniformLocation(hudShader, "uColor"), 0.2f, 0.4f, 0.9f);
            } else {
                glUniform3f(glGetUniformLocation(hudShader, "uColor"), 0.2f, 0.8f, 0.3f);
            }
            glUniform1f(glGetUniformLocation(hudShader, "uAlpha"), 0.6f);

            float barW = 0.005f;
            float barH = 0.04f;
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-1.0f + 0.005f, 1.0f - 0.05f, 0.0f));
            model = glm::scale(model, glm::vec3(barW * 2.0f, barH * 2.0f, 1.0f));
            glUniformMatrix4fv(glGetUniformLocation(hudShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glBindVertexArray(quadMesh.VAO);
            glDrawElements(GL_TRIANGLES, quadMesh.indexCount, GL_UNSIGNED_INT, 0);
        }

        glBindVertexArray(0);

        // Restore state
        if (depthTestEnabled) glEnable(GL_DEPTH_TEST);
        if (cullingEnabled) glEnable(GL_CULL_FACE);

        glfwSwapBuffers(window);
    }

    // Cleanup
    deleteMesh(quadMesh);
    deleteMesh(boxMesh);
    deleteMesh(cylinderMesh);
    deleteMesh(sphereMesh);
    deleteMesh(coneMesh);
    if (studentInfoTex) glDeleteTextures(1, &studentInfoTex);
    for (int i = 0; i < 12; i++) if (btnTextures[i]) glDeleteTextures(1, &btnTextures[i]);
    for (int i = 0; i < 8; i++) if (floorTextures[i]) glDeleteTextures(1, &floorTextures[i]);
    glDeleteProgram(basicShader);
    glDeleteProgram(hudShader);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
