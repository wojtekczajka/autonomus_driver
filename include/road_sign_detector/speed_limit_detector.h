#pragma once

#include <leptonica/allheaders.h>
#include <tesseract/baseapi.h>
#include <memory.h>

#include <opencv2/opencv.hpp>

class SpeedLimitDetector {
   public:
    SpeedLimitDetector();
    ~SpeedLimitDetector();
    void detectSpeedLimit(const cv::Mat& frame);
    bool isSignDetected();
    cv::Rect getSignPosition();
    int getSpeedLimitValue();
    std::string getSpeedLimitValueStr();

   private:
    bool signDetected;
    cv::Rect signPosition;
    int speedLimitValue;
    std::unique_ptr<char[]> text;
    std::unique_ptr<tesseract::TessBaseAPI> tessApi;
    int recognizeSpeed(const cv::Mat& frame);
    std::vector<std::vector<cv::Point>> contours;
};