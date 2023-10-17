#include "steering/auto_pilot.h"

AutoPilot::AutoPilot(RoadLaneDetectorCanny& detector, SteeringClient& steeringClient, DistanceClient& distanceClient)
    : roadLaneDetector(detector), steeringClient(steeringClient), distanceClient(distanceClient) {
}

void AutoPilot::controlSteering() {
    if (isColissionDetected()) {
        steeringClient.stop();
        std::cout << "colission detected: " << distanceClient.getDistance() << std::endl;
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
        steeringClient.driveForward(40);
    } else if (roadLaneDetector.isRightVerticalLaneDetected()) {
        steeringClient.turnLeft(100);
        currentAction = "making turn left";
    }
}

bool AutoPilot::isColissionDetected() {
    return distanceClient.getDistance() < 30;
}

std::string AutoPilot::getCurrentAction() {
    return currentAction;
}
