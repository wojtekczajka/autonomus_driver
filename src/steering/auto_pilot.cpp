#include "steering/auto_pilot.h"

AutoPilot::AutoPilot(RoadLaneDetectorCanny& detector, ISteeringClient& steeringClient, IDistanceClient& distanceClient, Logger& logger)
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
        if (std::abs(decenteredPixels) <= 5) {
            currentAction = "correction centering";
            steeringClient.center();
        } else if (decenteredPixels > 0) {
            currentAction = "correction right";
            steeringClient.turnRight(27);
        } else {
            currentAction = "correction left";
            steeringClient.turnLeft(27);
        }
        steeringClient.driveForward(24);
    } else if (roadLaneDetector.isRightVerticalLaneDetected()) {
        steeringClient.turnLeft(100);
        steeringClient.driveForward(28);
        currentAction = "making turn left";
    } else if (roadLaneDetector.isLeftVerticalLaneDetected()) {
        int distance = roadLaneDetector.getLeftDistance();
        if (distance > 75 ) 
            steeringClient.turnLeft(25);
        else if (distance < 65) 
           steeringClient.turnRight(25); 
        else 
            steeringClient.center();
        currentAction = "skipping turn right";
    }
}

bool AutoPilot::isColissionDetected() {
    return distanceClient.getDistance() < 30;
}

std::string AutoPilot::getCurrentAction() {
    return currentAction;
}
