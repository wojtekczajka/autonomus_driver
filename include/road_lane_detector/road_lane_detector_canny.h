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
    void findLanes();
    double calculateDecentering(const int& imageCols, const int& imageHeight);
    cv::Mat preprocessFrame(const cv::Mat& frame);
    cv::Mat cropRoiFromFrame(const cv::Mat& frame);
    void classifyPoints(const std::vector<cv::Vec4i>& lines);

    bool leftVerticalLaneDetected, rightVerticalLaneDetected, topHorizontalLaneDetected;
    int xPosition;
    std::vector<cv::Point> trianglePoints;
    std::vector<cv::Point> rightPoints, leftPoints, horizontalPoints;
    cv::Mat mask;
    cv::Rect maskedTopRectangle;
    cv::Rect maskedBottomRectangle;
    cv::Rect maskedLeftRectangle;
    cv::Vec4i leftVerticalLane, rightVerticalLane, topHorizontalLane;
};