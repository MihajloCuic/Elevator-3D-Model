#include "../Header/ButtonPanel.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

void ButtonPanel::Init() {
    buttons.clear();
    offsets.clear();

    // Panel is on the right interior wall of the elevator
    // Buttons face -X direction (into elevator interior)
    glm::vec3 normal(-1.0f, 0.0f, 0.0f);

    float panelX = ELEVATOR_WIDTH / 2.0f - 0.12f; // inset from right wall so buttons are visible
    float panelCenterY = ELEVATOR_HEIGHT * 0.55f;  // above middle height
    float panelCenterZ = 0.0f; // relative to elevator center Z

    float btnSize = BUTTON_SIZE;
    float btnSpacing = BUTTON_SPACING;
    float totalW = 4.0f * btnSize + 3.0f * btnSpacing;
    float totalH = 2.0f * btnSize + 1.0f * btnSpacing;

    float startLocalY = panelCenterY + totalH / 2.0f - btnSize / 2.0f;
    float startLocalZ = panelCenterZ - totalW / 2.0f + btnSize / 2.0f;

    // Floor buttons: 4x2 grid
    // Row 0 (top): SU, PR, 1, 2
    // Row 1 (bottom): 3, 4, 5, 6
    for (int row = 0; row < 2; row++) {
        for (int col = 0; col < 4; col++) {
            int floorIdx = row * 4 + col;
            float dz = startLocalZ + col * (btnSize + btnSpacing);
            float dy = startLocalY - row * (btnSize + btnSpacing);

            Button3D btn;
            btn.center = glm::vec3(0.0f); // updated in UpdatePositions
            btn.normal = normal;
            btn.halfW = btnSize / 2.0f;
            btn.halfH = btnSize / 2.0f;
            btn.type = 0;
            btn.floorIndex = floorIdx;
            btn.active = false;
            btn.glowLightIdx = -1;
            btn.activeColor = glm::vec3(0.8f, 0.6f, 0.2f);
            btn.inactiveColor = glm::vec3(0.4f, 0.4f, 0.5f);
            buttons.push_back(btn);

            ButtonOffset off;
            off.dx = panelX;
            off.dy = dy;
            off.dz = dz;
            offsets.push_back(off);
        }
    }

    // Control buttons: below floor buttons
    float controlY = startLocalY - 2.0f * (btnSize + btnSpacing) - btnSpacing;

    // Close doors
    {
        Button3D btn;
        btn.normal = normal;
        btn.halfW = btnSize / 2.0f;
        btn.halfH = btnSize / 2.0f;
        btn.type = 1;
        btn.floorIndex = -1;
        btn.active = false;
        btn.glowLightIdx = -1;
        btn.activeColor = glm::vec3(0.7f, 0.2f, 0.2f);
        btn.inactiveColor = glm::vec3(0.5f, 0.25f, 0.25f);
        buttons.push_back(btn);
        offsets.push_back({ panelX, controlY, startLocalZ + 0 * (btnSize + btnSpacing) });
    }

    // Open doors
    {
        Button3D btn;
        btn.normal = normal;
        btn.halfW = btnSize / 2.0f;
        btn.halfH = btnSize / 2.0f;
        btn.type = 2;
        btn.floorIndex = -1;
        btn.active = false;
        btn.glowLightIdx = -1;
        btn.activeColor = glm::vec3(0.2f, 0.7f, 0.2f);
        btn.inactiveColor = glm::vec3(0.25f, 0.5f, 0.25f);
        buttons.push_back(btn);
        offsets.push_back({ panelX, controlY, startLocalZ + 1 * (btnSize + btnSpacing) });
    }

    // Stop
    {
        Button3D btn;
        btn.normal = normal;
        btn.halfW = btnSize / 2.0f;
        btn.halfH = btnSize / 2.0f;
        btn.type = 3;
        btn.floorIndex = -1;
        btn.active = false;
        btn.glowLightIdx = -1;
        btn.activeColor = glm::vec3(1.0f, 0.0f, 0.0f);
        btn.inactiveColor = glm::vec3(0.6f, 0.4f, 0.1f);
        buttons.push_back(btn);
        offsets.push_back({ panelX, controlY, startLocalZ + 2 * (btnSize + btnSpacing) });
    }

    // Ventilation
    {
        Button3D btn;
        btn.normal = normal;
        btn.halfW = btnSize / 2.0f;
        btn.halfH = btnSize / 2.0f;
        btn.type = 4;
        btn.floorIndex = -1;
        btn.active = false;
        btn.glowLightIdx = -1;
        btn.activeColor = glm::vec3(0.3f, 0.7f, 1.0f);
        btn.inactiveColor = glm::vec3(0.2f, 0.3f, 0.6f);
        buttons.push_back(btn);
        offsets.push_back({ panelX, controlY, startLocalZ + 3 * (btnSize + btnSpacing) });
    }
}

