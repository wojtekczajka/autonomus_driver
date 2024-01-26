#pragma once

#include <leptonica/allheaders.h>
#include <tesseract/baseapi.h>

#include <opencv2/opencv.hpp>

class SpeedLimitDetector {
   public:
    SpeedLimitDetector();
    ~SpeedLimitDetector();
    int detectSpeedLimit(const cv::Mat& frame);
    void detectRedCircles(const cv::Mat& frame);

   private:
    tesseract::TessBaseAPI* tessApi;
    int recognizeSpeed(const cv::Mat& frame, const cv::Rect& rect);
};