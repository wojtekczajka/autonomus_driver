#include <chrono>
#include <csignal>
#include <iostream>
#include <opencv2/opencv.hpp>

#include "camera/camera.h"
#include "common/drawer.h"
#include "distance/distance_client.h"
#include "road_lane_detector/road_lane_detector_canny.h"
#include "steering/auto_pilot.h"
#include "steering/steering_client.h"
#include "road_sign_detector/speed_limit_detector.h"
#include "road_sign_detector/mandatory_sign_detector.h"
#include "traffic_light_detector/traffic_light_detector.h"

volatile bool shouldExit = false;

void signalHandler(int signum) {
    if (signum == SIGINT) {
        shouldExit = true;
    }
}

int main(int argc, char* argv[]) {
    signal(SIGINT, signalHandler);
    
    int decenteredPixels;
    cv::Vec4f rightLane, leftLane, leftBottomLane, rightBottomLane, leftTopLane, rightTopLane;

    // int fourcc = cv::VideoWriter::fourcc('X', 'V', 'I', 'D');
    // cv::VideoWriter videoWriterResult("result.avi", fourcc, 15, cv::Size(640 * 2, 368), true);

    Drawer drawer;
    Logger logger;
    Camera camera(logger);
    DistanceClient distanceClient(logger, "http://127.0.0.1:8000/distance");
    SteeringClient steeringClient(logger, "http://127.0.0.1:8000/control");
    RoadLaneDetectorCanny roadLaneDetectorCanny;
    SpeedLimitDetector speedLimitDetector;
    MandatorySignDetector mandatorySignDetector;
    TrafficLightDetector trafficLightDetector;
    AutoPilot autoPilot(trafficLightDetector, speedLimitDetector, mandatorySignDetector, roadLaneDetectorCanny, steeringClient, distanceClient, logger);
    if (!camera.isOpened()) {
        logger.error("Couldn't open the camera.");
        return -1;
    }

    steeringClient.start();

    while (true) {
        if (!camera.readFrame()) {
            logger.error("Could not read frame.");
            break;
        }

        cv::Mat frame = camera.getCurrentFrame();
        roadLaneDetectorCanny.processFrame(frame);
        speedLimitDetector.detectSpeedLimit(frame);
        mandatorySignDetector.detectMandatorySign(frame);
        trafficLightDetector.detectTrafficLight(frame);
        autoPilot.controlSteering();

        cv::Mat textRectangle = cv::Mat::zeros(frame.rows, frame.cols, frame.type());
        if (trafficLightDetector.isTrafficLightDetected())
            drawer.drawTextAboveBox(frame, trafficLightDetector.getTrafficLightPosition(), trafficLightDetector.getTrafficLightStr());
        if(mandatorySignDetector.isSignDetected()) 
            drawer.drawTextAboveBox(frame, mandatorySignDetector.getSignPosition(), mandatorySignDetector.getSignTypeStr());
        if(speedLimitDetector.isSignDetected()) 
            drawer.drawTextAboveBox(frame, speedLimitDetector.getSignPosition(), speedLimitDetector.getSpeedLimitValueStr());
        drawer.drawLanes(frame, roadLaneDetectorCanny.getRightVerticalLane(), roadLaneDetectorCanny.getLeftVerticalLane(),
                         roadLaneDetectorCanny.isRightVerticalLaneDetected(), roadLaneDetectorCanny.isLeftVerticalLaneDetected());
        drawer.updateText("turnRightDetected", std::to_string(roadLaneDetectorCanny.isTurnRightDetected()));
        drawer.updateText("turnLeftDetected", std::to_string(roadLaneDetectorCanny.isTurnLeftDetected()));
        drawer.updateText("rightVerticalLaneDetected", std::to_string(roadLaneDetectorCanny.isRightVerticalLaneDetected()));
        drawer.updateText("leftVerticalLaneDetected", std::to_string(roadLaneDetectorCanny.isLeftVerticalLaneDetected()));
        drawer.updateText("currentAction", autoPilot.getCurrentAction());
        drawer.updateText("distanceToObstacle", std::to_string(autoPilot.getDistanceToObstancle()));
        drawer.updateText("decenteredValue", std::to_string(roadLaneDetectorCanny.getXPosition()));
        drawer.updateText("FPS", std::to_string(camera.getFPS()));
        drawer.updateText("roadForwardDetected", std::to_string(roadLaneDetectorCanny.isRoadForwardBeyondIntersectionDetected()));
        drawer.drawAllText(textRectangle);

        cv::Mat displayImage = drawer.concatenateFrames(frame, textRectangle);
        cv::imshow("Result Frame", displayImage);
        // videoWriterResult.write(displayImage);

        if (cv::waitKey(1) == 'q' || shouldExit == true) {
            break;
        }
    }

    steeringClient.stop();
    cv::destroyAllWindows();

    return 0;
}
