#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "Constants.h"

class Elevator {
public:
    int currentFloor;
    int targetFloor;
    float currentY;
    bool moving;
    bool stopped;

    float doorOpenAmount;   // 0.0=closed, 1.0=fully open
    bool doorOpen;
    float doorTimer;
    bool doorExtended;
    bool waitingForDoors;

    bool ventilationOn;
    bool ventilationColorActive;
    int firstTargetFloor;

    std::vector<bool> floorRequests;

    Elevator();

    void Update(float deltaTime);

    void RequestFloor(int floor);
    void CloseDoors();
    void OpenDoors();
    void ToggleStop();
    void ToggleVentilation();

    float GetFloorY(int floor) const;
    bool AreDoorsOpen() const;
    bool IsAtFloor(int floor) const;

    // Call elevator to a floor from outside
    void CallToFloor(int floor);

private:
    int findNextFloor() const;
};
