#include "steering/auto_pilot.h"

AutoPilot::AutoPilot(RoadLaneDetectorCanny& detector, ISteeringClient& steeringClient, IDistanceClient& distanceClient, Logger& logger)
    : roadLaneDetector(detector), steeringClient(steeringClient), distanceClient(distanceClient), logger(logger) {
}

void AutoPilot::controlSteering() {
    if (isCollisionDetected()) {
        steeringClient.stop();
        currentAction = "stop collision detected: " + std::to_string(distanceClient.getDistance()) + "cm";
        return;
    }

    int decenteredPixels = roadLaneDetector.getXPosition();
    if (roadLaneDetector.isRightVerticalLaneDetected() && roadLaneDetector.isLeftVerticalLaneDetected()) {
        if (std::abs(decenteredPixels) < MIN_DEVIATION) {
            currentAction = "correction centering";
            steeringClient.center();
        } else if (decenteredPixels > 0) {
            currentAction = "correction right";
            steeringClient.turnRight(std::min(decenteredPixels, MAX_TURN_VALUE));
        } else {
            currentAction = "correction left";
            steeringClient.turnLeft(std::min(std::abs(decenteredPixels), MAX_TURN_VALUE));
        }
        steeringClient.driveForward(THROTTLE_VALUE);
    } else if (roadLaneDetector.isRightVerticalLaneDetected()) {
        steeringClient.turnLeft(MAX_TURN_VALUE);
        steeringClient.driveForward(THROTTLE_VALUE * 1.1);
        currentAction = "making turn left";
    } else if (roadLaneDetector.isLeftVerticalLaneDetected()) {
        steeringClient.turnRight(MAX_TURN_VALUE);
        steeringClient.driveForward(THROTTLE_VALUE * 1.1);
        currentAction = "making turn right";
    }
}

bool AutoPilot::isCollisionDetected() {
    return distanceClient.getDistance() < COLLISION_DISTANCE_THRESHOLD;
}

std::string AutoPilot::getCurrentAction() {
    return currentAction;
}
