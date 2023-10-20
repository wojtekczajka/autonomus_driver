#include <chrono>
#include <csignal>
#include <iostream>
#include <opencv2/opencv.hpp>

#include "camera/camera.h"
#include "distance/distance_client.h"
#include "road_lane_detector/road_lane_detector_canny.h"
#include "steering/steering_client.h"
#include "steering/auto_pilot.h"

void drawLine(cv::Mat& image, double vx, double vy, double x0, double y0, cv::Scalar color, int thickness = 2) {
    float x1 = x0 + (0 - y0) * vx / vy;
    float x2 = x0 + (image.rows - 1 - y0) * vx / vy;

    cv::Point2f startPoint(x1, 0);
    cv::Point2f endPoint(x2, image.rows - 1);
    cv::line(image, startPoint, endPoint, color, thickness);
}

void drawHorizontalLine(cv::Mat& image, double vx, double vy, double x0, double y0, cv::Scalar color, int thickness = 2) {
    float y1 = y0 + (0 - x0) * vy / vx;
    float y2 = y0 + (image.cols - 1 - x0) * vy / vx;

    cv::Point2f startPoint(0, y1);
    cv::Point2f endPoint(image.cols - 1, y2);
    cv::line(image, startPoint, endPoint, color, thickness);
}

volatile bool shouldExit = false;

void signalHandler(int signum) {
    if (signum == SIGINT) {
        shouldExit = true;
    }
}

std::pair<cv::Point2f, cv::Point2f> vectorToLinePointss(const cv::Vec4f& vector, double min_y, double max_y) {
    float vx = vector[0];
    float vy = vector[1];
    float x0 = vector[2];
    float y0 = vector[3];

    float x1 = x0 + (min_y - y0) * vx / vy;
    float x2 = x0 + (max_y - y0) * vx / vy;

    cv::Point2f startPoint(x1, min_y);
    cv::Point2f endPoint(x2, max_y);

    return std::make_pair(startPoint, endPoint);
}

