#pragma once

#include <opencv2/opencv.hpp>

#include "common/logger.h"

class RoadLaneDetectorCanny {
   public:
    RoadLaneDetectorCanny();
    void processFrame(const cv::Mat& frame);
    bool isRightVerticalLaneDetected();
    bool isLeftVerticalLaneDetected();
    bool isTurnRightDetected();
    bool isTurnLeftDetected();
    bool isRoadForwardBeyondIntersectionDetected();
    int getXPosition();
    std::pair<cv::Vec4f, cv::Vec4f> getLanes();
    cv::Vec4f getRightVerticalLane();
    cv::Vec4f getLeftVerticalLane();

   private:
    static constexpr int MIN_LANES_DISTANCE = 125;
    static constexpr int MAX_LANES_DISTANCE = 225;

    cv::Mat filterEdges(const cv::Mat& edgedFrame, const cv::Mat& grayFrame);
    std::vector<cv::Vec4i> detectLines(const cv::Mat& processedFrame);
    void findLanes();
    void findTurns();
    void findRoadForwardBeyondIntersectionDetected();
    double calculateDecentering(const int& imageCols, const int& imageHeight);
    cv::Mat preprocessFrame(const cv::Mat& frame);
    cv::Mat cropRoiFromFrame(const cv::Mat& frame);
    void autoCanny(const cv::Mat& frame, double sigma = 0.33);
    void classifyPoints(const std::vector<cv::Vec4i>& lines);

    bool leftVerticalLaneDetected, rightVerticalLaneDetected, turnLeftDetected, turnRightDetected, roadForwardBeyondIntersectionDetected;
    int xPosition, frameWidth, frameHeight, leftMidpointY, rightMidpointY;
    std::vector<cv::Point> rightPoints, leftPoints, horizontalPoints;
    cv::Vec4f leftVerticalLane, rightVerticalLane;
    cv::Mat frameAfterCanny, bottomCircleMask, preprocessedFrame;
    cv::Rect upperRoi, lowerRoi;
};