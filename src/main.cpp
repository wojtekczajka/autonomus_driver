#include <chrono>
#include <csignal>
#include <iostream>
#include <opencv2/opencv.hpp>

#include "camera/camera.h"
#include "distance/distance_client.h"
#include "road_lane_detector/road_lane_detector_canny.h"
#include "steering/steering_client.h"

void drawLine(cv::Mat& image, double vx, double vy, double x0, double y0, cv::Scalar color, int thickness = 2) {
    float x1 = x0 + (0 - y0) * vx / vy;
    float x2 = x0 + (image.rows - 1 - y0) * vx / vy;

    cv::Point2f startPoint(x1, 0);
    cv::Point2f endPoint(x2, image.rows - 1);
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
    cv::VideoWriter videoWriter(outputVideoFile, fourcc, 30, cv::Size(640, 368), true);
    Logger logger("/dev/null");
    Camera camera(logger);
    DistanceClient distanceClient("http://127.0.0.1:8000");
    SteeringClient steeringClient(logger);
    RoadLaneDetectorCanny roadLaneDetectorCanny;
    // RoadLineDetector roadLineDetector(logger);
    if (!camera.isOpened()) {
        std::cerr << "Error: Couldn't open the camera." << std::endl;
        return -1;
    }
    int frameCount = 0;  // Initialize frame count
    bool stopCar = false;
    steeringClient.start();
    // steeringClient.driveForward(35);
    // std::this_thread::sleep_for(std::chrono::seconds(5));
    // steeringClient.stop();
    int decenteredPixels;
    std::pair<cv::Vec4i, cv::Vec4i> lanes;
    while (true) {
        if (!camera.readFrame()) {
            std::cerr << "Error: Could not read frame." << std::endl;
            break;
        }

        try {
            roadLaneDetectorCanny.processFrame(camera.getCurrentFrame());
            decenteredPixels = roadLaneDetectorCanny.getXPosition(camera.getCurrentFrame());
            lanes = roadLaneDetectorCanny.getLanes();

        } catch (std::runtime_error er) {
            er.what();
            continue;
        }

        // Display current action on the frame
        std::string actionText = "Action: ";
        // if (!stopCar) {
        if (decenteredPixels != decenteredPixels) {
            actionText += "Nan";
            steeringClient.turnRight(80);
        } else if (std::abs(decenteredPixels) <= 10) {
            actionText += "Centering";
            steeringClient.center();
        } else {
            if (decenteredPixels > 0) {
                actionText += "Turning right";
                steeringClient.turnRight(80);
            } else {
                actionText += "Turning left";
                steeringClient.turnLeft(80);
            }
        }
        cv::Mat frame = camera.getCurrentFrame();
        std::cout << distanceClient.getDistance() << std::endl;
        drawLine(frame, lanes.first[0], lanes.first[1], lanes.first[2], lanes.first[3], cv::Scalar(255, 255, 255));
        drawLine(frame, lanes.second[0], lanes.second[1], lanes.second[2], lanes.second[3], cv::Scalar(255, 255, 255));
        cv::putText(
            frame,  // Target image
            "distance: " +
                std::to_string((int)distanceClient.getDistance()) +
                "cm",                   // Text to be added
            cv::Point(10, 120),         // Position
            cv::FONT_HERSHEY_SIMPLEX,   // Font type
            1.0,                        // Font scale
            cv::Scalar(255, 255, 255),  // Text color (white)
            2,                          // Text thickness
            cv::LINE_AA                 // Line type
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
