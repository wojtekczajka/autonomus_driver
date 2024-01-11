#include "steering/auto_pilot.h"

AutoPilot::AutoPilot(RoadLaneDetectorCanny& detector, ISteeringClient& steeringClient, IDistanceClient& distanceClient, Logger& logger)
    : roadLaneDetector(detector), steeringClient(steeringClient), distanceClient(distanceClient), logger(logger), nextAction(NextAction::TurnLeft), turningProcedureStarted(false) {
}

double AutoPilot::getDistanceToObstancle() {
    return distanceToObstacle;
}

void AutoPilot::controlSteering() {
    distanceToObstacle = distanceClient.getDistance();
    if (isCollisionDetected()) {
        steeringClient.stop();
        currentAction = "stop collision detected";
        return;
    }

    int decenteredPixels = roadLaneDetector.getXPosition();
    if (roadLaneDetector.isTurnLeftDetected()) {
        steeringClient.turnLeft(MAX_TURN_VALUE);
        // steeringClient.driveForward(20);
        currentAction = "making turn left";
        return;
    }
    if (roadLaneDetector.isRightVerticalLaneDetected() && roadLaneDetector.isLeftVerticalLaneDetected()) {
        if (std::abs(decenteredPixels) < MIN_DEVIATION) {
            currentAction = "correction centering";
            steeringClient.center();
        } else if (decenteredPixels > 0) {
            currentAction = "correction right";
            int correction = decenteredPixels * CORRECTION_SENSIVITY;
            steeringClient.turnRight(std::min(correction, MAX_TURN_VALUE));
        } else {
            currentAction = "correction left";
            int correction = std::abs(decenteredPixels * CORRECTION_SENSIVITY);
            steeringClient.turnLeft(std::min(correction, MAX_TURN_VALUE));
        }
        // steeringClient.driveForward(23);
    }

    // else if (roadLaneDetector.isRightVerticalLaneDetected()) {
    //     steeringClient.turnLeft(MAX_TURN_VALUE);
    //     steeringClient.driveForward(THROTTLE_VALUE * 1.2);
    //     currentAction = "making turn left";
    // } else if (roadLaneDetector.isLeftVerticalLaneDetected()) {
    //     steeringClient.turnRight(MAX_TURN_VALUE);
    //     steeringClient.driveForward(THROTTLE_VALUE * 1.2);
    //     currentAction = "making turn right";
    // }
}

bool AutoPilot::isCollisionDetected() {
    return distanceToObstacle < COLLISION_DISTANCE_THRESHOLD;
}

std::string AutoPilot::getCurrentAction() {
    return currentAction;
}
