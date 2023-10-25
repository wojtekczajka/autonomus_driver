#pragma once

#include "common/logger.h"
#include "distance/distance_client.h"
#include "road_lane_detector/road_lane_detector_canny.h"
#include "steering/steering_client.h"

class AutoPilot {
   public:
    AutoPilot(RoadLaneDetectorCanny& detector, ISteeringClient& steeringClient, IDistanceClient& distanceClient, Logger& logger);
    void controlSteering();
    std::string getCurrentAction();

   private:
    bool isColissionDetected();

    RoadLaneDetectorCanny& roadLaneDetector;
    ISteeringClient& steeringClient;
    IDistanceClient& distanceClient;
    Logger& logger;
    std::string currentAction;
};
