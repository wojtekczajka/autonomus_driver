#pragma once

#include <opencv2/opencv.hpp>

#include "common/logger.h"

class RoadLineDetector {
   public:
    RoadLineDetector(Logger& logger);
    ~RoadLineDetector();

    std::pair<cv::Vec4i, cv::Vec4i> getRightAndLeftRoadLine(const cv::Mat& frame);

   private:
    cv::Mat preprocessFrame(const cv::Mat& frame);
    std::vector<cv::Vec4i> detectLines(const cv::Mat& processedFrame);
    cv::Mat visualizeLines(const cv::Mat& frame, const std::vector<cv::Vec4i>& lines);
    std::pair<std::vector<cv::Point>, std::vector<cv::Point>> findRightAndLeftPoints(const std::vector<cv::Vec4i>& lines);
    std::pair<cv::Vec4i, cv::Vec4i> findLeftAndRightLine(const std::pair<std::vector<cv::Point>, std::vector<cv::Point>>& rightAndLeftPoints);
    cv::Mat cropRoiFromFrame(const cv::Mat& frame);
    cv::Mat convertFrameToGrayscale(const cv::Mat& frame);

    cv::Vec4i lastDetectedRightLine;
    cv::Vec4i lastDetectedLeftLine;
    Logger& logger;
};