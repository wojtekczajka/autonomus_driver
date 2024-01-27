#pragma once

#include <opencv2/opencv.hpp>

class TurnSignDetector {
   public:
    TurnSignDetector();
    void detectTurnSign(const cv::Mat& frame);
};