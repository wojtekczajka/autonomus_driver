#include "steering/auto_pilot.h"

AutoPilot::AutoPilot(RoadLaneDetectorCanny& detector, SteeringClient& client)
    : roadLaneDetector(detector), steeringClient(client) {
}

void AutoPilot::controlSteering() {
    int decenteredPixels = roadLaneDetector.getXPosition();
    if (roadLaneDetector.isRightVerticalLaneDetected() && roadLaneDetector.isLeftVerticalLaneDetected()) {
        if (std::abs(decenteredPixels) <= 10) {
            currentAction = "correction centering";
            steeringClient.center();
        } else if (decenteredPixels > 0) {
            currentAction = "correction right";
            steeringClient.turnRight(40);
        } else {
            currentAction = "correction left";
            steeringClient.turnLeft(40);
        }
    } else if (roadLaneDetector.isRightVerticalLaneDetected()) {
        steeringClient.turnLeft(100);
        currentAction = "making turn left";
    }
}

std::string AutoPilot::getCurrentAction() {
    return currentAction;
}
