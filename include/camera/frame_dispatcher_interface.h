#pragma once

#include <opencv2/opencv.hpp>

class IFrameDispatcherClient {
public:
    virtual ~IFrameDispatcherClient() = default;
    virtual void sendFrame(const cv::Mat& frame, const std::string& frameName) = 0;
};