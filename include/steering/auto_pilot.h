#pragma once

#include "common/logger.h"
#include "distance/distance_client.h"
#include "road_lane_detector/road_lane_detector_canny.h"
#include "road_sign_detector/speed_limit_detector.h"
#include "road_sign_detector/mandatory_sign_detector.h"
#include "traffic_light_detector/traffic_light_detector.h"
#include "steering/steering_client.h"

class AutoPilot {
   public:
    AutoPilot(TrafficLightDetector& trafficLightDetector, SpeedLimitDetector& speedLimitDetector, MandatorySignDetector& mandatorySignDetector, RoadLaneDetectorCanny& detector, ISteeringClient& steeringClient, IDistanceClient& distanceClient, Logger& logger);

    void controlSteering();
    std::string getCurrentAction();
    double getDistanceToObstancle();

   private:
    enum class NextAction {
        TurnLeft,
        TurnRight,
        DriveForward
    };

    static constexpr int COLLISION_DISTANCE_THRESHOLD = 30;
    static constexpr int MIN_DEVIATION = 5;
    static constexpr int MAX_TURN_VALUE = 100;
    static constexpr int THROTTLE_VALUE = 30;
    static constexpr double CORRECTION_SENSIVITY = 0.9;

    bool isCollisionDetected();

    RoadLaneDetectorCanny& roadLaneDetector;
    ISteeringClient& steeringClient;
    IDistanceClient& distanceClient;
    Logger& logger;
    MandatorySignDetector& mandatorySignDetector;
    SpeedLimitDetector& speedLimitDetector;
    TrafficLightDetector& trafficLightDetector;
    std::string currentAction;
    NextAction nextAction;
    int counter = 0;
    double distanceToObstacle;
    bool turningProcedureStarted;
};
