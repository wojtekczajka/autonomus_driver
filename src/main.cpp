#include <chrono>
#include <csignal>
#include <iostream>
#include <opencv2/opencv.hpp>

#include "camera/camera.h"
#include "distance/distance_client.h"
#include "road_lane_detector/road_lane_detector_contours.h"
#include "steering_control/steering_control.h"

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
    SteeringControl steeringControl(logger);
    RoadLaneDetectorContours roadLaneDetectorContours;
    // RoadLineDetector roadLineDetector(logger);
    if (!camera.isOpened()) {
        std::cerr << "Error: Couldn't open the camera." << std::endl;
        return -1;
    }
    int frameCount = 0;  // Initialize frame count
    bool stopCar = false;
    steeringControl.start();
    // steeringControl.driveForward(35);
    // std::this_thread::sleep_for(std::chrono::seconds(5));
    // steeringControl.stop();

    while (true) {
        if (!camera.readFrame()) {
            std::cerr << "Error: Could not read frame." << std::endl;
            break;
        }

        int decenteredPixels;
        try {
            decenteredPixels = roadLaneDetectorContours.getXPosition(camera.getCurrentFrame());
        } catch (std::runtime_error er) {
            er.what();
            continue;
        }

        // Display current action on the frame
        std::string actionText = "Action: ";
        // if (!stopCar) {
        if (decenteredPixels != decenteredPixels) {
            actionText += "Nan";
            steeringControl.turnRight(80);
        } else if (std::abs(decenteredPixels) <= 10) {
            actionText += "Centering";
            steeringControl.center();
        } else {
            if (decenteredPixels > 0) {
                actionText += "Turning right";
                steeringControl.turnRight(80);
            } else {
                actionText += "Turning left";
                steeringControl.turnLeft(80);
            }
        }
        cv::Mat frame = camera.getCurrentFrame();
        std::cout << distanceClient.getDistance() << std::endl;
        cv::putText(
            frame,  // Target image
            "distance: " +
                std::to_string((int) distanceClient.getDistance()) +
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
    steeringControl.stop();
    cv::destroyAllWindows();
    videoWriter.release();

    return 0;
}
