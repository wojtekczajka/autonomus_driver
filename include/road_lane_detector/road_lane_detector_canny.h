#pragma once

#include <opencv2/opencv.hpp>

#include "common/logger.h"

class RoadLaneDetectorCanny {
   public:
    RoadLaneDetectorCanny();
    void processFrame(const cv::Mat& frame);
    bool isRightVerticalLaneDetected();
    bool isLeftVerticalLaneDetected();
    bool isLeftHorizontalBottomLaneDetected();
    bool isRightHorizontalBottomLaneDetected();
    bool isLeftHorizontalTopLaneDetected();
    bool isRightHorizontalTopLaneDetected();
    int getXPosition();
    int getLeftDistance();
    int getRightDistance();
    std::pair<cv::Vec4f, cv::Vec4f> getLanes();
    cv::Vec4f getRightVerticalLane();
    cv::Vec4f getLeftVerticalLane();
    cv::Vec4f getLeftHorizontalBottomLane();
    cv::Vec4f getRightHorizontalBottomLane();
    cv::Vec4f getLeftHorizontalTopLane();
    cv::Vec4f getRightHorizontalTopLane();

   private:
    cv::Mat filterEdges(const cv::Mat& edgedFrame, const cv::Mat& grayFrame);
    std::vector<cv::Vec4i> detectLines(const cv::Mat& processedFrame);
    void findLanes();
    double calculateDecentering(const int& imageCols, const int& imageHeight);
    cv::Mat preprocessFrame(const cv::Mat& frame);
    cv::Mat cropRoiFromFrame(const cv::Mat& frame);
    void autoCanny(const cv::Mat& frame, double sigma = 0.33);
    void classifyPoints(const std::vector<cv::Vec4i>& lines);
    void classifyHorizontalPoints(const std::vector<cv::Vec4i>& lines);

    bool leftVerticalLaneDetected, rightVerticalLaneDetected, leftHorizontalBottomLaneDetected, leftHorizontalTopLaneDetected, rightHorizontalBottomLaneDetected, rightHorizontalTopLaneDetected;
    int xPosition, frameWidth, frameHeight;
    std::vector<cv::Point> rightPoints, leftPoints, horizontalPoints;
    cv::Vec4f leftVerticalLane, rightVerticalLane, leftHorizontalBottomLane, leftHorizontalTopLane, rightHorizontalBottomLane, rightHorizontalTopLane;
    std::vector<cv::Vec4i> leftBottomLines, rightBottomLines;
    cv::Mat frameAfterCanny, bottomCircleMask;
    cv::Point mostDistantPointOnLeftVerticalLane, mostDistantPointOnRightVerticalLane;
};