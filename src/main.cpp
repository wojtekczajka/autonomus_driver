#include <chrono>
#include <csignal>
#include <iostream>
#include <opencv2/opencv.hpp>

#include "camera/camera.h"
#include "camera/frame_dispatcher_client.h"
#include "distance/distance_client.h"
#include "road_lane_detector/road_lane_detector_canny.h"
#include "steering/auto_pilot.h"
#include "steering/steering_client.h"

void drawLineY(cv::Mat& image, const cv::Vec4f& line, const int& y1, const int& y2, cv::Scalar color, int thickness = 2) {
    float a = line[1] / line[0];
    float b = line[3] - (a * line[2]);
    cv::line(image, cv::Point2f((y1 - b) / a, y1), cv::Point2f((y2 - b) / a, y2), color, thickness);
}

void drawLineX(cv::Mat& image, const cv::Vec4f& line, const int& x1, const int& x2, cv::Scalar color, int thickness = 2) {
    float a = line[1] / line[0];
    float b = line[3] - (a * line[2]);
    cv::line(image, cv::Point2f(x1, a * x1 + b), cv::Point2f(x2, a * x2 + b), color, thickness);
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
    cv::VideoWriter videoWriter(outputVideoFile, fourcc, 30, cv::Size(640, 368), true);
    Logger logger("/dev/null");
    Camera camera(logger);
    DistanceClient distanceClient(logger, "ws://127.0.0.1:8000/distance");
    SteeringClient steeringClient(logger, "ws://127.0.0.1:8000/control");
    FrameDispatcherClient frameDispatcherClient(logger, "ws://localhost:8000/frame_dispatcher");
    RoadLaneDetectorCanny roadLaneDetectorCanny(frameDispatcherClient);
    AutoPilot autoPilot(roadLaneDetectorCanny, steeringClient, distanceClient, logger);
    if (!camera.isOpened()) {
        std::cerr << "Error: Couldn't open the camera." << std::endl;
        return -1;
    }
    int frameCount = 0;  // Initialize frame count
    steeringClient.start();
    // steeringClient.driveForward(40);
    int decenteredPixels;
    cv::Vec4f rightLane, leftLane, horizontalLane;
    while (true) {
        if (!camera.readFrame()) {
            std::cerr << "Error: Could not read frame." << std::endl;
            break;
        }

        cv::Mat frame = camera.getCurrentFrame();
        roadLaneDetectorCanny.processFrame(camera.getCurrentFrame());
        if (roadLaneDetectorCanny.isRightVerticalLaneDetected()) {
            rightLane = roadLaneDetectorCanny.getRightVerticalLane();
            drawLineY(frame, rightLane, frame.rows, frame.rows / 2, cv::Scalar(0, 255, 0));
        }
        if (roadLaneDetectorCanny.isLeftVerticalLaneDetected()) {
            leftLane = roadLaneDetectorCanny.getLeftVerticalLane();
            drawLineY(frame, leftLane, frame.rows, frame.rows / 2, cv::Scalar(0, 255, 0));
        }
        if (roadLaneDetectorCanny.isTopHorizontalLaneDetected()) {
            horizontalLane = roadLaneDetectorCanny.getTopHorizontalLane();
        }
        if (roadLaneDetectorCanny.isRightVerticalLaneDetected() && roadLaneDetectorCanny.isLeftVerticalLaneDetected()) {
            decenteredPixels = roadLaneDetectorCanny.getXPosition();
        }

        autoPilot.controlSteering();
        std::string actionText = "Action: " + autoPilot.getCurrentAction();
        // frameDispatcherClient.sendFrame(frame, "original frame");
        
        
        float a = rightLane[1] / rightLane[0];
        float b = rightLane[3] - (a * rightLane[2]);
        cv::Point p1((frame.rows - b) / a, frame.rows - 5);
        cv::Point p2(frame.cols, frame.rows - 5);
        float distance1 = frame.cols - ((frame.rows - b) / a);
        cv::line(frame, p1, p2, cv::Scalar(0, 255, 0), 3);
        a = leftLane[1] / leftLane[0];
        b = leftLane[3] - (a * leftLane[2]);
        p1 = cv::Point((frame.rows - b) / a, frame.rows - 5);
        p2 = cv::Point(0, frame.rows - 5);
        float distance2 = ((frame.rows - b) / a);
        cv::line(frame, p1, p2, cv::Scalar(255, 0, 0), 3);
        // drawHorizontalLine(frame, horizontalLane[0], horizontalLane[1], horizontalLane[2], horizontalLane[3], cv::Scalar(255, 255, 255));

        cv::putText(
            frame,                                                                                           // Target image
            "right distance: " + std::to_string(distance1),  // Text to be added
            cv::Point(10, 240),                                                                              // Position
            cv::FONT_HERSHEY_SIMPLEX,                                                                        // Font type
            1.0,                                                                                             // Font scale
            cv::Scalar(255, 255, 255),                                                                       // Text color (white)
            2,                                                                                               // Text thickness
            cv::LINE_AA                                                                                      // Line type
        );
        
        cv::putText(
            frame,                                                                                           // Target image
            "left distance: " + std::to_string(distance2),  // Text to be added
            cv::Point(10, 210),                                                                              // Position
            cv::FONT_HERSHEY_SIMPLEX,                                                                        // Font type
            1.0,                                                                                             // Font scale
            cv::Scalar(255, 255, 255),                                                                       // Text color (white)
            2,                                                                                               // Text thickness
            cv::LINE_AA                                                                                      // Line type
        );

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

        cv::imshow("Result Frame", frame);
        cv::waitKey(1);
        // videoWriter.write(frame);
        // frameDispatcherClient.sendFrame(frame, "result frame");

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
