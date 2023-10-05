#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>

#include "camera/camera.h"
#include "road_line_detector/road_line_detector.h"
#include "steering_control/steering_control.h"

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
    Logger logger("/dev/null");
    Camera camera(logger);
    SteeringControl steeringControl(logger);
    RoadLineDetector roadLineDetector(logger);
    if (!camera.isOpened()) {
        std::cerr << "Error: Couldn't open the camera." << std::endl;
        return -1;
    }
    int frameCount = 0;  // Initialize frame count
    bool stopCar = false;
    steeringControl.start();
    steeringControl.driveForward(20);
    // std::this_thread::sleep_for(std::chrono::seconds(5));
    // steeringControl.stop();

    while (true) {
        if (!camera.readFrame()) {
            std::cerr << "Error: Could not read frame." << std::endl;
            break;
        }
        std::pair<cv::Vec4i, cv::Vec4i> leftAndRightRoadLine = roadLineDetector.getRightAndLeftRoadLine(camera.getCurrentFrame());
        std::pair<cv::Point2f, cv::Point2f> leftLineinePoints = vectorToLinePointss(leftAndRightRoadLine.second, 288, 480);
        std::pair<cv::Point2f, cv::Point2f> rightLineinePoints = vectorToLinePointss(leftAndRightRoadLine.first, 288, 480);
        cv::Mat frame = camera.getCurrentFrame();
        cv::line(frame, leftLineinePoints.first, leftLineinePoints.second, cv::Scalar(255, 0, 0), 2);
        cv::line(frame, rightLineinePoints.first, rightLineinePoints.second, cv::Scalar(255, 0, 0), 2);

        int frameHeight = frame.rows;
        int yCoordinate = static_cast<int>(0.8 * frameHeight);
        float leftX = leftLineinePoints.first.x + (leftLineinePoints.second.x - leftLineinePoints.first.x) * (yCoordinate - leftLineinePoints.first.y) / (leftLineinePoints.second.y - leftLineinePoints.first.y);
        float rightX = rightLineinePoints.first.x + (rightLineinePoints.second.x - rightLineinePoints.first.x) * (yCoordinate - rightLineinePoints.first.y) / (rightLineinePoints.second.y - rightLineinePoints.first.y);
        float distance = rightX - leftX;
        float frameCenter = static_cast<float>(frame.cols) / 2.0;
        float decenteredPixels = frameCenter - ((leftX + rightX) / 2.0);
        std::cout << "Distance between lines at y = " << yCoordinate << ": " << distance << " pixels" << std::endl;
        std::cout << "Decentered by " << decenteredPixels << " pixels" << std::endl;
        cv::line(frame, cv::Point(0, yCoordinate), cv::Point(frame.cols, yCoordinate), cv::Scalar(0, 255, 0), 2);
        int circleRadius = 5;
        cv::circle(frame, cv::Point(static_cast<int>(leftX), yCoordinate), circleRadius, cv::Scalar(0, 0, 255), -1);   // Red circle for left point
        cv::circle(frame, cv::Point(static_cast<int>(rightX), yCoordinate), circleRadius, cv::Scalar(0, 0, 255), -1);  // Red circle for right point

        if (!stopCar) {
            if (frameCount % 2 == 0) {
                if (std::abs(decenteredPixels) <= 10) {
                    std::cout << "centring" << std::endl;
                    steeringControl.center();
                }
                else if (decenteredPixels < 0) {
                    steeringControl.turnRight(30);
                    std::cout << "Turning right" << std::endl;
                } else if (decenteredPixels > 0) {
                    steeringControl.turnLeft(30);
                    std::cout << "Turning left" << std::endl;
                }
            }

            if (frameCount >= 300) {
                // After 100 frames, stop the car
                steeringControl.stop();
                stopCar = true;
                std::cout << "Stopping the car" << std::endl;
                break;
            }
        }

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

        if (cv::waitKey(1) == 'q') {
            break;
        }
        frameCount++;
    }

    // Release the camera capture object
    cv::destroyAllWindows();

    return 0;
}
