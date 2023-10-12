#pragma once

#include <opencv2/opencv.hpp>

class RoadLaneDetector {
   public:
    virtual void processFrame(const cv::Mat frame) = 0;
    virtual int getXPosition(const cv::Mat& frame) = 0;
    virtual std::pair<cv::Vec4i, cv::Vec4i> detectLane(const cv::Mat& frame) = 0;
    virtual std::pair<cv::Vec4i, cv::Vec4i> getLanes() = 0;
};