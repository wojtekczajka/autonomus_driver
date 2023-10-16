#pragma once

#include "road_lane_detector/road_lane_detector_canny.h"
#include "steering/steering_client.h"

class AutoPilot {
public:
    AutoPilot(RoadLaneDetectorCanny& detector, SteeringClient& client);
    void controlSteering();
    std::string getCurrentAction();

private:
    RoadLaneDetectorCanny& roadLaneDetector;
    SteeringClient& steeringClient;

    std::string currentAction;
};