int main() {
    signal(SIGINT, signalHandler);
    std::string outputVideoFile = "output_video.avi";
    int fourcc = cv::VideoWriter::fourcc('X', 'V', 'I', 'D');
    cv::VideoWriter videoWriter(outputVideoFile, fourcc, 15, cv::Size(640, 368), true);
    Logger logger("/dev/null");
    Camera camera(logger);
    DistanceClient distanceClient("http://127.0.0.1:8000");
    SteeringClient steeringClient(logger);
    RoadLaneDetectorCanny roadLaneDetectorCanny;
    AutoPilot autoPilot(roadLaneDetectorCanny, steeringClient, distanceClient, logger);
    if (!camera.isOpened()) {
        std::cerr << "Error: Couldn't open the camera." << std::endl;
        return -1;
    }
    int frameCount = 0;  // Initialize frame count
    steeringClient.start();
    // steeringClient.driveForward(40);
    int decenteredPixels;
    cv::Vec4i rightLane, leftLane, horizontalLane;
    while (true) {
        if (!camera.readFrame()) {
            std::cerr << "Error: Could not read frame." << std::endl;
            break;
        }

        roadLaneDetectorCanny.processFrame(camera.getCurrentFrame());
        if (roadLaneDetectorCanny.isRightVerticalLaneDetected()) {
            rightLane = roadLaneDetectorCanny.getRightVerticalLane();
        }
        if (roadLaneDetectorCanny.isLeftVerticalLaneDetected()) {
            leftLane = roadLaneDetectorCanny.getLeftVerticalLane();
        }
        if (roadLaneDetectorCanny.isTopHorizontalLaneDetected()) {
            horizontalLane = roadLaneDetectorCanny.getTopHorizontalLane();
        }
        if (roadLaneDetectorCanny.isRightVerticalLaneDetected() && roadLaneDetectorCanny.isLeftVerticalLaneDetected()) {
            decenteredPixels = roadLaneDetectorCanny.getXPosition();
        }

        // Display current action on the frame
        
        autoPilot.controlSteering();
        std::string actionText = "Action: " + autoPilot.getCurrentAction();
        cv::Mat frame = camera.getCurrentFrame();

        drawLine(frame, rightLane[0], rightLane[1], rightLane[2], rightLane[3], cv::Scalar(255, 255, 255));
        drawLine(frame, leftLane[0], leftLane[1], leftLane[2], leftLane[3], cv::Scalar(255, 255, 255));
        drawHorizontalLine(frame, horizontalLane[0], horizontalLane[1], horizontalLane[2], horizontalLane[3], cv::Scalar(255, 255, 255));

        cv::putText(
            frame,                                                                                           // Target image
            "center line detected: " + std::to_string(roadLaneDetectorCanny.isTopHorizontalLaneDetected()),  // Text to be added
            cv::Point(10, 180),                                                                              // Position
            cv::FONT_HERSHEY_SIMPLEX,                                                                        // Font type
            1.0,                                                                                             // Font scale
            cv::Scalar(255, 255, 255),                                                                       // Text color (white)
            2,                                                                                               // Text thickness
            cv::LINE_AA                                                                                      // Line type
        );

        cv::putText(
            frame,                                                                                          // Target image
            "right line detected: " + std::to_string(roadLaneDetectorCanny.isRightVerticalLaneDetected()),  // Text to be added
            cv::Point(10, 150),                                                                             // Position
            cv::FONT_HERSHEY_SIMPLEX,                                                                       // Font type
            1.0,                                                                                            // Font scale
            cv::Scalar(255, 255, 255),                                                                      // Text color (white)
            2,                                                                                              // Text thickness
            cv::LINE_AA                                                                                     // Line type
        );

        cv::putText(
            frame,                                                                                        // Target image
            "left line detected: " + std::to_string(roadLaneDetectorCanny.isLeftVerticalLaneDetected()),  // Text to be added
            cv::Point(10, 120),                                                                           // Position
            cv::FONT_HERSHEY_SIMPLEX,                                                                     // Font type
            1.0,                                                                                          // Font scale
            cv::Scalar(255, 255, 255),                                                                    // Text color (white)
            2,                                                                                            // Text thickness
            cv::LINE_AA                                                                                   // Line type
        );

        cv::putText(
            frame,                      // Target image
            actionText,                 // Text to be added
            cv::Point(10, 90),          // Position
            cv::FONT_HERSHEY_SIMPLEX,   // Font type
            1.0,                        // Font scale
            cv::Scalar(255, 255, 255),  // Text color (white)
            2,                          // Text thickness
            cv::LINE_AA                 // Line type
        );

        cv::putText(
            frame,                                              // Target image
            "Decentered: " + std::to_string(decenteredPixels),  // Text to be added
            cv::Point(10, 60),                                  // Position
            cv::FONT_HERSHEY_SIMPLEX,                           // Font type
            1.0,                                                // Font scale
            cv::Scalar(255, 255, 255),                          // Text color (white)
            2,                                                  // Text thickness
            cv::LINE_AA                                         // Line type
        );

        std::string fpsString = "FPS: " + std::to_string(camera.getFPS());

        cv::putText(
            frame,                      // Target image
            fpsString,                  // Text to be added
            cv::Point(10, 30),          // Position
            cv::FONT_HERSHEY_SIMPLEX,   // Font type
            1.0,                        // Font scale
            cv::Scalar(255, 255, 255),  // Text color (white)
            2,                          // Text thickness
            cv::LINE_AA                 // Line type
        );

        cv::imshow("Camera Frame", frame);
        videoWriter.write(frame);

        if (cv::waitKey(1) == 'q' || shouldExit == true) {
            break;
        }
        frameCount++;
    }

    // Release the camera capture object
    steeringClient.stop();
    cv::destroyAllWindows();
    videoWriter.release();

    return 0;
}
