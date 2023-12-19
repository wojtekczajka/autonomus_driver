#include "steering/auto_pilot.h"

AutoPilot::AutoPilot(RoadLaneDetectorCanny& detector, ISteeringClient& steeringClient, IDistanceClient& distanceClient, Logger& logger)
    : roadLaneDetector(detector), steeringClient(steeringClient), distanceClient(distanceClient), logger(logger), nextAction(NextAction::TurnLeft), turningProcedureStarted(false) {
}

void AutoPilot::controlSteering() {
    if (isCollisionDetected()) {
        steeringClient.stop();
        currentAction = "stop collision detected: " + std::to_string(distanceClient.getDistance()) + "cm";
        return;
    }

    int decenteredPixels = roadLaneDetector.getXPosition();
    if (roadLaneDetector.isRightVerticalLaneDetected() && roadLaneDetector.isLeftVerticalLaneDetected() && !turningProcedureStarted) {
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
        // steeringClient.driveForward(THROTTLE_VALUE);
    }

    if (roadLaneDetector.isTurnLeftDetected() && nextAction == NextAction::TurnLeft && !turningProcedureStarted) {
        if (++counter == 2)
            turningProcedureStarted = true;
    }

    if (!roadLaneDetector.isTurnLeftDetected() && nextAction == NextAction::TurnLeft && turningProcedureStarted) {
        steeringClient.turnLeft(MAX_TURN_VALUE);
        // steeringClient.driveForward(THROTTLE_VALUE * 1.2);
        currentAction = "making turn left";
        if (--counter + 3 == 0) {
            turningProcedureStarted = false;
            counter = 0;
        }
    }

    if (!roadLaneDetector.isTurnLeftDetected() && !turningProcedureStarted)
        counter = 0;

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
    return distanceClient.getDistance() < COLLISION_DISTANCE_THRESHOLD;
}

std::string AutoPilot::getCurrentAction() {
    return currentAction;
}
