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
    static constexpr int COLLISION_DISTANCE_THRESHOLD = 30;
    static constexpr int MIN_DEVIATION = 10;
    static constexpr int MAX_TURN_VALUE = 100;
    static constexpr int THROTTLE_VALUE = 23;

    bool isCollisionDetected();

    RoadLaneDetectorCanny& roadLaneDetector;
    ISteeringClient& steeringClient;
    IDistanceClient& distanceClient;
    Logger& logger;
    std::string currentAction;
};
