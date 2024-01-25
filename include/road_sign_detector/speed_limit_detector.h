#pragma once

#include <opencv2/opencv.hpp>

class SpeedLimitDetector {
   public:
    SpeedLimitDetector();
    int detectSpeedLimit(const cv::Mat& frame);
    void detectRedCircles(const cv::Mat& frame);
};