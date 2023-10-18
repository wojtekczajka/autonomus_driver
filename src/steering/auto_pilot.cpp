#include "steering/auto_pilot.h"

AutoPilot::AutoPilot(RoadLaneDetectorCanny& detector, SteeringClient& steeringClient, DistanceClient& distanceClient, Logger& logger)
    : roadLaneDetector(detector), steeringClient(steeringClient), distanceClient(distanceClient), logger(logger) {
}

void AutoPilot::controlSteering() {
    if (isColissionDetected()) {
        steeringClient.stop();
        currentAction = "stop colission detected: " + std::to_string(distanceClient.getDistance()) + "cm";
        return;
    }
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
        steeringClient.driveForward(35);
    } else if (roadLaneDetector.isRightVerticalLaneDetected()) {
        steeringClient.turnLeft(100);
        steeringClient.driveForward(42);
        currentAction = "making turn left";
    }
}

bool AutoPilot::isColissionDetected() {
    return distanceClient.getDistance() < 30;
}

std::string AutoPilot::getCurrentAction() {
    return currentAction;
}
