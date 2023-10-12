#pragma once

#include "road_lane_detector/road_lane_detector.h"

class RoadLaneDetectorCanny : public RoadLaneDetector {
   public:
    void processFrame(const cv::Mat frame) override;
    int getXPosition(const cv::Mat& frame) override;
    std::pair<cv::Vec4i, cv::Vec4i> getLanes() override; 
    std::pair<cv::Vec4i, cv::Vec4i> detectLane(const cv::Mat& frame) override;

   private:
    int xPosition;
    std::pair<cv::Vec4i, cv::Vec4i> lanes;
};