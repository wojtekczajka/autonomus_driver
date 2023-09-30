#pragma once

#include <opencv2/opencv.hpp>
#include <chrono>

#include "common/logger.h"

class Camera {
public:
    Camera();
    ~Camera();

    void setCameraConfig();
    cv::Mat getCurrentFrame() const;
    bool isOpened();
    bool readFrame();
    double getFPS();

private:
    cv::VideoCapture cap;
    int fpsCounter;
    std::chrono::time_point<std::chrono::steady_clock> fpsStartTime;
    cv::Mat currentFrame;  // Member variable to store the current frame
    Logger logger;
};
