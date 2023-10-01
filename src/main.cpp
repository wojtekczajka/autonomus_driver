#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>

#include "camera/camera.h"
#include "steering_control/steering_control.h"

int main() {
    Logger logger("/dev/null");
    Camera camera(logger);
    SteeringControl steeringControl("/home/ubuntu/git-repos/Steering-for-PiRacer-Standard/steer.py", logger);
    // ad here code which will start and drive forward car with value 50 later sleep by 5 sec and stop car
    if (!camera.isOpened()) {
        std::cerr << "Error: Couldn't open the camera." << std::endl;
        return -1;
    }

    steeringControl.start();
    steeringControl.driveForward(50);
    std::this_thread::sleep_for(std::chrono::seconds(5));
    steeringControl.stop();

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
