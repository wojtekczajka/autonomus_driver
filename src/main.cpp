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
    RoadLineDetector roadLineDetector;
    if (!camera.isOpened()) {
        std::cerr << "Error: Couldn't open the camera." << std::endl;
        return -1;
    }

    // steeringControl.start();
    // steeringControl.driveForward(50);
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
        cv::imshow("line detector result", frame);
        cv::waitKey(1);

        std::string fpsString = "FPS: " + std::to_string(camera.getFPS());

        // Add the FPS text to the frame
        // cv::putText(
        //     roadDetection.getEdgeDetectionResult(),   // Target image
        //     fpsString,                                // Text to be added
        //     cv::Point(10, 30),                        // Position
        //     cv::FONT_HERSHEY_SIMPLEX,                 // Font type
        //     1.0,                                      // Font scale
        //     cv::Scalar(255, 255, 255),               // Text color (white)
        //     2,                                       // Text thickness
        //     cv::LINE_AA                               // Line type
        // );
        // cv::imshow("Camera Frame", roadDetection.getEdgeDetectionResult());

        if (cv::waitKey(1) == 'q') {
            break;
        }
    }

    // Release the camera capture object
    cv::destroyAllWindows();

    return 0;
}
