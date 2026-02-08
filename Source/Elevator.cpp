#include "../Header/Elevator.h"
#include <cmath>

Elevator::Elevator()
    : currentFloor(1), targetFloor(-1), moving(false), stopped(false),
      doorOpenAmount(0.0f), doorOpen(false), doorTimer(0.0f),
      doorExtended(false), waitingForDoors(false),
      ventilationOn(false), ventilationColorActive(false),
      firstTargetFloor(-1),
      floorRequests(NUM_FLOORS, false)
{
    currentY = GetFloorY(currentFloor);
}

float Elevator::GetFloorY(int floor) const {
    return floor * FLOOR_HEIGHT;
}

bool Elevator::AreDoorsOpen() const {
    return doorOpenAmount > 0.01f;
}

bool Elevator::IsAtFloor(int floor) const {
    return !moving && currentFloor == floor;
}

int Elevator::findNextFloor() const {
    for (int i = 0; i < NUM_FLOORS; i++) {
        if (floorRequests[i]) return i;
    }
    return -1;
}

void Elevator::Update(float deltaTime) {
    // Door animation
    if (doorOpen) {
        doorTimer -= deltaTime;
        if (doorOpenAmount < 1.0f) {
            doorOpenAmount += DOOR_SPEED * deltaTime;
            if (doorOpenAmount > 1.0f) doorOpenAmount = 1.0f;
        }
        if (doorTimer <= 0.0f) {
            doorOpen = false;
        }
    } else {
        if (doorOpenAmount > 0.0f) {
            doorOpenAmount -= DOOR_SPEED * deltaTime;
            if (doorOpenAmount < 0.0f) doorOpenAmount = 0.0f;
        }
        if (doorOpenAmount <= 0.0f && waitingForDoors) {
            waitingForDoors = false;
            doorExtended = false;
            // Reset targetFloor so new requests can start
            targetFloor = -1;
            int next = findNextFloor();
            if (next >= 0) {
                targetFloor = next;
                moving = true;
            }
        }
    }

    // Movement
    if (moving && !stopped) {
        float targetY = GetFloorY(targetFloor);
        float diff = targetY - currentY;
        float step = ELEVATOR_SPEED * deltaTime;

        if (fabs(diff) <= step) {
            currentY = targetY;
            currentFloor = targetFloor;
            moving = false;
            floorRequests[currentFloor] = false;

            // Arrived: open doors
            doorOpen = true;
            doorTimer = DOOR_OPEN_TIME;
            doorExtended = false;
            waitingForDoors = true;

            // Ventilation color deactivation on first arrival
            if (ventilationColorActive && currentFloor == firstTargetFloor) {
                ventilationColorActive = false;
            }
        } else {
            if (diff > 0) currentY += step;
            else currentY -= step;
        }
    }
}

void Elevator::RequestFloor(int floor) {
    if (floor < 0 || floor >= NUM_FLOORS) return;
    if (floor == currentFloor && !moving) return;

    floorRequests[floor] = true;

    // Start moving if idle
    if (!moving && doorOpenAmount <= 0.0f) {
        targetFloor = floor;
        firstTargetFloor = floor;
        moving = true;
        waitingForDoors = false;
        if (ventilationOn) ventilationColorActive = true;
    } else if (!moving && (doorOpen || doorOpenAmount > 0.0f)) {
        // Doors are open/closing - close them first, then move
        doorOpen = false; // trigger closing
        doorTimer = 0.0f;
        waitingForDoors = true;
    }
}

void Elevator::CloseDoors() {
    if (doorOpen || doorOpenAmount > 0.0f) {
        doorOpen = false;
        doorTimer = 0.0f;
    }
}

void Elevator::OpenDoors() {
    // Can open doors when stopped at a floor (not moving)
    if (!moving) {
        doorOpen = true;
        doorTimer = DOOR_OPEN_TIME;
        doorExtended = false;
        waitingForDoors = true;
    }
}

void Elevator::ToggleStop() {
    stopped = !stopped;
}

void Elevator::ToggleVentilation() {
    ventilationOn = !ventilationOn;
    if (ventilationOn && moving) {
        ventilationColorActive = true;
    }
}

void Elevator::CallToFloor(int floor) {
    if (currentFloor == floor && !moving) {
        // Already here, just open doors
        if (!doorOpen) {
            doorOpen = true;
            doorTimer = DOOR_OPEN_TIME;
            doorExtended = false;
            waitingForDoors = true;
        }
    } else {
        // Request this floor
        floorRequests[floor] = true;
        if (!moving && doorOpenAmount <= 0.0f) {
            targetFloor = floor;
            firstTargetFloor = floor;
            moving = true;
            waitingForDoors = false;
        } else if (!moving) {
            // Close doors first
            doorOpen = false;
            doorTimer = 0.0f;
            waitingForDoors = true;
        }
    }
}
