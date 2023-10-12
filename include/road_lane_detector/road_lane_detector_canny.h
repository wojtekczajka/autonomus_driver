#pragma once
#include <opencv2/opencv.hpp>
// #include "road_lane_detector/road_lane_detector.h"

class RoadLaneDetectorCanny {
   public:
    RoadLaneDetectorCanny();
    void processFrame(const cv::Mat frame);
    int getXPosition(const cv::Mat& frame);
    std::pair<cv::Vec4i, cv::Vec4i> getLanes();

   private:
    void findLeftAndRightLine(const std::pair<std::vector<cv::Point>, std::vector<cv::Point>>& rightAndLeftPoints);
    double calculateDecentering(const int& imageCols, const int& imageHeight);
    int xPosition;
    cv::Vec4i leftVerticalLane, rightVerticalLane;
    bool leftLaneDetected, rightLaneDetected;
    cv::Mat mask;
    std::vector<cv::Point> trianglePoints;
    cv::Mat preprocessFrame(const cv::Mat& frame);
    cv::Mat cropRoiFromFrame(const cv::Mat& frame);
    cv::Rect maskedTopRectangle;
    cv::Rect maskedBottomRectangle;
    cv::Rect maskedLeftRectangle;
};