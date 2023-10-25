#include "steering/auto_pilot.h"

AutoPilot::AutoPilot(RoadLaneDetectorCanny& detector, ISteeringClient& steeringClient, IDistanceClient& distanceClient, Logger& logger)
    : roadLaneDetector(detector), steeringClient(steeringClient), distanceClient(distanceClient), logger(logger) {
}

void AutoPilot::controlSteering() {
    std::cout << "here" << std::endl;
    if (isColissionDetected()) {
        std::cout << "here1" << std::endl;
        steeringClient.stop();
        currentAction = "stop colission detected: " + std::to_string(distanceClient.getDistance()) + "cm";
        return;
    }
    std::cout << "here2" << std::endl;
    int decenteredPixels = roadLaneDetector.getXPosition();
    if (roadLaneDetector.isRightVerticalLaneDetected() && roadLaneDetector.isLeftVerticalLaneDetected()) {
        if (std::abs(decenteredPixels) <= 5) {
            currentAction = "correction centering";
            // steeringClient.center();
        } else if (decenteredPixels > 0) {
            currentAction = "correction right";
            // steeringClient.turnRight(35);
        } else {
            currentAction = "correction left";
            // steeringClient.turnLeft(35);
        }
        // steeringClient.driveForward(30);
    } else if (roadLaneDetector.isRightVerticalLaneDetected()) {
        // steeringClient.turnLeft(100);
        // steeringClient.driveForward(30);
        currentAction = "making turn left";
    }
}

bool AutoPilot::isColissionDetected() {
    return distanceClient.getDistance() < 30;
}

std::string AutoPilot::getCurrentAction() {
    return currentAction;
}
