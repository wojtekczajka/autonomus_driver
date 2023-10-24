#pragma once

#include <opencv2/opencv.hpp>

#include "camera/frame_dispatcher_client.h"
#include "common/logger.h"

class RoadLaneDetectorCanny {
   public:
    RoadLaneDetectorCanny(FrameDispatcherClient& frameDispatcherClient);
    void processFrame(const cv::Mat frame);
    bool isRightVerticalLaneDetected();
    bool isLeftVerticalLaneDetected();
    bool isTopHorizontalLaneDetected();
    int getXPosition();
    std::pair<cv::Vec4f, cv::Vec4f> getLanes();
    cv::Vec4f getRightVerticalLane();
    cv::Vec4f getLeftVerticalLane();
    cv::Vec4f getTopHorizontalLane();

   private:
    cv::Mat filterEdges(cv::Mat& edgedFrame, const cv::Mat& grayFrame);
    std::vector<cv::Vec4i> detectLines(const cv::Mat& processedFrame);
    void findLanes();
    double calculateDecentering(const int& imageCols, const int& imageHeight);
    cv::Mat preprocessFrame(const cv::Mat& frame);
    cv::Mat cropRoiFromFrame(const cv::Mat& frame);
    void classifyPoints(const std::vector<cv::Vec4i>& lines);

    FrameDispatcherClient& frameDispatcherClient;
    bool leftVerticalLaneDetected, rightVerticalLaneDetected, topHorizontalLaneDetected;
    int xPosition, frameWidth, frameHeight;
    std::vector<cv::Point> rightPoints, leftPoints, horizontalPoints;
    cv::Mat topCircleMask, bottomCircleMask;
    cv::Vec4f leftVerticalLane, rightVerticalLane, topHorizontalLane;
};