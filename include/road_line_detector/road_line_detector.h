#pragma once

#include <opencv2/opencv.hpp>

class RoadLineDetector {
public:
    RoadLineDetector();
    ~RoadLineDetector();

    void processFrame(const cv::Mat& frame);
    cv::Mat getEdgeDetectionResult() const;

private:
    cv::Mat frame;
    cv::Mat cropRoiFromFrame(const cv::Mat& frame);
    cv::Mat convertFrameToGrayscale(const cv::Mat& frame);
};