#pragma once
#include <opencv2/opencv.hpp>
// #include "road_lane_detector/road_lane_detector.h"

class RoadLaneDetectorCanny {
   public:
    RoadLaneDetectorCanny();
    void processFrame(const cv::Mat frame);
    bool isRightVerticalLaneDetected();
    bool isLeftVerticalLaneDetected();
    bool isTopHorizontalLaneDetected();
    int getXPosition();
    std::pair<cv::Vec4i, cv::Vec4i> getLanes();
    cv::Vec4i getRightVerticalLane();
    cv::Vec4i getLeftVerticalLane();
    cv::Vec4i getTopHorizontalLane();

   private:
    void findLanes();
    double calculateDecentering(const int& imageCols, const int& imageHeight);
    cv::Mat preprocessFrame(const cv::Mat& frame);
    cv::Mat cropRoiFromFrame(const cv::Mat& frame);
    void classifyPoints(const std::vector<cv::Vec4i>& lines);

    bool leftVerticalLaneDetected, rightVerticalLaneDetected, topHorizontalLaneDetected;
    int xPosition, frameWidth, frameHeight;
    std::vector<cv::Point> rightPoints, leftPoints, horizontalPoints;
    cv::Mat topCircleMask, bottomCircleMask;
    cv::Vec4i leftVerticalLane, rightVerticalLane, topHorizontalLane;
};