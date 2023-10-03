#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>

#include "camera/camera.h"
#include "steering_control/steering_control.h"
#include "road_line_detector/road_line_detector.h"

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
        roadLineDetector.processFrame(camera.getCurrentFrame());

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
