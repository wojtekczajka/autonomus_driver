#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>

#include "camera/camera.h"

int main() {
    Camera camera;
    if (!camera.isOpened()) {
        std::cerr << "Error: Couldn't open the camera." << std::endl;
        return -1;
    }

    while (true) {
        if (!camera.readFrame()) {
            std::cerr << "Error: Could not read frame." << std::endl;
            break;
        }
        cv::imshow("Camera Frame", camera.getCurrentFrame());
        camera.getFPS();
        if (cv::waitKey(1) == 'q') {
            break;
        }
    }

    // Release the camera capture object
    cv::destroyAllWindows();

    return 0;
}