void ButtonPanel::UpdatePositions(float elevatorY) {
    for (size_t i = 0; i < buttons.size() && i < offsets.size(); i++) {
        buttons[i].center = glm::vec3(
            SHAFT_CENTER_X + offsets[i].dx,
            elevatorY + offsets[i].dy,
            SHAFT_CENTER_Z + offsets[i].dz
        );
    }
}

int ButtonPanel::Raycast(glm::vec3 rayOrigin, glm::vec3 rayDir, float maxDist) const {
    int closest = -1;
    float closestT = maxDist;

    for (size_t i = 0; i < buttons.size(); i++) {
        const Button3D& btn = buttons[i];

        float denom = glm::dot(btn.normal, rayDir);
        if (fabs(denom) < 0.0001f) continue;

        float t = glm::dot(btn.center - rayOrigin, btn.normal) / denom;
        if (t < 0 || t > closestT) continue;

        glm::vec3 hitPoint = rayOrigin + t * rayDir;
        glm::vec3 diff = hitPoint - btn.center;

        // Button local frame
        glm::vec3 up(0.0f, 1.0f, 0.0f);
        glm::vec3 right = glm::normalize(glm::cross(btn.normal, up));
        glm::vec3 localUp = glm::normalize(glm::cross(right, btn.normal));

        float u = glm::dot(diff, right);
        float v = glm::dot(diff, localUp);

        if (fabs(u) <= btn.halfW && fabs(v) <= btn.halfH) {
            closest = (int)i;
            closestT = t;
        }
    }

    return closest;
}

void ButtonPanel::Draw(unsigned int shader, const Mesh& box, unsigned int* btnTextures) const {
    for (size_t i = 0; i < buttons.size(); i++) {
        const Button3D& btn = buttons[i];
        glm::vec3 color = btn.active ? btn.activeColor : btn.inactiveColor;

        // Determine which texture index this button uses
        // Buttons 0-7: floor buttons (btn_0..btn_7)
        // Button 8: close doors (btn_8)
        // Button 9: open doors (btn_9)
        // Button 10: stop (btn_10)
        // Button 11: ventilation (btn_11)
        int texIdx = (int)i; // buttons are stored in order: 8 floor + 4 control
        bool hasTex = (btnTextures != nullptr && texIdx >= 0 && texIdx < 12 && btnTextures[texIdx] != 0);

        if (hasTex) {
            // Draw textured button with color tint
            glUniform1i(glGetUniformLocation(shader, "useTexture"), 1);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, btnTextures[texIdx]);
            glUniform1i(glGetUniformLocation(shader, "diffuseTexture"), 0);
        } else {
            glUniform3fv(glGetUniformLocation(shader, "solidColor"), 1, glm::value_ptr(color));
            glUniform1i(glGetUniformLocation(shader, "useTexture"), 0);
        }

        // Emissive for active buttons
        if (btn.active) {
            glUniform3fv(glGetUniformLocation(shader, "emissiveColor"), 1, glm::value_ptr(btn.activeColor));
            glUniform1f(glGetUniformLocation(shader, "emissiveStrength"), 0.8f);
        } else {
            glm::vec3 zero(0.0f);
            glUniform3fv(glGetUniformLocation(shader, "emissiveColor"), 1, glm::value_ptr(zero));
            glUniform1f(glGetUniformLocation(shader, "emissiveStrength"), 0.0f);
        }

        // Draw button as a small box protruding from the wall
        // Disable face culling for buttons so they're always visible
        glDisable(GL_CULL_FACE);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, btn.center);
        model = glm::scale(model, glm::vec3(0.04f, btn.halfH * 2.0f, btn.halfW * 2.0f));

        glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glBindVertexArray(box.VAO);
        glDrawElements(GL_TRIANGLES, box.indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glEnable(GL_CULL_FACE);
    }

    // Reset emissive and texture state
    glBindTexture(GL_TEXTURE_2D, 0);
    glUniform1i(glGetUniformLocation(shader, "useTexture"), 0);
    glm::vec3 zero(0.0f);
    glUniform3fv(glGetUniformLocation(shader, "emissiveColor"), 1, glm::value_ptr(zero));
    glUniform1f(glGetUniformLocation(shader, "emissiveStrength"), 0.0f);
}
