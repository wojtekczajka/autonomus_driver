#include <chrono>
#include <csignal>
#include <iostream>
#include <opencv2/opencv.hpp>

#include "camera/camera.h"
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

int main(int argc, char* argv[]) {
    std::string turnLeftDetected = "false";
    std::string turnRightDetected = "false";
    signal(SIGINT, signalHandler);
    // std::string outputVideoFile = argv[1];
    int fourcc = cv::VideoWriter::fourcc('X', 'V', 'I', 'D');
    // cv::VideoWriter videoWriterRaw("raw_" + outputVideoFile, fourcc, 15, cv::Size(640, 368), true);
    cv::VideoWriter videoWriterResult("result.avi", fourcc, 15, cv::Size(640, 368), true);
    Logger logger("log1.log");
    Camera camera(logger);
    DistanceClient distanceClient(logger, "http://127.0.0.1:8000/distance");
    SteeringClient steeringClient(logger, "http://127.0.0.1:8000/control");
    RoadLaneDetectorCanny roadLaneDetectorCanny;
    AutoPilot autoPilot(roadLaneDetectorCanny, steeringClient, distanceClient, logger);
    if (!camera.isOpened()) {
        std::cerr << "Error: Couldn't open the camera." << std::endl;
        return -1;
    }
    int frameCount = 0;  // Initialize frame count
    steeringClient.start();
    int decenteredPixels;
    cv::Vec4f rightLane, leftLane, leftBottomLane, rightBottomLane, leftTopLane, rightTopLane;
    while (true) {
        if (!camera.readFrame()) {
            std::cerr << "Error: Could not read frame." << std::endl;
            break;
        }

        cv::Mat frame = camera.getCurrentFrame();
        // videoWriterRaw.write(frame);
        roadLaneDetectorCanny.processFrame(camera.getCurrentFrame());
        if (roadLaneDetectorCanny.isRightVerticalLaneDetected()) {
            rightLane = roadLaneDetectorCanny.getRightVerticalLane();
            drawLineY(frame, rightLane, frame.rows, frame.rows / 2, cv::Scalar(0, 255, 0));
        }
        if (roadLaneDetectorCanny.isLeftVerticalLaneDetected()) {
            leftLane = roadLaneDetectorCanny.getLeftVerticalLane();
            drawLineY(frame, leftLane, frame.rows, frame.rows / 2, cv::Scalar(0, 255, 0));
        }
        if (roadLaneDetectorCanny.isRightVerticalLaneDetected() && roadLaneDetectorCanny.isLeftVerticalLaneDetected()) {
            decenteredPixels = roadLaneDetectorCanny.getXPosition();
        }

        autoPilot.controlSteering();
        std::string actionText = "Action: " + autoPilot.getCurrentAction();

        cv::Mat textRectangle = cv::Mat::zeros(frame.rows, frame.cols, frame.type());
        cv::putText(
            textRectangle,                                           // Target image
            "turn left detected: " + std::to_string(roadLaneDetectorCanny.isTurnLeftDetected()),  // Text to be added
            cv::Point(10, 210),                              // Position
            cv::FONT_HERSHEY_SIMPLEX,                        // Font type
            1.0,                                             // Font scale
            cv::Scalar(255, 255, 255),                       // Text color (white)
            2,                                               // Text thickness
            cv::LINE_AA                                      // Line type
        );

        cv::putText(
            textRectangle,                                          // Target image
            "turn right detected: " + std::to_string(roadLaneDetectorCanny.isTurnRightDetected()),  // Text to be added
            cv::Point(10, 180),                             // Position
            cv::FONT_HERSHEY_SIMPLEX,                       // Font type
            1.0,                                            // Font scale
            cv::Scalar(255, 255, 255),                      // Text color (white)
            2,                                              // Text thickness
            cv::LINE_AA                                     // Line type
        );

        cv::putText(
            textRectangle,                                                                                          // Target image
            "right line detected: " + std::to_string(roadLaneDetectorCanny.isRightVerticalLaneDetected()),  // Text to be added
            cv::Point(10, 150),                                                                             // Position
            cv::FONT_HERSHEY_SIMPLEX,                                                                       // Font type
            1.0,                                                                                            // Font scale
            cv::Scalar(255, 255, 255),                                                                      // Text color (white)
            2,                                                                                              // Text thickness
            cv::LINE_AA                                                                                     // Line type
        );

        cv::putText(
            textRectangle,                                                                                        // Target image
            "left line detected: " + std::to_string(roadLaneDetectorCanny.isLeftVerticalLaneDetected()),  // Text to be added
            cv::Point(10, 120),                                                                           // Position
            cv::FONT_HERSHEY_SIMPLEX,                                                                     // Font type
            1.0,                                                                                          // Font scale
            cv::Scalar(255, 255, 255),                                                                    // Text color (white)
            2,                                                                                            // Text thickness
            cv::LINE_AA                                                                                   // Line type
        );

        cv::putText(
            textRectangle,                      // Target image
            actionText,                 // Text to be added
            cv::Point(10, 90),          // Position
            cv::FONT_HERSHEY_SIMPLEX,   // Font type
            1.0,                        // Font scale
            cv::Scalar(255, 255, 255),  // Text color (white)
            2,                          // Text thickness
            cv::LINE_AA                 // Line type
        );

        cv::putText(
            textRectangle,                                              // Target image
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
            textRectangle,                      // Target image
            fpsString,                  // Text to be added
            cv::Point(10, 30),          // Position
            cv::FONT_HERSHEY_SIMPLEX,   // Font type
            1.0,                        // Font scale
            cv::Scalar(255, 255, 255),  // Text color (white)
            2,                          // Text thickness
            cv::LINE_AA                 // Line type
        );

        cv::Mat displayImage;
        cv::hconcat(frame, textRectangle, displayImage);
        cv::imshow("Result Frame", displayImage);
        cv::waitKey(1);
        videoWriterResult.write(frame);
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
    // videoWriterRaw.release();
    // videoWriterResult.release();

    return 0;
}
