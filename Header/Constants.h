#pragma once

// Window / timing
const float TARGET_FPS = 75.0f;
const float TARGET_FRAME_TIME = 1.0f / TARGET_FPS;

// Building - room is open on front side (z=0), walls on sides and back
const int NUM_FLOORS = 8;
const float FLOOR_HEIGHT = 3.0f;
const float BUILDING_WIDTH = 12.0f;
const float BUILDING_DEPTH = 10.0f;
const float WALL_THICKNESS = 0.15f;

// Elevator shaft position (center) - at the back wall, centered X
const float SHAFT_CENTER_X = 0.0f;
const float SHAFT_CENTER_Z = -BUILDING_DEPTH + WALL_THICKNESS + 1.5f; // near back wall

// Elevator cab
const float ELEVATOR_WIDTH = 2.8f;
const float ELEVATOR_DEPTH = 2.8f;
const float ELEVATOR_HEIGHT = 2.8f;
const float ELEVATOR_SPEED = 3.0f;
const float DOOR_SPEED = 0.5f;
const float DOOR_OPEN_TIME = 5.0f;
const float DOOR_WIDTH = 1.5f;
const float DOOR_HEIGHT = 2.9f;

// Player / camera
const float PLAYER_HEIGHT = 1.7f;
const float PLAYER_SPEED = 3.5f;
const float MOUSE_SENSITIVITY = 0.1f;
const float CAMERA_FOV = 70.0f;
const float CAMERA_NEAR = 0.05f;
const float CAMERA_FAR = 200.0f;

// Button panel - bigger buttons so labels are readable
const int NUM_FLOOR_BUTTONS = 8;
const int NUM_CONTROL_BUTTONS = 4;
const float BUTTON_SIZE = 0.12f;
const float BUTTON_SPACING = 0.025f;

// Floor names
const char* const FLOOR_NAMES[] = { "SU", "PR", "1", "2", "3", "4", "5", "6" };

// Lighting
const int MAX_LIGHTS = 20;
