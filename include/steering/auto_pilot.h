#pragma once

#include "distance/distance_client.h"
#include "road_lane_detector/road_lane_detector_canny.h"
#include "steering/steering_client.h"

class AutoPilot {
   public:
    AutoPilot(RoadLaneDetectorCanny& detector, SteeringClient& steeringClient, DistanceClient& distanceClient);
    void controlSteering();
    std::string getCurrentAction();

   private:
    bool isColissionDetected();

    RoadLaneDetectorCanny& roadLaneDetector;
    SteeringClient& steeringClient;
    DistanceClient& distanceClient;
    std::string currentAction;
};
